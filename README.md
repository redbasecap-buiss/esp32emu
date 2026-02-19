<p align="center">
  <h1 align="center">esp32emu</h1>
  <p align="center">
    <strong>Lightweight ESP32/Arduino emulator for host-side testing — real network sockets, no hardware needed.</strong>
  </p>
  <p align="center">
    <a href="https://github.com/redbasecap-buiss/esp32emu/actions/workflows/ci.yml"><img src="https://github.com/redbasecap-buiss/esp32emu/actions/workflows/ci.yml/badge.svg" alt="CI"></a>
    <a href="LICENSE"><img src="https://img.shields.io/badge/License-MIT-blue.svg" alt="License: MIT"></a>
    <a href="#"><img src="https://img.shields.io/badge/C++-17-blue.svg?logo=cplusplus" alt="C++17"></a>
    <a href="#"><img src="https://img.shields.io/badge/Platform-macOS%20%7C%20Linux-lightgrey.svg" alt="Platform"></a>
    <a href="#-supported-boards"><img src="https://img.shields.io/badge/Boards-13-green.svg" alt="Boards"></a>
    <a href="#-examples"><img src="https://img.shields.io/badge/Examples-70-orange.svg" alt="Examples"></a>
  </p>
</p>

---

Run your ESP32 and Arduino sketches on macOS or Linux. The HTTP server listens on localhost with real POSIX sockets — test with `curl`, your browser, or integration tests. No QEMU, no Wokwi, no hardware.

```
┌──────────────────────────────────────────────────────────────┐
│  $ ./esp32emu run examples/blink/blink.ino                   │
│                                                              │
│  ╔══════════════════════════════════╗                         │
│  ║         ESP32 DevKit            ║                         │
│  ║         ESP32 @ 240MHz          ║                         │
│  ╚══════════════════════════════════╝                         │
│                                                              │
│  💡 LED (pin 2): ON  ████                                    │
│  [INFO] Blink cycle 1                                        │
│  💡 LED (pin 2): OFF ░░░░                                    │
└──────────────────────────────────────────────────────────────┘
```

## ✨ Features

