#ifndef DISPLAY_H
#define DISPLAY_H

#include <Adafruit_SH1106.h>
#include "Image_binaries.h"

class Display {
public:
    explicit Display(Adafruit_SH1106* oled, int screen_width, int screen_height);
    void display(int exhaustTemperature, int waterTemperature, int lowerLimit, int upperLimit, bool heating, int sinceStartedMinutes);
    void init();
    void displayLogo();

private:
    Adafruit_SH1106* d;
    int width;
    int height;
    bool rotated;

    int getPixelIndexForTextFromRow(int row);
    void writeLine(int line, String text);
    void writeLine(int x, int y, String text);
    void writeTextAtEndOfLine(int line, String text);
    void drawImage(Image img, int x, int y);
};

#endif