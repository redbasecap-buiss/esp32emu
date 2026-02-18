// ESP32 CAN bus (TWAI) example — send and receive CAN frames
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <driver/twai.h>

void setup() {
    Serial.begin(115200);
    Serial.println("ESP32 CAN Bus (TWAI) Demo");

    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(21, 22, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    if (twai_driver_install(&g_config, &t_config, &f_config) != ESP_OK) {
        Serial.println("Failed to install TWAI driver");
        return;
    }
    if (twai_start() != ESP_OK) {
        Serial.println("Failed to start TWAI driver");
        return;
    }
    Serial.println("TWAI driver started @ 500kbps");
}

void loop() {
    // Send a heartbeat frame every second
    twai_message_t tx_msg;
    memset(&tx_msg, 0, sizeof(tx_msg));
    tx_msg.identifier = 0x100;
    tx_msg.data_length_code = 2;
    tx_msg.data[0] = (millis() >> 8) & 0xFF;
    tx_msg.data[1] = millis() & 0xFF;

    if (twai_transmit(&tx_msg, pdMS_TO_TICKS(100)) == ESP_OK) {
        Serial.print("TX ID=0x100 data=");
        Serial.print(tx_msg.data[0], HEX);
        Serial.print(" ");
        Serial.println(tx_msg.data[1], HEX);
    }

    // Check for received frames
    twai_message_t rx_msg;
    if (twai_receive(&rx_msg, pdMS_TO_TICKS(50)) == ESP_OK) {
        Serial.print("RX ID=0x");
        Serial.print(rx_msg.identifier, HEX);
        Serial.print(" len=");
        Serial.print(rx_msg.data_length_code);
        Serial.print(" data=");
        for (int i = 0; i < rx_msg.data_length_code; i++) {
            Serial.print(rx_msg.data[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
    }

    delay(1000);
}
