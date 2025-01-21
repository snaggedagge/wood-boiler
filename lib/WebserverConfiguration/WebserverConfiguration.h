#ifndef WEBSERVER_CONFIGURATION_H
#define WEBSERVER_CONFIGURATION_H
#include <avr/pgmspace.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>

#include "LogManager.h"
#include "Stats.h"

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

class WebserverConfiguration {
private:
    const char* ssid;
    const char* password;
    ESP8266WebServer server;
    LogManager& logManager;
    Stats& stats;

    
    void connectToWiFi() {
        logManager.addLog("Connecting to " + (String)ssid);
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
            logManager.addLog(F("Connection Failed!"));    
        }
    }

    void setUpOverTheAirProgramming() {
        ArduinoOTA.setHostname("wood-boiler");
        ArduinoOTA.begin();
    }
public:
    WebserverConfiguration(const char* wifi_ssid, const char* wifi_password, LogManager& lm, Stats& s) : 
    ssid(wifi_ssid), password(wifi_password), server(80), logManager(lm), stats(s) { }

    void handleRoot() {
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
        server.sendContent(logManager.getLog("<p>", "</p>"));
        server.sendContent(htmlPart3);
        server.sendContent("");
    }

    void init() {
        WiFi.mode(WIFI_STA);
        connectToWiFi();
        setUpOverTheAirProgramming();
        if (MDNS.begin("esp8266")) {
            logManager.addLog(F("mDNS responder started"));
        } else {
            logManager.addLog(F("Error setting up MDNS responder!"));
        }
        server.on("/", [this]() { handleRoot(); }); 
        server.begin(); 
    }

    void handleUpdate() {
        ArduinoOTA.handle();
        server.handleClient();
        if (WiFi.status() != WL_CONNECTED) {
            WiFi.disconnect();
            connectToWiFi();
        }
    }

};

#endif