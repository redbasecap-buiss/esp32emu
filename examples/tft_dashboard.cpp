// TFT_eSPI dashboard display example
#include "Arduino.h"
#include "TFT_eSPI.h"
#include <cstdio>

TFT_eSPI tft;
TFT_eSprite gauge(&tft);

void setup() {
    Serial.begin(115200);
    tft.init();
    tft.setRotation(1);  // landscape
    tft.fillScreen(TFT_BLACK);

    // Title
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextDatum(TC_DATUM);
    tft.drawString("ESP32 Dashboard", tft.width()/2, 10);

    // Temperature box
    tft.drawRoundRect(10, 50, 150, 80, 8, TFT_WHITE);
    tft.setTextColor(TFT_YELLOW);
    tft.setTextSize(1);
    tft.drawString("Temperature", 85, 55);
    tft.setTextSize(3);
    tft.setTextColor(TFT_GREEN);
    tft.drawFloat(23.5f, 1, 85, 80);

    // Humidity box
    tft.drawRoundRect(170, 50, 150, 80, 8, TFT_WHITE);
    tft.setTextColor(TFT_YELLOW);
    tft.setTextSize(1);
    tft.drawString("Humidity", 245, 55);
    tft.setTextSize(3);
    tft.setTextColor(TFT_SKYBLUE);
    tft.drawFloat(65.2f, 1, 245, 80);

    // Sprite gauge
    gauge.createSprite(60, 60);
    gauge.fillScreen(TFT_BLACK);
    gauge.drawCircle(30, 30, 28, TFT_GREEN);
    gauge.setTextDatum(MC_DATUM);
    gauge.setTextColor(TFT_WHITE);
    gauge.drawString("OK", 30, 30);
    gauge.pushSprite(130, 150);
    gauge.deleteSprite();

    printf("Dashboard drawn: %u draws, %u fills, %u text ops\n",
           tft.getDrawCount(), tft.getFillCount(), tft.getTextCount());
    printf("Display: %dx%d (landscape)\n", tft.width(), tft.height());
}

void loop() {
    delay(1000);
}
