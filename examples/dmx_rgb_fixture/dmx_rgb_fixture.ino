// ESP-DMX — Control an RGB light fixture via DMX-512
#include <Arduino.h>
#include <esp_dmx.h>

dmx::DMX dmxPort;

void setup() {
    Serial.begin(115200);

    dmx::Config cfg;
    cfg.uart_num = 1;
    cfg.tx_pin = 17;
    cfg.rx_pin = 16;
    cfg.en_pin = 21;
    cfg.type = DMX_TYPE_SEND;
    dmxPort.install(cfg);

    // RGB fixture at DMX address 1 (channels 1=R, 2=G, 3=B, 4=Dimmer)
    dmxPort.setStartCode(0x00);
    Serial.println("DMX RGB Fixture Controller Ready");
}

void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t dimmer) {
    dmxPort.write(1, r);
    dmxPort.write(2, g);
    dmxPort.write(3, b);
    dmxPort.write(4, dimmer);
    dmxPort.send();
    Serial.printf("DMX → R:%u G:%u B:%u Dim:%u\n", r, g, b, dimmer);
}

void loop() {
    // Color chase sequence
    setColor(255, 0, 0, 255);    // Red
    delay(1000);
    setColor(0, 255, 0, 255);    // Green
    delay(1000);
    setColor(0, 0, 255, 255);    // Blue
    delay(1000);
    setColor(255, 255, 255, 128); // White, half brightness
    delay(1000);
}
