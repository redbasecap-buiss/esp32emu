#include "QMC5883L.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    printf("test_qmc5883l: ");

    QMC5883L mag;
    assert(mag.begin());
    assert(mag.dataReady());

    // Default readings
    assert(mag.readX() == 100);
    assert(mag.readY() == -50);
    assert(mag.readZ() == 200);

    // Set test data
    mag._setXYZ(300, 400, 0);
    int16_t x, y, z;
    mag.read(&x, &y, &z);
    assert(x == 300);
    assert(y == 400);
    assert(z == 0);

    // Heading calculation
    float heading = mag.getHeading();
    assert(heading >= 0 && heading < 360);
    // atan2(400, 300) ≈ 53.13°
    assert(fabs(heading - 53.13f) < 1.0f);

    // Pointing north (Y=positive, X=0) → 90°
    mag._setXYZ(0, 100, 0);
    heading = mag.getHeading();
    assert(fabs(heading - 90.0f) < 0.1f);

    // Calibration offsets
    mag.setCalibration(10, -10, 5);
    mag._setXYZ(100, 50, 200);
    assert(mag.readX() == 110);
    assert(mag.readY() == 40);
    assert(mag.readZ() == 205);

    // Configuration
    mag.setRange(QMC5883L_RANGE_8G);
    assert(mag.getRange() == QMC5883L_RANGE_8G);
    mag.setOutputDataRate(QMC5883L_ODR_200HZ);
    assert(mag.getOutputDataRate() == QMC5883L_ODR_200HZ);
    mag.setOverSampling(QMC5883L_OSR_64);
    assert(mag.getOverSampling() == QMC5883L_OSR_64);

    // Temperature
    mag._setTemperature(30);
    assert(mag.readTemperature() == 30);

    // Reset
    mag.reset();
    assert(mag.readX() == 0);
    assert(mag.readY() == 0);
    assert(mag.readZ() == 0);

    printf("all assertions passed\n");
    return 0;
}
