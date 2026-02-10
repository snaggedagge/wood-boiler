
#include "WebserverConfiguration.h"

#include <avr/pgmspace.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

#include <ArduinoOTA.h>

#include "BurnLogger.h"

// Use separate PROGMEM parts, sending chunked responses from server which minimizes memory footprint
const char htmlPart1[] PROGMEM = 
    "<!DOCTYPE html>"
    "<html lang=\"en\">"
    "<head>"
    "    <meta charset=\"UTF-8\">"
    "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
    "    <title>Boiler Stats</title>"
    "    <style>"
    "        body { font-family: Arial, sans-serif; margin: 20px; }"
    "        .stats-table { width: 50%; border-collapse: collapse; margin-bottom: 20px; }"
    "        .stats-table th, .stats-table td { border: 1px solid #ddd; padding: 8px; text-align: left; }"
    "        .stats-table th { background-color: #f4f4f4; }"
    "        .log { border: 1px solid #ccc; padding: 10px; background-color: #f9f9f9; }"
    "    </style>"
    "</head>"
    "<body>"
    "    <h1>Boiler Stats</h1>"
    "    <table class=\"stats-table\">"
    "        <tr><th>Property</th><th>Value</th></tr>";

const char htmlPart2[] PROGMEM = 
    "        <tr><td>Burn Time</td><td>%s</td></tr>"
    "        <tr><td>Exhaust Temperature</td><td>%d</td></tr>"
    "        <tr><td>Water Temperature</td><td>%d</td></tr>"
    "        <tr><td>Lower Exhaust Limit</td><td>%d</td></tr>"
    "        <tr><td>Upper Exhaust Limit</td><td>%d</td></tr>"
    "        <tr><td>Primary Air Damper</td><td>%d</td></tr>"
    "        <tr><td>Heating</td><td>%s</td></tr>"
    "    </table>"
    "    <div class=\"log\">"
    "        <h3>Log</h3>";

const char htmlPart3[] PROGMEM = 
    "    </div>"
    "</body>"
    "</html>";


void WebserverConfiguration::connectToWiFi() {
/*
    IPAddress ip(192, 168, 1, 100); // 192.168.13.37
    IPAddress gateway(192, 168, 1, 1);
    IPAddress dns(8, 8, 8, 8);
    if (!WiFi.config(ip, dns, gateway)) {
        addLog("Static IP Configuration Failed");
    }
*/
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        //logManager.addLog(F("Connection Failed!"));    
    }
}

void WebserverConfiguration::setupOtp() { // Over the air programming. Enables flashing over WIFI
    ArduinoOTA.setHostname("wood-boiler");
    ArduinoOTA.begin();
}

void WebserverConfiguration::handleRoot() {
    Stats& stats = BurnLogger::getStats();

    String path = server.uri();
    if (path.startsWith("/test/")) {
        String number = path.substring(6);
        int num = number.toInt();
        stats.primaryAirDamperPosition += num;
    }

    int hours = stats.burnTimeMinutes / 60;
    int minutes = stats.burnTimeMinutes % 60;
    char timeString[32];
    snprintf(timeString, sizeof(timeString), "%d Hours %d Minutes", hours, minutes);

    char dynamicPart[sizeof(htmlPart2)+50];
    snprintf(dynamicPart, sizeof(dynamicPart), htmlPart2,
        timeString, stats.exhaustTemperature, stats.waterTemperature, stats.lowerExhaustLimit, stats.upperExhaustLimit, stats.primaryAirDamperPosition, stats.heating ? "yes" : "no");
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "text/html", "");

    server.sendContent_P(htmlPart1); 
    server.sendContent(dynamicPart);

    char buffer[512];
    size_t len = 0;

    const auto& entries = BurnLogger::getEntries();
    for(const auto& correction : entries)
    {
        len += snprintf(buffer + len, sizeof(buffer) - len, "<p>PID %d (%.2f) Temp %d Time %d</p>", (int) round(correction.correction), correction.correction, 
            correction.exhaustTemperature, correction.burnTimeMinutes);
        if(len > 200)
        {
            server.sendContent(buffer);
            len = 0;
        }
    }
    if(len > 0) 
    {
        server.sendContent(buffer);
    }

    int sinceStartedMinutes = millis() / 1000 / 60;
    server.sendContent("Free memory " + String(ESP.getFreeHeap()) + " " + String(ESP.getHeapFragmentation()) + "%"+ "</br>" 
    + "Minutes since start " + String(sinceStartedMinutes) + "</br>" 
    + "Reset reason: " + ESP.getResetReason());
    server.sendContent_P(htmlPart3); 
    server.sendContent("");
}

void WebserverConfiguration::init() {
    WiFi.mode(WIFI_STA);
    connectToWiFi();
    setupOtp();
    server.on("/", [this]() { handleRoot(); }); 
    server.onNotFound([this]() { handleRoot(); });
    server.begin(); 
}

void WebserverConfiguration::handleUpdate() {
    ArduinoOTA.handle();
    server.handleClient();
    if (WiFi.status() != WL_CONNECTED) {
        //logManager.addLog(F("Wifi not connected, reconnecting.."));    
        WiFi.disconnect();
        connectToWiFi();
    }
}