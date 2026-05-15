#ifndef DUAL_BUTTON_PROGRESS_H
#define DUAL_BUTTON_PROGRESS_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "DisplayManager.h"

class DisplayManager;

class DualButtonProgress {
public:
    DualButtonProgress(uint8_t btn1, uint8_t btn2, uint8_t btn3, DisplayManager* displayMgr);
    void begin();
    void setHoldTime(uint16_t ms);
    void onComplete(void (*callback)());
    void update();

private:
    uint8_t _btn1, _btn2, _btn3;
    DisplayManager* _displayMgr;
    uint16_t _holdTime;
    unsigned long _startTime;
    bool _isHolding;
    bool _completed;
    int _lastPercent;
    void (*_callback)();

    // Settings state
    int _settingField = 0; // 0: Min, 1: Max
    bool _inConfirm = false;
    int _confirmChoice = 0; // 0 = YES, 1 = NO

    // Edge & press tracking
    bool _lastOKState = false;
    bool _lastDownState = false;
    bool _lastUpState = false;

    int _okPressCount = 0;
    unsigned long _okLastPressTime = 0;

    // Hold-to-enter-settings
    unsigned long _okPressStart = 0;
    bool _okHeld = false;
    bool _ignoreNextOK = false; // ignore the next rising OK (used when entering settings by hold)

    // generic debounce timestamp
    unsigned long _lastPressTime = 0;

    void drawProgress(int percent);
    void handleSettings();
  
};

#endif
