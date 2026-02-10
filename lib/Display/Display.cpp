#include "Display.h"
#include "Images.h"

  /*
  The Adafruit SSD1306 OLED library typically uses a default font size of 1, which corresponds to a 5x8 pixel font. This
  means each character is 5 pixels wide and 8 pixels tall. When you use this font size on the display, each character you
  print will take up that amount of space on the screen. Keep in mind that there might be a 1-pixel space between
  characters for readability, so if you're calculating the number of characters that can fit on a line, you might need to
  consider this additional space.
  */

Display::Display(Adafruit_SH1106* oled, int screen_width, int screen_height) {
  d = oled;
  width = screen_width;
  height = screen_height;
  rotated = false;
}

void Display::init() {
  d->begin(SH1106_SWITCHCAPVCC, 0x3C);
  delay(500);
  d->setTextSize(1);
  d->setTextColor(WHITE);
}

void Display::displayLogo() {
  d->clearDisplay();
  drawImage(effecta_logo_large, 0, 0);
  d->display();
}

/*
There are 7 rows (8 really but nice to have some pixels in between)
*/
int Display::getPixelIndexForTextFromRow(int row) {
  return (height / 7) * (row - 1);
}

void Display::writeLine(int line, String text) {
    d->setCursor(0, getPixelIndexForTextFromRow(line));
    d->println(text);
}

void Display::writeLine(int x, int y, String text) {
    d->setCursor(x, y);
    d->println(text);
}

void Display::writeTextAtEndOfLine(int line, String text) {
  d->setCursor(width - (text.length()*6), getPixelIndexForTextFromRow(line));
  d->print(String(text));
}

void Display::display(Stats* stats) {
  d->clearDisplay();
  drawImage(effecta_logo_small, 0, 0);
  if (stats->heating)
  {
    if (rotated)
    {
      drawImage(fan_rotated_small, 60, 0);
    } else {
      drawImage(fan_small, 60, 0);
    }
    rotated = !rotated;
  }

  drawImage(o2_small, 0, 22);
  writeLine(20, 25, String(stats->primaryAirDamperPosition) + " %");

  drawImage(waterdrop_small, 0, 42);
  writeLine(20, 47, String(stats->waterTemperature) + " C");

  drawImage(heating_small, 70, 28);
  writeLine(92, 32, String(stats->exhaustTemperature) + " C");

  writeTextAtEndOfLine(7, String(stats->lowerExhaustLimit) + " - " + String(stats->upperExhaustLimit) + "    ");
/*
  writeLine(4, "O2: " + String(10) + " %");
  writeLine(6, "Temperatur: " + String(temperature) + " C");
  writeLine(7, "Grans: " + String(lowerLimit) + "-" + String(upperLimit)  + " C");
*/
  writeTextAtEndOfLine(1, String(stats->burnTimeMinutes/60) + "H " + String(stats->burnTimeMinutes%60) + "M ");
  d->display();
}

void Display::drawImage(Image img, int x, int y) {
  d->drawBitmap(x, y, img.data, img.width, img.height, WHITE);
}