- 🔌 **Real TCP/HTTP sockets** — WebServer, WiFiClient, HTTPClient use actual POSIX sockets
- 🎯 **GPIO, I2C, SPI, EEPROM** — Full emulation with state tracking
- 📡 **WiFi simulation** — STA/AP mode, connection lifecycle, scan results
- 🖥️ **Rich Terminal UI** — Colored output, pin visualization, ASCII board diagrams
- 🎛️ **Servo, LCD, NeoPixel** — Visual mock output in terminal
- 🏗️ **Multi-board support** — ESP32, ESP32-S3, ESP32-C3, ESP32-S2, ESP32-C6, ESP32-C2, ESP32-H2, Arduino Uno, Mega, Nano, RP2040 Pico, Teensy 4.0, STM32 Blue Pill
- ⚡ **FreeRTOS tasks** — xTaskCreate, mutexes, semaphores backed by std::thread
- 💾 **SD card & filesystem** — SD, SPIFFS, LittleFS backed by host filesystem
- 😴 **Deep sleep mock** — Test power management without exiting
- 📡 **ESP-NOW** — Peer-to-peer communication mock with test helpers
- 📬 **WiFiUDP** — Real UDP sockets on loopback for NTP, DNS, etc.
- 👆 **Touch pins** — ESP32 capacitive touch with `touchRead()`, interrupt support, test helpers
- 🎵 **I2S audio** — Full I2S driver mock with TX/RX, configurable sample rate, test injection
- 🔧 **Stepper motor** — Position tracking, angle calculation, 2/4-wire support
- 📡 **IRremote** — Infrared send/receive with NEC, Sony, Samsung, RC5 protocols
- 🌡️ **BMP280 sensor** — Temperature, pressure, altitude with barometric formula
- 🌡️ **DHT sensor** — DHT11/DHT22 temperature, humidity, heat index
- 🖥️ **SSD1306 OLED** — 128x64/32 display with pixel buffer, text, shapes
- 🎮 **MPU6050 IMU** — 6-axis accelerometer/gyroscope, pitch/roll, G-force
- 🚗 **TWAI/CAN bus** — ESP32 CAN controller mock with TX/RX, filters, test helpers
- 💡 **FastLED** — WS2812B/APA102 LED strip mock with CRGB, HSV, fill_rainbow
- 📋 **ArduinoJson** — JSON serialize/deserialize with nested objects, arrays, parser
- 📝 **ESP-IDF logging** — `ESP_LOGE`/`W`/`I`/`D`/`V` macros with colored output, level filtering, test hooks
- ⏱️ **ESP-IDF timers** — `esp_timer` high-resolution periodic/one-shot timers, microsecond precision
- 🎛️ **LEDC PWM** — 16-channel PWM controller with frequency/duty tracking, tone/note generation
- 📡 **BLE (Bluetooth Low Energy)** — BLEDevice, BLEServer, BLEService, BLECharacteristic, advertising, notifications, test helpers
- 📻 **RMT peripheral** — Remote Control Transceiver TX/RX, configurable channels, WS2812B-compatible timing
- 📮 **FreeRTOS Queues** — xQueueCreate, Send, Receive, Peek, SendToFront, ISR variants, cross-thread producer/consumer
- 🔌 **ESP-IDF I2C driver** — i2c_param_config, i2c_master_cmd_begin, read/write commands, test slave registration
- ⏲️ **ESP-IDF Timer driver** — Hardware timer groups, counter read, alarm, pause/resume, ISR registration
- 🔄 **PCNT (Pulse Counter)** — Rotary encoder support, configurable count modes, limits, filters, event thresholds
- 🌐 **ESP-IDF HTTP server** — `esp_http_server` mock with URI handlers, query parsing, request/response, test dispatch
- 🌐 **ESP-IDF HTTP client** — `esp_http_client` mock with GET/POST/PUT, headers, event handler, response injection
- ⚙️ **MCPWM driver** — Motor Control PWM with dual operators, frequency/duty control, start/stop, multi-unit support
- 📊 **Sigma-Delta modulation** — 8-channel sigma-delta driver with duty/prescale/GPIO configuration
- 🔀 **FreeRTOS Event Groups** — xEventGroupCreate, SetBits, WaitBits, ClearBits, Sync, cross-thread synchronization
- 📡 **ESP-IDF WiFi driver** — esp_wifi_init/start/stop/connect/scan, STA/AP config, test helpers
- 🌐 **ESP-IDF Network Interface** — esp_netif_init, create_default_wifi_sta/ap, IP info, test helpers
- ⏲️ **FreeRTOS Software Timers** — xTimerCreate, Start, Stop, Reset, ChangePeriod, Delete, one-shot and auto-reload
- 🔔 **FreeRTOS Task Notifications** — xTaskNotify, xTaskNotifyGive, ulTaskNotifyTake, xTaskNotifyWait, lightweight IPC
- 🔧 **ESP-IDF System API** — esp_restart, chip info, heap size, reset reason, random, IDF version
- 📦 **Partition Table** — esp_partition find/read/write/erase with default partition layout
- 🔄 **OTA Updates** — esp_ota begin/write/end, boot partition management, app description
- 🎵 **ESP-IDF I2S Driver** — Legacy I2S API with install/uninstall, TX/RX, pin config, sample rate
- 🧪 **55+ library mocks** — Drop-in replacements for Arduino and ESP-IDF APIs
- 📦 **Zero dependencies** — Only C++ stdlib + POSIX

## 🚀 Quick Start

```bash
git clone https://github.com/redbasecap-buiss/esp32emu.git
cd esp32emu

# Run all tests
make test

# Run a sketch
./esp32emu run examples/blink/blink.ino

# Run with a specific board
./esp32emu run --board uno examples/blink/blink.ino

# Run the web server demo — then open http://127.0.0.1:8080
./esp32emu run examples/webserver_demo.cpp

# Build a sketch to a standalone binary
./esp32emu build examples/blink/blink.ino -o blink
./blink
```

## 🎯 Supported Boards

