#pragma once

class SimpleKalmanFilter {
public:
    SimpleKalmanFilter(float mea_e, float est_e, float q)
        : _err_measure(mea_e), _err_estimate(est_e), _q(q),
          _current_estimate(0), _last_estimate(0), _kalman_gain(0) {}

    float updateEstimate(float mea) {
        _kalman_gain = _err_estimate / (_err_estimate + _err_measure);
        _current_estimate = _last_estimate + _kalman_gain * (mea - _last_estimate);
        _err_estimate = (1.0f - _kalman_gain) * _err_estimate
                        + fabsf(_last_estimate - _current_estimate) * _q;
        _last_estimate = _current_estimate;
        return _current_estimate;
    }

    void setMeasurementError(float mea_e) { _err_measure = mea_e; }
    void setEstimateError(float est_e) { _err_estimate = est_e; }
    void setProcessNoise(float q) { _q = q; }

    float getKalmanGain() const { return _kalman_gain; }
    float getEstimateError() const { return _err_estimate; }

private:
    float _err_measure;
    float _err_estimate;
    float _q;
    float _current_estimate;
    float _last_estimate;
    float _kalman_gain;

    static float fabsf(float x) { return x < 0 ? -x : x; }
};
