// ESP-IDF pthread configuration — set stack size, priority, core pinning
#include <Arduino.h>
#include <esp_pthread.h>
#include <pthread.h>

void* worker(void* arg) {
    const char* name = static_cast<const char*>(arg);
    Serial.print("Worker thread: ");
    Serial.println(name);
    delay(100);
    return nullptr;
}

void setup() {
    Serial.begin(115200);

    // Configure pthread defaults
    esp_pthread_cfg_t cfg = esp_pthread_get_default_config();
    cfg.stack_size = 8192;
    cfg.prio = 10;
    cfg.pin_to_core = 0;
    cfg.thread_name = "sensor_worker";
    esp_pthread_set_cfg(&cfg);

    Serial.println("pthread configured: 8KB stack, prio 10, core 0");

    // Verify config
    esp_pthread_cfg_t out = {};
    esp_pthread_get_cfg(&out);
    Serial.print("Stack: "); Serial.println(out.stack_size);
    Serial.print("Priority: "); Serial.println(out.prio);
    Serial.print("Core: "); Serial.println(out.pin_to_core);
}

void loop() {
    delay(5000);
}
