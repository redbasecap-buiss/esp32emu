// SimpleKalmanFilter — smooth noisy sensor readings
#include <Arduino.h>
#include <SimpleKalmanFilter.h>

// Parameters: measurement uncertainty, estimation uncertainty, process noise
SimpleKalmanFilter tempFilter(2, 2, 0.01);
SimpleKalmanFilter pressFilter(1, 1, 0.01);

float simulateNoisyTemp() {
    return 22.5f + (random(-100, 100) / 100.0f);  // 22.5°C ± 1°C noise
}

void setup() {
    Serial.begin(115200);
    Serial.println("Kalman filter sensor smoothing demo");
    Serial.println("Raw Temperature | Filtered Temperature | Kalman Gain");
}

void loop() {
    float raw = simulateNoisyTemp();
    float filtered = tempFilter.updateEstimate(raw);
    float gain = tempFilter.getKalmanGain();

    Serial.printf("  %.2f°C        |   %.2f°C             |  %.4f\n",
                  raw, filtered, gain);
    delay(500);
}
