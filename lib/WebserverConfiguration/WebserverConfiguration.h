#ifndef WEBSERVER_CONFIGURATION_H
#define WEBSERVER_CONFIGURATION_H

#include <ESP8266WebServer.h>

class WebserverConfiguration {
private:
    const char* ssid;
    const char* password;
    ESP8266WebServer server;

    void connectToWiFi();
    void setupOtp();
public:
    WebserverConfiguration(const char* wifi_ssid, const char* wifi_password) : 
    ssid(wifi_ssid), password(wifi_password), server(80) { }

    void handleRoot();
    void init();
    void handleUpdate();
};

#endif