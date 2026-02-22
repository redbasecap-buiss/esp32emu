// RunningMedian — outlier-resistant sensor filtering
#include <Arduino.h>
#include <RunningMedian.h>

RunningMedian distanceMedian(7);  // Window of 7 samples

void setup() {
    Serial.begin(115200);
    Serial.println("Running median distance filter");

    // Simulate readings with an outlier spike
    float readings[] = {100.2, 100.5, 99.8, 250.0, 100.1, 100.3, 99.9};

    for (float r : readings) {
        distanceMedian.add(r);
        Serial.printf("Reading: %.1f cm → Median: %.1f cm (avg: %.1f)\n",
                       r, distanceMedian.getMedian(), distanceMedian.getAverage());
    }

    Serial.printf("\nFinal stats: Low=%.1f High=%.1f Median=%.1f\n",
                  distanceMedian.getLowest(),
                  distanceMedian.getHighest(),
                  distanceMedian.getMedian());
}

void loop() {
    delay(1000);
}
