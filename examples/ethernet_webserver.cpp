// Example: Ethernet web server using ETH.h
#include "Arduino.h"
#include "ETH.h"
#include "WebServer.h"

WebServer server(80);

void handleRoot() {
    String html = "<html><body>";
    html += "<h1>ESP32 Ethernet Web Server</h1>";
    html += "<p>IP: " + ETH.localIP().toString() + "</p>";
    html += "<p>MAC: " + ETH.macAddress() + "</p>";
    html += "<p>Link Speed: " + String(ETH.linkSpeed()) + " Mbps</p>";
    html += "<p>Full Duplex: " + String(ETH.fullDuplex() ? "Yes" : "No") + "</p>";
    html += "</body></html>";
    server.send(200, "text/html", html);
}

void setup() {
    Serial.begin(115200);
    Serial.println("Ethernet Web Server Example");

    ETH.begin(0, -1, 23, 18, ETH_PHY_LAN8720);
    ETH.setHostname("esp32-eth-server");

    if (ETH.linkUp()) {
        Serial.print("Ethernet connected! IP: ");
        Serial.println(ETH.localIP().toString());
    }

    server.on("/", handleRoot);
    server.begin();
    Serial.println("HTTP server started");
}

void loop() {
    server.handleClient();
    delay(2);
}