| Board | Chip | Digital Pins | Analog Pins | RAM | Flash | WiFi | Bluetooth |
|-------|------|:------------:|:-----------:|-----|-------|:----:|:---------:|
| **ESP32** (default) | ESP32 | 40 | 8 | 320 KB | 4 MB | ✅ | ✅ |
| **ESP32-S3** | ESP32-S3 | 48 | 10 | 512 KB | 8 MB | ✅ | ✅ |
| **ESP32-C3** | ESP32-C3 (RISC-V) | 22 | 6 | 400 KB | 4 MB | ✅ | ❌ |
| **ESP32-S2** | ESP32-S2 | 46 | 10 | 320 KB | 4 MB | ✅ | ❌ |
| **Arduino Uno** | ATmega328P | 14 | 6 | 2 KB | 32 KB | ❌ | ❌ |
| **Arduino Mega** | ATmega2560 | 54 | 16 | 8 KB | 256 KB | ❌ | ❌ |
| **Arduino Nano** | ATmega328P | 14 | 8 | 2 KB | 32 KB | ❌ | ❌ |
| **ESP32-C6** | ESP32-C6 | 31 | 7 | 512 KB | 4 MB | ✅ WiFi 6 | ✅ BLE 5 |
| **ESP32-H2** | ESP32-H2 (RISC-V) | 22 | 5 | 320 KB | 4 MB | ❌ | ✅ BLE 5 + Zigbee |
| **RP2040 Pico** | RP2040 | 30 | 4 | 264 KB | 2 MB | ❌ | ❌ |
| **Teensy 4.0** | IMXRT1062 | 40 | 14 | 1 MB | 2 MB | ❌ | ❌ |
| **STM32 Blue Pill** | STM32F103C8 | 37 | 10 | 20 KB | 64 KB | ❌ | ❌ |

> ⚠️ **Arduino boards:** WiFi/Bluetooth calls print a warning and are ignored. Pin limits are enforced per board. `LED_BUILTIN = 13` (vs pin 2 on ESP32).

## 📁 Examples

| Example | Description | Best Board |
|---------|-------------|:----------:|
| [`blink/`](examples/blink/) | Classic LED blink with terminal visualization | Any |
| [`button_led/`](examples/button_led/) | Simulated button input → LED toggle | Uno/Mega |
| [`temperature_logger/`](examples/temperature_logger/) | Fake DHT22 sensor, colored output, CSV logging | Any |
| [`rest_api/`](examples/rest_api/) | CRUD Todo REST API on localhost:8080 | ESP32 |
| [`mqtt_client/`](examples/mqtt_client/) | Simulated MQTT publish/subscribe | ESP32 |
| [`servo_sweep/`](examples/servo_sweep/) | Servo 0–180° sweep with ASCII visualization | Uno/Mega |
| [`lcd_display/`](examples/lcd_display/) | 16×2 LCD text display simulation | Uno/Mega |
| [`i2c_scanner/`](examples/i2c_scanner/) | I2C bus scanner with mock devices | Any |
| [`wifi_scanner/`](examples/wifi_scanner/) | Fake WiFi network scanner with signal bars | ESP32 |
| [`neopixel_rainbow/`](examples/neopixel_rainbow/) | NeoPixel RGB rainbow using ANSI true-color | Any |
| [`ota_update/`](examples/ota_update/) | OTA firmware update simulation with progress bar | ESP32 |
| [`captive_portal/`](examples/captive_portal/) | WiFi config portal with web UI | ESP32 |
| [`freertos_tasks/`](examples/freertos_tasks/) | FreeRTOS multi-task with mutex synchronization | ESP32 |
| [`sd_datalogger/`](examples/sd_datalogger/) | SD card CSV data logging | Any |
| [`ultrasonic_buzzer/`](examples/ultrasonic_buzzer/) | Ultrasonic sensor + buzzer parking sensor | Uno/Mega |
| [`mqtt_sensor/`](examples/mqtt_sensor/) | MQTT temp publisher + LED control subscriber | ESP32 |
| [`bluetooth_echo/`](examples/bluetooth_echo/) | Bluetooth Serial echo server | ESP32 |
| [`udp_ntp/`](examples/udp_ntp/) | UDP NTP client with WiFiUDP | ESP32 |
| [`espnow_sender/`](examples/espnow_sender/) | ESP-NOW peer-to-peer sensor data | ESP32 |
| [`deep_sleep_timer/`](examples/deep_sleep_timer/) | Deep sleep with timer wakeup | ESP32 |
| [`webserver_demo.cpp`](examples/webserver_demo.cpp) | Full WebServer with API endpoints | ESP32 |
| [`mcpwm_motor/`](examples/mcpwm_motor/) | MCPWM DC motor control with speed/direction | ESP32 |
| [`http_client_idf/`](examples/http_client_idf/) | ESP-IDF HTTP client with event handler | ESP32 |
| [`sigmadelta_led/`](examples/sigmadelta_led/) | Sigma-delta LED dimming | ESP32 |

