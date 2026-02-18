// test_twai: ESP32 TWAI (CAN bus) driver mock tests
#include "driver/twai.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    twai_test::reset();

    // Install driver
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(21, 22, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    assert(twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK);
    assert(twai_driver_install(&g_config, &t_config, &f_config) == ESP_ERR_INVALID_STATE); // double install

    // Start
    assert(twai_start() == ESP_OK);
    assert(twai_start() == ESP_ERR_INVALID_STATE); // double start

    // Transmit
    twai_message_t tx_msg;
    memset(&tx_msg, 0, sizeof(tx_msg));
    tx_msg.identifier = 0x100;
    tx_msg.data_length_code = 4;
    tx_msg.data[0] = 0xDE;
    tx_msg.data[1] = 0xAD;
    tx_msg.data[2] = 0xBE;
    tx_msg.data[3] = 0xEF;

    assert(twai_transmit(&tx_msg, 100) == ESP_OK);
    assert(twai_test::getTxCount() == 1);
    assert(twai_test::getTxLog().size() == 1);
    assert(twai_test::getTxLog()[0].identifier == 0x100);
    assert(twai_test::getTxLog()[0].data[0] == 0xDE);

    // Receive — inject a message
    uint8_t rxdata[] = {0x01, 0x02, 0x03};
    twai_test::injectRxMessage(0x200, rxdata, 3);

    twai_message_t rx_msg;
    assert(twai_receive(&rx_msg, 100) == ESP_OK);
    assert(rx_msg.identifier == 0x200);
    assert(rx_msg.data_length_code == 3);
    assert(rx_msg.data[0] == 0x01);
    assert(rx_msg.data[2] == 0x03);
    assert(twai_test::getRxCount() == 1);

    // Receive empty → timeout
    assert(twai_receive(&rx_msg, 0) == ESP_ERR_TIMEOUT);

    // Extended frame
    uint8_t extdata[] = {0xFF};
    twai_test::injectRxMessage(0x1FFFFFFF, extdata, 1, true);
    assert(twai_receive(&rx_msg, 100) == ESP_OK);
    assert(rx_msg.identifier == 0x1FFFFFFF);
    assert(rx_msg.flags.extd == 1);

    // Status
    twai_status_info_t status;
    assert(twai_get_status_info(&status) == ESP_OK);
    assert(status.state == 1); // started

    // Stop and uninstall
    assert(twai_stop() == ESP_OK);
    assert(twai_driver_uninstall() == ESP_OK);

    printf("test_twai: all assertions passed\n");
    return 0;
}
