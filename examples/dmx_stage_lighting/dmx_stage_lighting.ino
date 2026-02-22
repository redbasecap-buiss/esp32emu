// ESP-DMX — Stage lighting controller with multiple fixtures
#include <Arduino.h>
#include <esp_dmx.h>

dmx::DMX dmxPort;

// Fixture addresses (each fixture uses 4 channels: R, G, B, Dimmer)
#define FIXTURE_1_ADDR 1
#define FIXTURE_2_ADDR 5
#define FIXTURE_3_ADDR 9
#define CHANNELS_PER_FIXTURE 4

struct Color { uint8_t r, g, b; };
const Color WARM_WHITE = {255, 200, 150};
const Color COOL_BLUE  = {0, 50, 255};
const Color AMBER      = {255, 150, 0};

void setFixture(uint16_t addr, Color c, uint8_t dimmer) {
    dmxPort.write(addr, c.r);
    dmxPort.write(addr + 1, c.g);
    dmxPort.write(addr + 2, c.b);
    dmxPort.write(addr + 3, dimmer);
}

void setup() {
    Serial.begin(115200);

    dmx::Config cfg;
    cfg.uart_num = 1;
    cfg.tx_pin = 17;
    cfg.rx_pin = 16;
    cfg.en_pin = 21;
    cfg.type = DMX_TYPE_SEND;
    dmxPort.install(cfg);

    Serial.println("Stage Lighting Controller — 3 Fixtures");
}

void loop() {
    // Scene 1: Warm wash
    Serial.println("Scene: Warm Wash");
    setFixture(FIXTURE_1_ADDR, WARM_WHITE, 255);
    setFixture(FIXTURE_2_ADDR, WARM_WHITE, 200);
    setFixture(FIXTURE_3_ADDR, WARM_WHITE, 255);
    dmxPort.send();
    delay(3000);

    // Scene 2: Cool accent
    Serial.println("Scene: Cool Accent");
    setFixture(FIXTURE_1_ADDR, COOL_BLUE, 180);
    setFixture(FIXTURE_2_ADDR, AMBER, 255);
    setFixture(FIXTURE_3_ADDR, COOL_BLUE, 180);
    dmxPort.send();
    delay(3000);

    // Scene 3: Blackout
    Serial.println("Scene: Blackout");
    setFixture(FIXTURE_1_ADDR, {0,0,0}, 0);
    setFixture(FIXTURE_2_ADDR, {0,0,0}, 0);
    setFixture(FIXTURE_3_ADDR, {0,0,0}, 0);
    dmxPort.send();
    delay(2000);
}