> 📖 See [docs/examples.md](docs/examples.md) for detailed walkthroughs of each example.

## 🖥️ Terminal UI

The emulator renders rich visual output directly in your terminal:

### Board Diagram (shown at startup)
```
    ╔══════════════════════════════════╗
    ║         ARDUINO UNO              ║
    ║         ATmega328P               ║
    ║                                  ║
    ║  D0-D13    [■■■■■■■■■■■■■■]     ║
    ║  A0-A5     [■■■■■■]             ║
    ║  PWR  [●]  USB [═══]            ║
    ╚══════════════════════════════════╝
```

### GPIO Pin States
```
┌─── GPIO States ───┐
│ GPIO  2 [OUT] HIGH ● │
│ GPIO  4 [IN ] LOW  ○ │
│ GPIO 13 [OUT] HIGH ● │
└────────────────────┘
```

### Peripheral Visualizations
```
💡 LED (pin 13): ON  ████          🔄 Servo:  90° [░░░░░░░░░░█░░░░░░░░░░]
💡 LED (pin 13): OFF ░░░░

┌──────────────────┐                ██████████████████████████████████
│ Hello World!     │  ← LCD         ↑ NeoPixel rainbow (true-color)
│ Arduino + LCD    │
└──────────────────┘

[INFO]  Sensor reading OK        ← green
[WARN]  Battery low: 3.2V        ← yellow
[ERROR] Connection timeout       ← red
```

## 📚 Supported Arduino Libraries

| Library | Status | Notes |
|---------|:------:|-------|
| `Arduino.h` | ✅ | Core functions, GPIO, Serial, timing |
| `WiFi.h` | ✅ | STA/AP mode, connection simulation |
| `WebServer.h` | ✅ | Real HTTP server on localhost |
| `HTTPClient.h` | ✅ | Real HTTP client |
| `WiFiClient.h` | ✅ | Real TCP client |
| `WiFiClientSecure.h` | ✅ | TLS mock |
| `Wire.h` | ✅ | I2C with mock device callbacks |
| `SPI.h` | ✅ | SPI mock (echo mode) |
| `EEPROM.h` | ✅ | In-memory EEPROM |
| `Preferences.h` | ✅ | NVS-like key-value storage |
| `ESP.h` | ✅ | Chip info, restart, deep sleep |
| `ESPmDNS.h` | ✅ | mDNS mock |
| `Servo.h` | ✅ | Servo mock with terminal visualization |
| `LiquidCrystal.h` | ✅ | LCD mock with terminal display |
| `SoftwareSerial.h` | ✅ | Software serial mock |
| `Adafruit_NeoPixel.h` | ✅ | NeoPixel with terminal true-color rendering |
| `SD.h` | ✅ | SD card mock using host filesystem |
| `SPIFFS.h` / `LittleFS` | ✅ | Flash filesystem mock |
| `freertos/FreeRTOS.h` | ✅ | Tasks, mutexes, semaphores (std::thread) |
| `PubSubClient.h` | ✅ | MQTT publish/subscribe with local delivery |
| `BluetoothSerial.h` | ✅ | ESP32 BT SPP with ring buffer |
| `ArduinoOTA.h` | ✅ | OTA update lifecycle simulation |
| `WiFiUDP.h` | ✅ | Real UDP sockets on loopback |
| `esp_now.h` | ✅ | ESP-NOW peer-to-peer with test helpers |
| `esp_sleep.h` | ✅ | Deep/light sleep mock (non-exiting) |
| `esp_task_wdt.h` | ✅ | Task watchdog timer mock |
| `Ticker.h` | ✅ | Non-blocking timer callbacks |
| `OneWire.h` | ✅ | OneWire bus mock with device search |
| `DallasTemperature.h` | ✅ | DS18B20 temperature sensor mock |

## 🔧 Use as a Library

```bash
make lib
# Link against build/libesp32emu.a, include from include/
```

```cpp
#include "Arduino.h"

void setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
}
```

## 🏗️ Architecture

