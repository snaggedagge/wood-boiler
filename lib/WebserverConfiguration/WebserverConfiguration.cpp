
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

void WebserverConfiguration::handleRoot() {
    Stats& stats = BurnLogger::getStats();

    String path = server.uri();
    if (path.startsWith("/test/")) {
        int number = path.substring(6).toInt();
        stats.primaryAirDamperPosition += number;
    }

  File f = LittleFS.open("/index.html", "r");
  server.streamFile(f, "text/html");
  f.close();
  
  // TODO: Cache page when done tweaking
  // // server.serveStatic("/", LittleFS, "/index.html", "max-age=86400");

  /*
    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/style.css", "text/css");
  });
  */
}

void WebserverConfiguration::init() {
    WiFi.mode(WIFI_STA);
    connectToWiFi();
    setupOtp();
    server.on("/", [this]() { handleRoot(); }); 
    server.onNotFound([this]() { handleRoot(); });

    server.on("/api/stats", [this]() { handleStats(); });
    server.on("/api/logs", [this]() { handleLogs(); });

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
            "\"resetReason\":\"%s\"}",
            stats.exhaustTemperature, stats.waterTemperature, stats.burnTimeMinutes,
            stats.lowerExhaustLimit, stats.upperExhaustLimit, stats.primaryAirDamperPosition, ESP.getFreeHeap(), ESP.getHeapFragmentation(), ESP.getResetReason().c_str());

  server.send(200, "application/json", json);
}

void WebserverConfiguration::handleLogs() {
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "application/json", "");
    bool first = true;
    char buf[256];
    size_t len = 0;
    const auto& entries = BurnLogger::getEntries();
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