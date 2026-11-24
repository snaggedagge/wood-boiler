#ifndef NOTIFIER_H
#define NOTIFIER_H

#include <Arduino.h>

// Sends push notifications through ntfy (https://ntfy.sh).
// Subscribe to the same topic in the ntfy app on the phone; the Garmin watch
// mirrors the phone notification if the ntfy app is enabled in Garmin Connect.
class Notifier {
public:
    explicit Notifier(const char* topic) : topic(topic) {}

    // Blocking (a second or two). Returns true if ntfy accepted the message.
    // priority: "min", "low", "default", "high", "urgent"
    bool send(const char* title, const String& message, const char* tags = "fire", const char* priority = "default");

private:
    const char* topic;
};

#endif
