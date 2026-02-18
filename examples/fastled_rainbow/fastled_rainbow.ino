// FastLED rainbow animation example
#include <Arduino.h>
#include <FastLED.h>

#define NUM_LEDS 16
#define DATA_PIN 5
#define BRIGHTNESS 128

CRGB leds[NUM_LEDS];
uint8_t hue = 0;

void setup() {
    Serial.begin(115200);
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);
    Serial.println("FastLED Rainbow initialized");
}

void loop() {
    fill_rainbow(leds, NUM_LEDS, hue, 16);
    FastLED.show();
    hue++;

    Serial.print("Rainbow cycle, hue=");
    Serial.println(hue);
    delay(50);
}
