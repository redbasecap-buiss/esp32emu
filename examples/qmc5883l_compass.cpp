#include <Arduino.h>
#include <QMC5883L.h>

QMC5883L compass;

void setup() {
    Serial.begin(115200);
    Serial.println("QMC5883L Digital Compass");

    Wire.begin();
    if (!compass.begin()) {
        Serial.println("Failed to find QMC5883L!");
        while (1) delay(10);
    }

    compass.setRange(QMC5883L_RANGE_2G);
    compass.setOutputDataRate(QMC5883L_ODR_50HZ);
    compass.setOverSampling(QMC5883L_OSR_512);
    compass.setCalibration(0, 0, 0); // Adjust after calibration

    Serial.println("Compass ready!");
}

const char* headingToDirection(float heading) {
    if (heading < 22.5 || heading >= 337.5) return "N";
    if (heading < 67.5) return "NE";
    if (heading < 112.5) return "E";
    if (heading < 157.5) return "SE";
    if (heading < 202.5) return "S";
    if (heading < 247.5) return "SW";
    if (heading < 292.5) return "W";
    return "NW";
}

void loop() {
    if (compass.dataReady()) {
        int16_t x, y, z;
        compass.read(&x, &y, &z);

        float heading = compass.getHeading();
        int16_t temp = compass.readTemperature();

        Serial.print("X: "); Serial.print(x);
        Serial.print("  Y: "); Serial.print(y);
        Serial.print("  Z: "); Serial.println(z);
        Serial.print("Heading: "); Serial.print(heading, 1);
        Serial.print("° ("); Serial.print(headingToDirection(heading));
        Serial.println(")");
        Serial.print("Temp: "); Serial.print(temp); Serial.println("°C");
        Serial.println("---");
    }
    delay(500);
}
