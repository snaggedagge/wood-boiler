
#include "WebserverConfiguration.h"

#include <avr/pgmspace.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <LittleFS.h>

#include "BurnLogger.h"

void WebserverConfiguration::connectToWiFi() {
    WiFi.begin(ssid, password);
}

void WebserverConfiguration::setupOtp() { // Over the air programming. Enables flashing over WIFI
    ArduinoOTA.setHostname("wood-boiler");
    ArduinoOTA.begin();
}

void WebserverConfiguration::init() {
    WiFi.mode(WIFI_STA);
    connectToWiFi();
    setupOtp();

    server.onNotFound([this]() { handleNotFound(); });
    server.on("/restart", [this]() { ESP.restart(); }); 
    server.on("/api/stats", [this]() { handleStats(); });
    server.serveStatic("/api/info", LittleFS, BurnLogger::getInfoFilename(), "no-cache");
    server.serveStatic("/api/logs", LittleFS, BurnLogger::getLogFilename(), "no-cache");
    server.serveStatic("/", LittleFS, "/index.html", "public, max-age=31536000, immutable");
    server.begin(); 
}

void WebserverConfiguration::handleUpdate() {
    ArduinoOTA.handle();
    server.handleClient();
}

void WebserverConfiguration::reconnectIfDisconnected() {
    if (WiFi.status() != WL_CONNECTED) {
        WiFi.disconnect();
        connectToWiFi();
    }
}

void WebserverConfiguration::handleStats() {
    Stats& stats = BurnLogger::getStats();
    char json[350];
    snprintf(json, sizeof(json),
            "{\"exhaustTemperature\":%d,"
            "\"waterTemperature\":%d,"
            "\"burnTime\":%d,"
            "\"targetExhaustTemperature\":%d,"
            "\"lowerExhaustLimit\":%d,"
            "\"upperExhaustLimit\":%d,"
            "\"primaryAirDamper\":%d,"
            "\"freeHeap\":%d,"
            "\"heapFragmentation\":%d,"
            "\"resetReason\":\"%s\","
            "\"heating\":%s}",
            stats.exhaustTemperature, stats.waterTemperature, stats.burnTimeMinutes, stats.targetExhaustTemperature,
            stats.lowerExhaustLimit, stats.upperExhaustLimit, stats.primaryAirDamperPosition,
            ESP.getFreeHeap(), ESP.getHeapFragmentation(), ESP.getResetReason().c_str(), stats.heating? "true" : "false");
  server.send(200, "application/json", json);
}

void WebserverConfiguration::handleNotFound() {
    String path = server.uri();
    if (path.startsWith("/test/")) {
        int number = path.substring(6).toInt();
        BurnLogger::getStats().primaryAirDamperPosition += number;
    }
    server.sendHeader("Location", "/",true);
    server.send(302, "text/plane","");
}