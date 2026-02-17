# esp32emu

**Lightweight ESP32/Arduino emulator for host-side testing — real network sockets, no hardware needed.**

Run your ESP32 and Arduino sketches on macOS or Linux. The HTTP server listens on localhost with real POSIX sockets — test with `curl`, your browser, or integration tests. No QEMU, no Wokwi, no hardware.

## ✨ Features

- 🔌 Real TCP/HTTP sockets (WebServer, WiFiClient, HTTPClient)
- 🎯 GPIO, I2C (Wire), SPI, EEPROM emulation
- 📡 WiFi mock with connection simulation
- 🖥️ Terminal UI: colored output, pin visualization, ASCII board diagrams
- 🎛️ Servo, LCD, SoftwareSerial mocks
- 🏗️ Multi-board support (ESP32, ESP32-S3, Arduino Uno, Arduino Mega)

## 🎯 Supported Boards

| Board | Chip | Digital Pins | Analog Pins | RAM | Flash | WiFi | Bluetooth |
|-------|------|:------------:|:-----------:|-----|-------|:----:|:---------:|
| **ESP32** (default) | ESP32 | 40 | 8 | 320KB | 4MB | ✅ | ✅ |
| **ESP32-S3** | ESP32-S3 | 48 | 10 | 512KB | 8MB | ✅ | ✅ |
| **Arduino Uno** | ATmega328P | 14 | 6 | 2KB | 32KB | ❌ | ❌ |
| **Arduino Mega** | ATmega2560 | 54 | 16 | 8KB | 256KB | ❌ | ❌ |

## Quick Start

```bash
git clone https://github.com/redbasecap-buiss/esp32emu.git
cd esp32emu

# Run the self-tests
make test

# Run a sketch (default: ESP32)
./esp32emu run examples/blink/blink.ino

# Run with specific board
./esp32emu run --board uno examples/blink/blink.ino
./esp32emu run --board mega examples/servo_sweep/servo_sweep.ino

# Run the webserver demo
./esp32emu run examples/webserver_demo.cpp
# → Open http://127.0.0.1:8080 in your browser

# Build a sketch to a binary
./esp32emu build examples/blink/blink.ino -o blink
./blink
```

## 📁 Examples

| Example | Description | Best Board |
|---------|-------------|:----------:|
| [`blink/`](examples/blink/) | Classic LED blink with terminal visualization | Any |
| [`button_led/`](examples/button_led/) | Simulated button input → LED toggle | Uno/Mega |
| [`temperature_logger/`](examples/temperature_logger/) | Fake DHT22 sensor with colored output + CSV logging | Any |
| [`rest_api/`](examples/rest_api/) | CRUD Todo REST API on localhost:8080 | ESP32 |
| [`mqtt_client/`](examples/mqtt_client/) | Simulated MQTT publish/subscribe | ESP32 |
| [`servo_sweep/`](examples/servo_sweep/) | Servo 0-180° sweep with ASCII visualization | Uno/Mega |
| [`lcd_display/`](examples/lcd_display/) | 16x2 LCD text display simulation | Uno/Mega |
| [`i2c_scanner/`](examples/i2c_scanner/) | I2C bus scanner with mock devices | Any |
| [`wifi_scanner/`](examples/wifi_scanner/) | Fake WiFi network scanner with signal bars | ESP32 |
| [`neopixel_rainbow/`](examples/neopixel_rainbow/) | NeoPixel RGB rainbow using ANSI true-color | Any |
| [`ota_update/`](examples/ota_update/) | OTA firmware update simulation with progress bar | ESP32 |
| [`captive_portal/`](examples/captive_portal/) | WiFi config portal with web UI | ESP32 |
| [`webserver_demo.cpp`](examples/webserver_demo.cpp) | Full WebServer with API endpoints | ESP32 |

## 🖥️ Terminal Output

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

### LED Visualization
```
💡 LED (pin 13): ON  ████
💡 LED (pin 13): OFF ░░░░
```

### Servo Position
```
🔄 Servo:  90° [░░░░░░░░░░░░░░░░░░░░█░░░░░░░░░░░░░░░░░░░]
```

### LCD Display
```
┌──────────────────┐
│ Hello World!     │
│ Arduino + LCD    │
└──────────────────┘
```

### Serial Monitor (colored)
```
[INFO]  Sensor reading OK        ← green
[WARN]  Battery low: 3.2V        ← yellow  
[ERROR] Connection timeout       ← red
```

### NeoPixel Rainbow (ANSI true-color)
```
  ██████████████████████████████████  ← actual colors in terminal
```

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
| `ESP.h` | ✅ | Chip info, restart, deep sleep |
| `ESPmDNS.h` | ✅ | mDNS mock |
| `Servo.h` | ✅ | Servo mock with terminal visualization |
| `LiquidCrystal.h` | ✅ | LCD mock with terminal display |
| `SoftwareSerial.h` | ✅ | Software serial mock |

## ⚠️ Arduino Board Limitations

When using `--board uno` or `--board mega`:
- **No WiFi/Bluetooth** — WiFi calls print a warning and are ignored
- **Pin limits enforced** — using pins beyond board capacity triggers warnings
- **LED_BUILTIN = 13** (vs pin 2 on ESP32)

## 🧪 Tests

```bash
make test
```

## 📋 Changelog

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

MIT
