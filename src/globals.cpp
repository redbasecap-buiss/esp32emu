// Global instances for Arduino-compatible singletons
#include "Arduino.h"
#include "ESP.h"
#include "WiFi.h"
#include "EEPROM.h"
#include "Wire.h"
#include "SPI.h"
#include "ESPmDNS.h"
#include "Update.h"
#include "ETH.h"

HardwareSerial Serial;
HardwareSerial Serial1;
HardwareSerial Serial2;
EspClass ESP;
WiFiClass WiFi;
EEPROMClass EEPROM;
TwoWire Wire;
SPIClass SPI;
MDNSResponder MDNS;
UpdateClass Update;
ETHClass ETH;

#include "ElegantOTA.h"
ElegantOTAClass ElegantOTA;
