#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include "LogManager.h"

class Configuration {
private:
    const char* ssid;
    const char* password;
    ESP8266WebServer server;
    LogManager& logManager;
public:
    Configuration(const char* wifi_ssid, const char* wifi_password, LogManager& lm) : ssid(wifi_ssid), password(wifi_password), server(80), logManager(lm) { }

    void handleRoot() {
        server.send(200, "text/plain", logManager.getLog());
    }

    void connectToWiFi() {
        logManager.addLog("Connecting to " + (String)ssid);

        WiFi.mode(WIFI_STA);
/*
        IPAddress ip(192, 168, 1, 100); // 192.168.13.37
        IPAddress gateway(192, 168, 1, 1);
        IPAddress dns(8, 8, 8, 8);
        if (!WiFi.config(ip, dns, gateway)) {
            addLog("Static IP Configuration Failed");
        }
*/
        WiFi.begin(ssid, password);
        if (WiFi.waitForConnectResult() == WL_CONNECTED) {
            delay(2000);        
            if (MDNS.begin("esp8266")) {
                logManager.addLog("mDNS responder started");
            } else {
                logManager.addLog("Error setting up MDNS responder!");
            }
            server.on("/", [this]() { handleRoot(); }); 
            server.begin(); 
        } else {
            logManager.addLog("Connection Failed!");
        }
    }

    void setUpOverTheAirProgramming() {
        ArduinoOTA.setHostname("wood-boiler");
/*
        ArduinoOTA.onStart([]() {
            String type;
            if (ArduinoOTA.getCommand() == U_FLASH)
            type = "sketch";
            else // U_SPIFFS
            type = "filesystem";

            // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
            addLog("Start updating " + type);
        });
        ArduinoOTA.onEnd([]() {
            addLog("\nEnd");
        });
        ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
            addLog("Progress: %u%%\r", (progress / (total / 100)));
        });
        ArduinoOTA.onError([](ota_error_t error) {
            addLog("Error[%u]: ", error);
            if (error == OTA_AUTH_ERROR) addLog("Auth Failed");
            else if (error == OTA_BEGIN_ERROR) addLog("Begin Failed");
            else if (error == OTA_CONNECT_ERROR) addLog("Connect Failed");
            else if (error == OTA_RECEIVE_ERROR) addLog("Receive Failed");
            else if (error == OTA_END_ERROR) addLog("End Failed");
        });
*/
        ArduinoOTA.begin();
    }

    void handleUpdate() {
        ArduinoOTA.handle();
        server.handleClient();
    }

};

#endif