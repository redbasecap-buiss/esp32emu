# Changelog

All notable changes to esp32emu are documented here.

## [v1.3.0] — 2026-02-18

### Added
- **DHT sensor library mock** (`DHT.h`) — DHT11/DHT22/AM2301 temperature & humidity sensor with `readTemperature()` (°C/°F), `readHumidity()`, `computeHeatIndex()` (Rothfusz regression), and `setTemperature()`/`setHumidity()` test helpers
- **Adafruit SSD1306 OLED display mock** (`Adafruit_SSD1306.h`) — 128x64/128x32 OLED emulation with `drawPixel`, `drawRect`, `fillCircle`, `drawLine`, `drawBitmap`, text rendering (`print`/`println`), pixel buffer, and test helpers (`getPrintedLines()`, `getPixel()`, `getDisplayCount()`, `getDrawCalls()`)
- **MPU6050 IMU mock** (`MPU6050.h`) — 6-axis accelerometer/gyroscope with `getMotion6()`, individual axis accessors, G-force conversion (±2g), degrees/second conversion (±250°/s), pitch/roll calculation, temperature, and test helpers
- 3 new examples: `dht_monitor` (DHT22 readings), `oled_dashboard` (SSD1306 system dashboard), `mpu6050_tilt` (tilt angle display)

### Stats
- **12 boards**, **42 library mocks** (was 39): +DHT, Adafruit_SSD1306, MPU6050
- **42 tests** (was 39): +test_dht, test_ssd1306, test_mpu6050
- **39 examples** (was 36): +dht_monitor, oled_dashboard, mpu6050_tilt

## [v1.2.0] — 2026-02-18

### Added
- **Stepper motor library mock** (`Stepper.h`) — Full Arduino Stepper library emulation with 2-wire and 4-wire constructors, position tracking, angle calculation, speed setting, and test helpers (`getPosition()`, `getTotalSteps()`, `getAngle()`, `resetPosition()`)
- **IRremote library mock** (`IRremote.h`) — Infrared send/receive emulation with `IRrecv` (decode queue, `injectCode()` test helper) and `IRsend` (NEC, Sony, Samsung, RC5, raw protocols, sent log tracking)
- **Adafruit BMP280 sensor mock** (`Adafruit_BMP280.h`) — BMP280 pressure/temperature sensor with `readTemperature()`, `readPressure()`, `readAltitude()`, barometric formula, and `setTemperature()`/`setPressure()` test helpers
- 3 new examples: `stepper_sweep` (motor back-and-forth), `ir_remote_control` (IR send/receive echo), `bmp280_weather` (weather station readings)

### Fixed
- **SPI.h unused field warnings** — Added getter methods for `SPISettings` private fields, eliminating `-Wunused-private-field` warnings

### Stats
- **12 boards**, **39 library mocks** (was 36): +Stepper, IRremote, Adafruit_BMP280
- **39 tests** (was 36): +test_stepper, test_irremote, test_bmp280
- **36 examples** (was 33): +stepper_sweep, ir_remote_control, bmp280_weather

## [v1.1.0] — 2026-02-18

### Added
- **Touch pin mock** (`esp32emu_touch.h`) — ESP32 capacitive touch support with `touchRead()`, `touchAttachInterrupt()`, `touchDetachInterrupt()`, threshold-based interrupt firing, and test helpers (`setTouchValue()`, `simulateTouch()`, `releaseTouch()`)
- **I2S audio mock** (`I2S.h`) — Full ESP-IDF I2S driver emulation with `i2s_driver_install/uninstall`, `i2s_start/stop`, `i2s_write/read`, `i2s_set_pin`, configurable sample rate/bit depth, and test helpers (`injectRxData()`, `getTxData()`, `getTxSize()`)
- **GPIO interrupt test** (`test_interrupt`) — Comprehensive test for `attachInterrupt`/`detachInterrupt` covering RISING, FALLING, CHANGE modes and edge cases
- **CONTRIBUTING.md** — Contributing guide with mock design guidelines, code style, testing and example conventions
- 2 new examples: `touch_piano` (capacitive touch piano with 7 keys), `i2s_tone` (440Hz sine wave I2S output)

### Stats
- **12 boards**, **36 library mocks** (was 33): +Touch pins, I2S, GPIO interrupts (test)
- **36 tests** (was 33): +test_touch, test_i2s, test_interrupt
- **33 examples** (was 31): +touch_piano, i2s_tone

## [v1.0.0] — 2026-02-18

### 🎉 v1.0 Release!

