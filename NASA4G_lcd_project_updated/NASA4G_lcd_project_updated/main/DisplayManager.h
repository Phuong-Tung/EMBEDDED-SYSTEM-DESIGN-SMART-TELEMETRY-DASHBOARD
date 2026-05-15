#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "PacketParser.h"
#include "logo_data_rgb565.h"
#include "logo.h"
#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeSansBoldOblique9pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>

class DisplayManager {
public:
  void init();
  void display(const ParsedData& data);
  void showLogo();

private:
  Adafruit_ILI9341 tft = Adafruit_ILI9341(7, 1, 0);
  char lastTime[32] = "", lastSpeed[32] = "", lastTemp[32] = "", lastDriver[64] = "";
  char lastLicense[32] = "", lastDrive1[32] = "", lastDrive2[32] = "", lastEngine[16] = "";
  uint16_t swapRedBlue(uint16_t color);
  void drawImageRegionRGB565_FAST(int16_t x, int16_t y, const uint16_t* data, int imgWidth,
                                  int16_t srcX, int16_t srcY, int regionW, int regionH);
  void displayUpdatedTextFontSmart(const char* newText, char* lastText, int x, int y,
                                   const GFXfont* font, uint16_t color,
                                   const uint16_t* bgImage, int bgImageWidth);
};

#endif