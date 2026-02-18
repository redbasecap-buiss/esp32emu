# Changelog

All notable changes to esp32emu are documented here.

## [v0.6.0] — 2026-02-18

### Added
- **PubSubClient (MQTT) mock** — Full publish/subscribe emulation with local message delivery, wildcard topic matching, and test helpers (`injectMessage`, `getPublished`)
- **BluetoothSerial mock** — ESP32 Bluetooth SPP emulation with ring buffer, `readString`/`readStringUntil`, TX inspection, and connection simulation
- **ArduinoOTA mock** — OTA update lifecycle simulation with callbacks (`onStart`, `onEnd`, `onProgress`, `onError`) and `simulateOTA()` test helper
- 3 new tests: `test_mqtt`, `test_bluetooth`, `test_ota`
- 2 new examples: `mqtt_sensor` (publish temps + subscribe to LED commands), `bluetooth_echo` (BT Serial echo server)
- This CHANGELOG

### Stats
- **23 library mocks** (was 20)
- **21 tests** (was 18)
- **19 examples** (was 17)

## [v0.5.0] — 2026-02-18

### Added
- SD card library mock (host filesystem backed)
- SPIFFS & LittleFS filesystem mocks
- Adafruit NeoPixel mock with terminal color visualization
- `tone()` / `noTone()` / `pulseIn()` functions
- 4 new tests, 2 new examples

## [v0.4.0] — 2026-02-18

### Added
- ESP32-S2 and Arduino Nano board support
- FreeRTOS task API (`xTaskCreate`, mutexes, semaphores) backed by `std::thread`
- 14 new tests

## [v0.3.0] — 2026-02-18

### Added
- ESP32-C3 board support (RISC-V)
- `Preferences.h` library mock
- LEDC PWM API
- CI with GitHub Actions
- 5 new tests

## [v0.2.0] — 2026-02-17

### Added
- Arduino Uno and Mega board support with terminal visualization
- `--board` CLI flag
- WiFi capability checks for non-WiFi boards
- 12 new examples

## [v0.1.0] — 2026-02-17

### Added
- Initial release
- ESP32 and ESP32-S3 emulation
- Real TCP/HTTP sockets (WebServer, WiFiClient, HTTPClient)
- GPIO, I2C, SPI, EEPROM, Servo, LCD mocks
- Rich terminal UI with pin visualization