### Added
- **Ticker library mock** — Non-blocking timer callbacks with `attach()`, `attach_ms()`, `once()`, `once_ms()`, `detach()`, `active()` and real threaded execution
- **OneWire library mock** — Full bus emulation with device search, read/write, CRC8, and `addDevice()`/`injectReadData()` test helpers
- **DallasTemperature library mock** — DS18B20 temperature sensor support with multi-sensor discovery, °C/°F conversion, resolution config, and `setTemperature()`/`setTemperatureByIndex()` test helpers
- **ESP32 Task Watchdog Timer mock** (`esp_task_wdt.h`) — `esp_task_wdt_init/deinit/add/delete/reset` with timeout tracking, feed counting, and `simulateTimeout()` test helper
- 3 new tests: `test_ticker`, `test_onewire`, `test_wdt`
- 3 new examples: `ticker_blink` (non-blocking LED blink), `ds18b20_temp` (temperature sensor reading), `watchdog_demo` (WDT lifecycle)

### Stats
- **12 boards**, **33 library mocks** (was 29): +Ticker, OneWire, DallasTemperature, esp_task_wdt
- **33 tests** (was 30): +test_ticker, test_onewire, test_wdt
- **31 examples** (was 28): +ticker_blink, ds18b20_temp, watchdog_demo

## [v0.9.0] — 2026-02-18

### Added
- **ESP32-H2 board** — RISC-V core at 96MHz, BLE 5.0 + IEEE 802.15.4 (Thread/Zigbee), no WiFi, 22 GPIOs
- **Teensy 4.0 board** — NXP IMXRT1062 ARM Cortex-M7 at 600MHz, 1MB RAM, 40 digital + 14 analog pins
- **STM32 Blue Pill board** — STM32F103C8 ARM Cortex-M3 at 72MHz, 20KB RAM, 37 digital + 10 analog pins
- **ESPAsyncWebServer mock** — Full async web server emulation with route registration, request/response objects, query params, headers, redirects, not-found handler, wildcard URI matching, and `simulateRequest()` test helper
- **mDNS test** — Test coverage for ESPmDNS mock
- **SoftwareSerial test** — Test coverage for SoftwareSerial mock with inject/read verification
- 3 new examples: `async_rest_api` (async JSON API), `teensy_blink` (Teensy 4.0 LED chase), `bluepill_serial` (STM32 serial bridge)
- ASCII art for all 3 new boards
- Updated `--list-boards` to include all 12 boards

### Stats
- **12 boards** (was 9): +ESP32-H2, Teensy 4.0, STM32 Blue Pill
- **29 library mocks** (was 28): +ESPAsyncWebServer
- **30 tests** (was 27): +test_asyncwebserver, test_mdns, test_softserial
- **28 examples** (was 25): +async_rest_api, teensy_blink, bluepill_serial

## [v0.8.0] — 2026-02-18

### Added
- **ESP32-C6 board** — WiFi 6, BLE 5.0, RISC-V core at 160MHz with 31 GPIOs and ASCII art
- **RP2040 Pico board** — Raspberry Pi Pico support, dual-core Cortex-M0+ at 133MHz, GP0-GP29, LED on GP25
- **WiFiServer mock** — Real TCP server on loopback with `begin()`, `available()`, `stop()`. Accepts incoming WiFiClient connections
- **DNSServer mock** — Captive portal DNS redirection with wildcard/specific domain resolution, test helpers (`resolve`, `getProcessCount`)
- **WiFiClient move semantics** — Proper move constructor/assignment, preventing double-close bugs
- **`--list-boards` CLI flag** — Lists all supported boards with specs
- **`--help` CLI flag** — Usage information
- 3 new tests: `test_boards`, `test_dnsserver`, `test_wifiserver`
- 3 new examples: `captive_portal_dns` (DNS captive portal), `tcp_echo_server` (WiFiServer echo), `pico_blink` (RP2040 blink)

### Stats
- **9 boards** (was 7): +ESP32-C6, RP2040 Pico
- **28 library mocks** (was 26)
- **27 tests** (was 24)
- **25 examples** (was 22)

## [v0.7.0] — 2026-02-18

### Added
- **WiFiUDP mock** — Real UDP sockets on loopback for NTP, DNS, and general UDP communication. Full send/receive API with `beginPacket`/`endPacket`/`parsePacket`
- **ESP-NOW mock** — Peer-to-peer communication emulation with peer management, send/receive callbacks, broadcast support, and test helpers (`injectRecv`, `getSentLog`)
- **esp_sleep mock** — Deep sleep and light sleep emulation that records state instead of exiting. Timer wakeup, ext0/ext1 wakeup config, sleep callbacks for testing
- 3 new tests: `test_udp`, `test_espnow`, `test_sleep`
- 3 new examples: `udp_ntp` (NTP client), `espnow_sender` (sensor data P2P), `deep_sleep_timer` (timer wakeup)

### Stats
- **26 library mocks** (was 23)
- **24 tests** (was 21)
- **22 examples** (was 19)

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
