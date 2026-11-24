#include "Notifier.h"

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

bool Notifier::send(const char* title, const String& message, const char* tags, const char* priority) {
    if (WiFi.status() != WL_CONNECTED) {
        return false;
    }

    BearSSL::WiFiClientSecure client;
    client.setInsecure();               // No certificate validation; the topic name is the only "secret"
    client.setBufferSizes(512, 512);    // Keep TLS RAM usage low
    client.setTimeout(5000);

    HTTPClient http;
    http.setTimeout(5000);
    String url = String("https://ntfy.sh/") + topic;
    if (!http.begin(client, url)) {
        return false;
    }
    http.addHeader("Title", title);
    http.addHeader("Tags", tags);
    http.addHeader("Priority", priority);
    int code = http.POST(message);
    http.end();
    return code == 200;
}
