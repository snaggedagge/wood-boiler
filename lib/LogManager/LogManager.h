#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H

#include <Arduino.h>

#define LOG_SIZE 50  // Define the size of the log

class LogManager {
private:
    String logs[LOG_SIZE];
    int head;
    int count;
public:
    LogManager() : head(0), count(0)  { }

    void addLog(const String &log) {
        logs[head] = log;
        head = (head + 1) % LOG_SIZE;
        if (count < LOG_SIZE) {
            count++;
        }
    }

    String getLog() {
        String result = "Logs:\n";
        for (int i = 0; i < count; i++) {
            int index = (head + i - count) % LOG_SIZE; 
            result += "\t" + logs[index];
            result += "\n";
        }
        return result; 
    }
};

#endif