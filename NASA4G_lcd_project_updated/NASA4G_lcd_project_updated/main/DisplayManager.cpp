#include "DisplayManager.h"

void DisplayManager::init() {
  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(ILI9341_BLACK);
}

void DisplayManager::showLogo() {
  drawImageRegionRGB565_FAST(0, 0, image_rgb565_logo, 320, 0, 0, 320, 240);
  delay(1000);
  drawImageRegionRGB565_FAST(0, 0, image_rgb565, 320, 0, 0, 320, 240);
  delay(500);
}

uint16_t DisplayManager::swapRedBlue(uint16_t color) {
  uint16_t r = (color & 0xF800) >> 11;
  uint16_t g = (color & 0x07E0);
  uint16_t b = (color & 0x001F);
  return (b << 11) | g | r;
}

void DisplayManager::drawImageRegionRGB565_FAST(int16_t x, int16_t y, const uint16_t* data, int imgWidth,
                                                int16_t srcX, int16_t srcY, int regionW, int regionH) {
  tft.startWrite();
  tft.setAddrWindow(x, y, regionW, regionH);
  for (int16_t j = 0; j < regionH; j++) {
    for (int16_t i = 0; i < regionW; i++) {
      uint16_t color = swapRedBlue(pgm_read_word(data + (srcY + j) * imgWidth + (srcX + i)));
      tft.writeColor(color, 1);
    }
  }
  tft.endWrite();
}

void DisplayManager::displayUpdatedTextFontSmart(const char* newText, char* lastText, int x, int y,
                                                  const GFXfont* font, uint16_t color,
                                                  const uint16_t* bgImage, int bgImageWidth) {
  if (strcmp(newText, lastText) != 0) {
    tft.setFont(font);
    tft.setTextSize(1);

    int16_t x1_old, y1_old;
    uint16_t w_old, h_old;

    tft.getTextBounds(lastText, x, y, &x1_old, &y1_old, &w_old, &h_old);

    // 👉 Thêm padding xoá nền rộng hơn (để xoá sạch dấu cũ)
    const uint8_t padding = 4;
    x1_old = max(0, x1_old - padding);
    y1_old = max(0, y1_old - padding);
    w_old += padding * 2;
    h_old += padding * 2;

    drawImageRegionRGB565_FAST(x1_old, y1_old, bgImage, bgImageWidth, x1_old, y1_old, w_old, h_old);

    tft.setCursor(x, y);
    tft.setTextColor(swapRedBlue(color));
    tft.print(newText);

    strcpy(lastText, newText);
    tft.setFont();  // Reset font
  }
}


void DisplayManager::display(const ParsedData& info) {
  displayUpdatedTextFontSmart(info.timestamp.c_str(), lastTime, 30, 20, &FreeSansBoldOblique9pt7b, ILI9341_BLACK, image_rgb565, 320);
  displayUpdatedTextFontSmart((String(info.speed) + " km/h").c_str(), lastSpeed, 100, 158, &FreeSansBoldOblique9pt7b, ILI9341_GREEN, image_rgb565, 320);
  displayUpdatedTextFontSmart(String(info.temperature).c_str(), lastTemp, 110, 100, &FreeSansBold18pt7b, ILI9341_GREEN, image_rgb565, 320);
  const char* engineText = info.engineOn ? "ON" : "OFF";
uint16_t engineColor = info.engineOn ? ILI9341_GREEN : ILI9341_RED;
displayUpdatedTextFontSmart(engineText, lastEngine, 210, 170, &FreeSansBoldOblique9pt7b, engineColor, image_rgb565, 320);


  char contStr[10];
  snprintf(contStr, sizeof(contStr), "%02d:%02d", info.continuousDrive / 60, info.continuousDrive % 60);
  displayUpdatedTextFontSmart(contStr, lastDrive1,20, 80, &FreeMonoBold9pt7b, ILI9341_GREEN, image_rgb565, 320);

  char dailyStr[10];
  snprintf(dailyStr, sizeof(dailyStr),"%02d:%2d", info.dailyDrive / 60, info.dailyDrive % 60);
  displayUpdatedTextFontSmart(dailyStr, lastDrive2, 20,140, &FreeMonoBold9pt7b, ILI9341_GREEN, image_rgb565, 320);


  displayUpdatedTextFontSmart(info.driverName.c_str(), lastDriver, 60, 195, &FreeSansBoldOblique9pt7b, ILI9341_BLUE, image_rgb565, 320);
  displayUpdatedTextFontSmart(info.licenseID.c_str(), lastLicense, 60, 225, &FreeSansBoldOblique9pt7b, ILI9341_BLUE, image_rgb565, 320);
}