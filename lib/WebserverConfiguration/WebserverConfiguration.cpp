
#include "WebserverConfiguration.h"

#include <avr/pgmspace.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

#include <ArduinoOTA.h>

#include "BurnLogger.h"
#include <LittleFS.h>


void WebserverConfiguration::connectToWiFi() {

/*
    IPAddress ip(192, 168, 1, 100); // 192.168.13.37
    IPAddress gateway(192, 168, 1, 1);
    IPAddress dns(8, 8, 8, 8);
    if (!WiFi.config(ip, dns, gateway)) {
        //addLog("Static IP Configuration Failed");
    }
        */

    WiFi.begin(ssid, password);
    //if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        //logManager.addLog(F("Connection Failed!"));    
    //}
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
    server.on("/api/logs", [this]() { handleLogs(); });
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
    char json[300];
    snprintf(json, sizeof(json),
            "{\"exhaustTemperature\":%d,"
            "\"waterTemperature\":%d,"
            "\"burnTime\":%d,"
            "\"lowerExhaustLimit\":%d,"
            "\"upperExhaustLimit\":%d,"
            "\"primaryAirDamper\":%d,"
            "\"freeHeap\":%d,"
            "\"heapFragmentation\":%d,"
            "\"resetReason\":\"%s\","
            "\"heating\":\"%s\"}",
            stats.exhaustTemperature, stats.waterTemperature, stats.burnTimeMinutes,
            stats.lowerExhaustLimit, stats.upperExhaustLimit, stats.primaryAirDamperPosition,
            ESP.getFreeHeap(), ESP.getHeapFragmentation(), ESP.getResetReason().c_str(), stats.heating? "true" : "false");

  server.send(200, "application/json", json);
}

void WebserverConfiguration::handleLogs() {
    const auto& entries = BurnLogger::getEntries();
    if (entries.size() == 0) {
        server.send(200, "application/json", "[]");
        return;
    }
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "application/json", "");

    bool first = true;
    char buf[256];
    size_t len = 0;
    for(const auto& e : entries)
    {
        yield();
        len += snprintf(buf + len, sizeof(buf) - len,
            "%c{\"correction\":%.2f,\"exhaustTemperature\":%d,\"burnTimeMinutes\":%d}",
            first ? '[' : ',', e.correction, e.exhaustTemperature, e.burnTimeMinutes);
        first = false;
        if (len > sizeof(buf) * 0.8) {
            server.sendContent(buf);
            len = 0;
        }
    }
    if(len > 0) 
    {
        server.sendContent(buf);
    }
    server.sendContent("]");
    server.sendContent("");
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