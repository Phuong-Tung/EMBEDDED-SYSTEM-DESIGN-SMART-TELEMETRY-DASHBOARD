#pragma once
#include <EEPROM.h>

// Địa chỉ lưu MIN/MAX/DriveAlert trong EEPROM
#define EEPROM_ADDR_MIN        0   // 4 bytes
#define EEPROM_ADDR_MAX        4   // 4 bytes
#define EEPROM_ADDR_DRIVEALERT 8   // 1 byte

inline void saveMinMaxToEEPROM(int minVal, int maxVal, bool driveAlert) {
    EEPROM.put(EEPROM_ADDR_MIN, minVal);
    EEPROM.put(EEPROM_ADDR_MAX, maxVal);

    uint8_t d = driveAlert ? 1 : 0;
    EEPROM.put(EEPROM_ADDR_DRIVEALERT, d);

    #if defined(ESP8266) || defined(ESP32)
    EEPROM.commit();
    #endif
}

inline void loadMinMaxFromEEPROM(int &minVal, int &maxVal, bool &driveAlert) {
    EEPROM.get(EEPROM_ADDR_MIN, minVal);
    EEPROM.get(EEPROM_ADDR_MAX, maxVal);

    uint8_t d;
    EEPROM.get(EEPROM_ADDR_DRIVEALERT, d);
    if (d == 1) {
        driveAlert = true;
    } else if (d == 0) {
        driveAlert = false;
    } else {
        driveAlert = false;  // mặc định OFF nếu EEPROM chưa ghi
    }

    if (minVal < -100 || minVal > 100) minVal = 20;
    if (maxVal < -100 || maxVal > 100) maxVal = 40;
}
