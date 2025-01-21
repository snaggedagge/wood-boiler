#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H

#include <Arduino.h>

class LogManager {
private:
    static const int MAX_LOGS = 50; 
    String logs[MAX_LOGS];
    int head;
    int count;
public:
    LogManager() : head(0), count(0)  { }

    void addLog(const String &log) {
        logs[head] = log;
        head = (head + 1) % MAX_LOGS;
        if (count < MAX_LOGS) {
            count++;
        }
    }

    String getLog(String prefix, String postfix) {
        String result = "";
        for (int i = 0; i < count; i++) {
            int index = (head - i) % MAX_LOGS; 
            result += prefix + logs[index] + postfix;
        }
        return result; 
    }
};

#endif