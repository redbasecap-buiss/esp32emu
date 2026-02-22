#include "SimpleKalmanFilter.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    SimpleKalmanFilter kf(2, 2, 0.01);

    // Feed some measurements
    float est = kf.updateEstimate(10.0f);
    // First estimate should move toward measurement
    assert(est > 0.0f);

    // Feed consistent values, should converge
    for (int i = 0; i < 50; i++) {
        est = kf.updateEstimate(25.0f);
    }
    assert(fabs(est - 25.0f) < 1.0f);

    // Kalman gain should be between 0 and 1
    float gain = kf.getKalmanGain();
    assert(gain >= 0.0f && gain <= 1.0f);

    // Estimate error should be positive
    assert(kf.getEstimateError() > 0.0f);

    // Setters
    kf.setMeasurementError(1.0f);
    kf.setEstimateError(1.0f);
    kf.setProcessNoise(0.1f);

    // Still works after changing parameters
    est = kf.updateEstimate(25.0f);
    assert(fabs(est - 25.0f) < 2.0f);

    printf("test_simplekalman: all assertions passed\n");
    return 0;
}