```mermaid
graph TB
    subgraph "Your Sketch (.ino / .cpp)"
        S[setup & loop]
    end

    subgraph "Arduino API Layer"
        A[Arduino.h] --> GPIO[GPIO Emulation]
        A --> SER[Serial]
        W[WiFi.h] --> NET[POSIX Sockets]
        WS[WebServer.h] --> NET
        HC[HTTPClient.h] --> NET
        WR[Wire.h] --> I2C[I2C Bus Mock]
        SP[SPI.h] --> SPIM[SPI Mock]
        EE[EEPROM.h] --> MEM[In-Memory Store]
    end

    subgraph "Board Emulation"
        B[Board Config] --> PINS[Pin Validator]
        B --> INFO[Chip Info]
    end

    subgraph "Terminal UI"
        T[Terminal Renderer] --> BOARD[Board Diagram]
        T --> PINV[Pin State Display]
        T --> LED[LED / Servo / LCD]
        T --> COL[Colored Serial Output]
    end

    S --> A
    S --> W
    S --> WS
    S --> WR
    GPIO --> T
    SER --> T
    B --> A
```

> 📖 See [docs/architecture.md](docs/architecture.md) for a deep dive into the internals.

## 🧪 Tests

```bash
make test
```

18 tests cover GPIO, WiFi, WebServer, Wire, SPI, EEPROM, Preferences, Servo, LEDC, String, Board, Time, ESP, SD, SPIFFS, NeoPixel, and tone/pulseIn modules.

## 🤝 Contributing

We welcome contributions! See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

**Quick version:**
1. Fork & clone
2. Create a feature branch
3. Run `make test` — all tests must pass
4. Keep headers Arduino-compatible
5. No external dependencies (C++ stdlib + POSIX only)
6. Open a PR

## ❓ FAQ

<details>
<summary><strong>Can I test real HTTP endpoints?</strong></summary>

Yes! `WebServer` binds to a real port on localhost. Use `curl`, Postman, or your browser to interact with it. `HTTPClient` and `WiFiClient` also make real TCP connections.
</details>

<details>
<summary><strong>Does it work on Windows?</strong></summary>

Not yet — the emulator relies on POSIX sockets and terminal escape codes. WSL2 should work.
</details>

<details>
<summary><strong>How accurate is the emulation?</strong></summary>

The API surface matches the real Arduino/ESP32 SDK. Timing is approximate (no cycle-accurate emulation). The goal is functional testing, not hardware simulation.
</details>

<details>
<summary><strong>Can I add custom I2C devices?</strong></summary>

Yes! Use `Wire.onRequest()` and `Wire.onReceive()` callbacks, or register mock devices programmatically.
</details>

<details>
<summary><strong>Why not use PlatformIO's native mode?</strong></summary>

PlatformIO native mode doesn't provide WiFi, WebServer, or networking. esp32emu gives you real sockets, so your HTTP endpoints actually work on localhost.
</details>

## 📋 Changelog

### v0.5.0
- 💾 **SD card library** — full mock with read/write/mkdir/remove backed by host filesystem
- 📂 **SPIFFS & LittleFS** — flash filesystem mocks
- 🌈 **Adafruit NeoPixel library** — with ColorHSV, brightness, terminal true-color rendering
- 🔊 **tone()/noTone()** — buzzer/speaker simulation with terminal output
- 📏 **pulseIn()** — simulated pulse measurement (e.g., ultrasonic sensors)
- 🔀 **shiftOut()/shiftIn()** — shift register mock
- 🔧 **Bug fix**: webserver_demo missing ESP.h include
- 📁 2 new examples: SD data logger, ultrasonic parking sensor
- 🧪 4 new tests (18 total): SD, SPIFFS, NeoPixel, tone/pulseIn

### v0.2.0
- 🏗️ Arduino Uno & Mega board support with `--board` CLI flag
- 📺 Terminal UI: board ASCII art, pin state visualization, colored serial output
- 🎛️ New library mocks: Servo.h, LiquidCrystal.h, SoftwareSerial.h
- 📁 12 new examples covering diverse use cases
- ⚠️ WiFi warnings for non-WiFi boards
- 📊 Pin validation per board

### v0.1.0
- Initial release: ESP32 emulation with real sockets
- WebServer, WiFi, Wire, SPI, EEPROM, HTTPClient support

## License

MIT — see [LICENSE](LICENSE).
