// ST7789 TFT display demo
#include <Adafruit_ST7789.h>

#define TFT_CS   5
#define TFT_DC   16
#define TFT_RST  17

Adafruit_ST7789 tft(TFT_CS, TFT_DC, TFT_RST);

void setup() {
    Serial.begin(115200);
    tft.init(240, 240);
    tft.setRotation(2);
    tft.fillScreen(0x0000);
    Serial.println("ST7789 ready!");

    // Draw color bars
    uint16_t colors[] = {0xF800, 0x07E0, 0x001F, 0xFFE0, 0xF81F, 0x07FF};
    int barWidth = 240 / 6;
    for (int i = 0; i < 6; i++) {
        tft.fillRect(i * barWidth, 0, barWidth, 240, colors[i]);
    }
    Serial.println("Color bars drawn");
}

void loop() {
    delay(1000);
}
