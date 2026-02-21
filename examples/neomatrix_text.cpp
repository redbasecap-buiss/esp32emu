#include <Arduino.h>
#include <Adafruit_NeoMatrix.h>

// 8x8 NeoPixel matrix on pin 6
Adafruit_NeoMatrix matrix(8, 8, 6,
    NEO_MATRIX_TOP + NEO_MATRIX_RIGHT + NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
    NEO_GRB + NEO_KHZ800);

const uint16_t colors[] = {
    Adafruit_NeoMatrix::Color(255, 0, 0),   // Red
    Adafruit_NeoMatrix::Color(0, 255, 0),   // Green
    Adafruit_NeoMatrix::Color(0, 0, 255),   // Blue
    Adafruit_NeoMatrix::Color(255, 255, 0)  // Yellow
};

int colorIdx = 0;
int x = matrix.width();

void setup() {
    Serial.begin(115200);
    Serial.println("NeoMatrix Scrolling Text");

    matrix.begin();
    matrix.setTextWrap(false);
    matrix.setBrightness(40);
    matrix.setTextColor(colors[0]);
}

void loop() {
    matrix.fillScreen(0);
    matrix.setCursor(x, 0);
    matrix.print("Hello!");

    if (--x < -36) {
        x = matrix.width();
        colorIdx = (colorIdx + 1) % 4;
        matrix.setTextColor(colors[colorIdx]);
    }

    matrix.show();
    delay(100);
}
