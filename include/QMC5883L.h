#pragma once
#include "Arduino.h"
#include "Wire.h"

// QMC5883L 3-axis magnetometer/compass mock

#define QMC5883L_ADDR 0x0D

enum qmc5883l_range_t {
    QMC5883L_RANGE_2G = 0,
    QMC5883L_RANGE_8G = 1
};

enum qmc5883l_odr_t {
    QMC5883L_ODR_10HZ  = 0,
    QMC5883L_ODR_50HZ  = 1,
    QMC5883L_ODR_100HZ = 2,
    QMC5883L_ODR_200HZ = 3
};

enum qmc5883l_osr_t {
    QMC5883L_OSR_512 = 0,
    QMC5883L_OSR_256 = 1,
    QMC5883L_OSR_128 = 2,
    QMC5883L_OSR_64  = 3
};

class QMC5883L {
public:
    QMC5883L() {}

    bool begin(TwoWire* wire = &Wire) {
        (void)wire;
        _initialized = true;
        return true;
    }

    void setRange(qmc5883l_range_t range) { _range = range; }
    qmc5883l_range_t getRange() const { return _range; }

    void setOutputDataRate(qmc5883l_odr_t odr) { _odr = odr; }
    qmc5883l_odr_t getOutputDataRate() const { return _odr; }

    void setOverSampling(qmc5883l_osr_t osr) { _osr = osr; }
    qmc5883l_osr_t getOverSampling() const { return _osr; }

    void setCalibration(int16_t xOff, int16_t yOff, int16_t zOff) {
        _calX = xOff; _calY = yOff; _calZ = zOff;
    }

    bool dataReady() const { return _initialized; }

    int16_t readX() const { return _x + _calX; }
    int16_t readY() const { return _y + _calY; }
    int16_t readZ() const { return _z + _calZ; }

    void read(int16_t* x, int16_t* y, int16_t* z) const {
        if (x) *x = readX();
        if (y) *y = readY();
        if (z) *z = readZ();
    }

    // Heading in degrees (0-360), relative to X/Y plane
    float getHeading() const {
        float heading = atan2((float)readY(), (float)readX()) * 180.0f / M_PI;
        if (heading < 0) heading += 360.0f;
        return heading;
    }

    int16_t readTemperature() const { return _temp; }

    void reset() {
        _x = _y = _z = 0;
        _temp = 25;
        _calX = _calY = _calZ = 0;
    }

    // ── Test helpers ──
    void _setXYZ(int16_t x, int16_t y, int16_t z) { _x = x; _y = y; _z = z; }
    void _setTemperature(int16_t t) { _temp = t; }

private:
    bool _initialized = false;
    qmc5883l_range_t _range = QMC5883L_RANGE_2G;
    qmc5883l_odr_t _odr = QMC5883L_ODR_10HZ;
    qmc5883l_osr_t _osr = QMC5883L_OSR_512;
    int16_t _x = 100, _y = -50, _z = 200;
    int16_t _calX = 0, _calY = 0, _calZ = 0;
    int16_t _temp = 25;
};
