// esp32emu — ESP-IDF UART driver mock test
#include "driver/uart.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    // Configure UART0
    uart_config_t cfg = {};
    cfg.baud_rate = 115200;
    cfg.data_bits = UART_DATA_8_BITS;
    cfg.parity = UART_PARITY_DISABLE;
    cfg.stop_bits = UART_STOP_BITS_1;
    cfg.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;

    assert(uart_param_config(UART_NUM_0, &cfg) == ESP_OK);
    assert(uart_set_pin(UART_NUM_0, 1, 3, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE) == ESP_OK);
    assert(uart_driver_install(UART_NUM_0, 1024, 0, 0, nullptr, 0) == ESP_OK);

    // Write data
    const char* msg = "Hello UART";
    int written = uart_write_bytes(UART_NUM_0, msg, strlen(msg));
    assert(written == (int)strlen(msg));

    // Verify TX data via emulator API
    auto txData = esp32emu::UartDriver::instance().port(UART_NUM_0).getTxData();
    assert(txData.size() == strlen(msg));
    assert(memcmp(txData.data(), msg, strlen(msg)) == 0);

    // Inject RX data and read back
    const uint8_t rxIn[] = {0xAA, 0xBB, 0xCC};
    esp32emu::UartDriver::instance().port(UART_NUM_0).injectRx(rxIn, 3);

    size_t avail = 0;
    assert(uart_get_buffered_data_len(UART_NUM_0, &avail) == ESP_OK);
    assert(avail == 3);

    uint8_t rxOut[8] = {};
    int nread = uart_read_bytes(UART_NUM_0, rxOut, sizeof(rxOut), 100);
    assert(nread == 3);
    assert(rxOut[0] == 0xAA);
    assert(rxOut[1] == 0xBB);
    assert(rxOut[2] == 0xCC);

    // After read, buffer is empty
    assert(uart_get_buffered_data_len(UART_NUM_0, &avail) == ESP_OK);
    assert(avail == 0);

    // Flush and delete
    assert(uart_flush(UART_NUM_0) == ESP_OK);
    assert(uart_driver_delete(UART_NUM_0) == ESP_OK);

    // Multiple ports
    assert(uart_param_config(UART_NUM_1, &cfg) == ESP_OK);
    assert(uart_driver_install(UART_NUM_1, 256, 0, 0, nullptr, 0) == ESP_OK);
    esp32emu::UartDriver::instance().port(UART_NUM_1).injectRx((const uint8_t*)"AB", 2);
    uint8_t b[2];
    assert(uart_read_bytes(UART_NUM_1, b, 2, 0) == 2);
    assert(b[0] == 'A' && b[1] == 'B');
    assert(uart_driver_delete(UART_NUM_1) == ESP_OK);

    // Invalid arg
    assert(uart_param_config(UART_NUM_0, nullptr) == ESP_ERR_INVALID_ARG);
    assert(uart_get_buffered_data_len(UART_NUM_0, nullptr) == ESP_ERR_INVALID_ARG);

    printf("test_uart_driver: all assertions passed\n");
    return 0;
}
