#ifndef DISPLAY_H
#define DISPLAY_H

#include <Adafruit_SH1106.h>
#include <Adafruit_SSD1306.h>
#include "Image_binaries.h"
#include "Stats.h"

class Display {
public:
    explicit Display(Adafruit_GFX* oled, int screen_width, int screen_height, bool isSH1106);
    void display(Stats& stats);
    void init();
    void displayLogo();
private:
    Adafruit_GFX* d;
    int width;
    int height;
    bool rotated;
    bool isSH1106;

    void display();
    void clearDisplay();
    int getPixelIndexForTextFromRow(int row);
    void writeLine(int line, String text);
    void writeLine(int x, int y, String text);
    void writeTextAtEndOfLine(int line, String text);
    void drawImage(Image img, int x, int y);
};

#endif