// ESP-IDF UART Echo Example
// Configures UART1 and echoes received bytes back
#include <Arduino.h>
#include "driver/uart.h"
#include <cstring>

#define UART_PORT UART_NUM_1
#define BUF_SIZE  256

void setup() {
    Serial.begin(115200);

    uart_config_t uart_config = {};
    uart_config.baud_rate = 115200;
    uart_config.data_bits = UART_DATA_8_BITS;
    uart_config.parity    = UART_PARITY_DISABLE;
    uart_config.stop_bits = UART_STOP_BITS_1;
    uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;

    uart_param_config(UART_PORT, &uart_config);
    uart_set_pin(UART_PORT, 17, 16, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_PORT, BUF_SIZE * 2, 0, 0, nullptr, 0);

    Serial.println("UART Echo ready on UART1");
}

void loop() {
    uint8_t buf[BUF_SIZE];
    int len = uart_read_bytes(UART_PORT, buf, BUF_SIZE, 20);
    if (len > 0) {
        Serial.printf("Received %d bytes, echoing back\n", len);
        uart_write_bytes(UART_PORT, buf, len);
    }
}
