#pragma once
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <vector>

class RunningMedian {
public:
    explicit RunningMedian(uint8_t size = 11) : _maxSize(size) {
        _values.reserve(size);
    }

    void add(float value) {
        if (_values.size() >= _maxSize) {
            _values.erase(_values.begin());
        }
        _values.push_back(value);
    }

    float getMedian() const {
        if (_values.empty()) return 0;
        std::vector<float> sorted = _values;
        std::sort(sorted.begin(), sorted.end());
        size_t n = sorted.size();
        if (n % 2 == 1) return sorted[n / 2];
        return (sorted[n / 2 - 1] + sorted[n / 2]) / 2.0f;
    }

    float getAverage() const {
        if (_values.empty()) return 0;
        float sum = 0;
        for (float v : _values) sum += v;
        return sum / _values.size();
    }

    float getHighest() const {
        if (_values.empty()) return 0;
        return *std::max_element(_values.begin(), _values.end());
    }

    float getLowest() const {
        if (_values.empty()) return 0;
        return *std::min_element(_values.begin(), _values.end());
    }

    size_t getCount() const { return _values.size(); }
    size_t getSize() const { return _maxSize; }
    bool isFull() const { return _values.size() >= _maxSize; }

    void clear() { _values.clear(); }

    float getElement(uint8_t n) const {
        if (n >= _values.size()) return 0;
        return _values[n];
    }

    float getSortedElement(uint8_t n) const {
        if (n >= _values.size()) return 0;
        std::vector<float> sorted = _values;
        std::sort(sorted.begin(), sorted.end());
        return sorted[n];
    }

    float getQuantile(float q) const {
        if (_values.empty()) return 0;
        std::vector<float> sorted = _values;
        std::sort(sorted.begin(), sorted.end());
        size_t idx = static_cast<size_t>(q * (sorted.size() - 1));
        return sorted[idx];
    }

private:
    size_t _maxSize;
    std::vector<float> _values;
};
