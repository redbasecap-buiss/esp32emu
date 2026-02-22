// esp32emu example — E-Paper Display with GxEPD2
#include "Arduino.h"
#include "GxEPD2.h"

// 2.9" e-paper display
GxEPD2_BW<GxEPD2_290_T94, 128> display(5, 17, 16, 4);

void setup() {
    Serial.begin(115200);
    Serial.println("E-Paper Display Demo");

    display.init();
    display.setRotation(1);

    // Draw welcome screen
    display.firstPage();
    do {
        display.fillScreen(GxEPD_WHITE);
        display.setCursor(10, 20);
        display.setTextColor(GxEPD_BLACK);
        display.setTextSize(2);
        display.println("esp32emu");
        display.setTextSize(1);
        display.println("E-Paper Demo");

        // Draw border
        display.drawRect(0, 0, display.width(), display.height(), GxEPD_BLACK);

        // Draw some boxes
        display.fillRect(10, 70, 40, 30, GxEPD_BLACK);
        display.fillRect(60, 70, 40, 30, GxEPD_BLACK);
    } while (display.nextPage());

    Serial.println("Display updated");

    // Partial update example
    display.setPartialWindow(10, 20, 100, 30);
    display.display(true);

    // Power management
    display.hibernate();
}

void loop() {
    delay(60000); // E-paper retains image
}
