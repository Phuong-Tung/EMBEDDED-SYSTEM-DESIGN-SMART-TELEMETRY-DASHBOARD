#include "DualButtonProgress.h"
#include "DisplayManager.h"
#include "MinMaxEEPROM.h"
#include <math.h>

DualButtonProgress::DualButtonProgress(uint8_t btn1, uint8_t btn2, uint8_t btn3, DisplayManager* displayMgr)
  : _btn1(btn1), _btn2(btn2), _btn3(btn3), _displayMgr(displayMgr),
    _holdTime(1000), _startTime(0), _isHolding(false), _completed(false), _lastPercent(-1),
    _callback(nullptr),
    _settingField(0), _inConfirm(false), _confirmChoice(0),
    _lastOKState(false), _lastDownState(false), _lastUpState(false),
    _okPressCount(0), _okLastPressTime(0),
    _okPressStart(0), _okHeld(false), _ignoreNextOK(false),
    _lastPressTime(0) {}

void DualButtonProgress::begin() {
  pinMode(_btn1, INPUT_PULLUP);
  pinMode(_btn2, INPUT_PULLUP);
  pinMode(_btn3, INPUT_PULLUP);
}

void DualButtonProgress::setHoldTime(uint16_t ms) {
  _holdTime = ms;
}

void DualButtonProgress::onComplete(void (*callback)()) {
  _callback = callback;
}

void DualButtonProgress::update() {
  int screen = _displayMgr->getScreenIndex();
  if (screen == SCREEN_SETTINGS) {
    handleSettings();
    return;
  }

  bool b1 = (digitalRead(_btn1) == HIGH);  // OK
  bool b2 = (digitalRead(_btn2) == HIGH);
  bool b3 = (digitalRead(_btn3) == HIGH);

  // -------------------------
  // Shortcut: giữ OK 5s để vào Settings
  // -------------------------
  if (b1 && !b2 && !b3) {
    if (!_okHeld) {
      if (_okPressStart == 0) _okPressStart = millis();
      if (millis() - _okPressStart >= 5000) {
        _okHeld = true;
        _okPressStart = 0;

        // vào màn Settings
        _displayMgr->setScreen(SCREEN_SETTINGS);
        _displayMgr->displaySettings(_displayMgr->tempMin, _displayMgr->tempMax, 0);

        // không để nhấn OK đang giữ bị tính như 1 lần nhấn
        _ignoreNextOK = true;
        _okPressCount = 0;
        _okLastPressTime = 0;

        // Đánh dấu last state là pressed để tránh rising-edge ngay lập tức
        _lastOKState = true;
        return;
      }
    }
  } else {
    _okPressStart = 0;
    _okHeld = false;
  }

  // -------------------------
  // Giữ đồng thời 2 nút để vẽ progress (như trước)
  // -------------------------
  if ((b1 && b2) || (b1 && b3) || (b2 && b3)) {
    if (!_isHolding) {
      _isHolding = true;
      _completed = false;
      _startTime = millis();
      _lastPercent = -1;
    }

    unsigned long elapsed = millis() - _startTime;
    int percent = min(100, (int)((elapsed * 100) / _holdTime));

    if (percent != _lastPercent) {
      drawProgress(percent);
      _lastPercent = percent;
    }

    if (percent >= 100 && !_completed) {
      _completed = true;
      if (_callback) _callback();
      _displayMgr->redrawProgressArea(160, 120, 34);
      _isHolding = false;
    }
  } else {
    if (_isHolding) {
      _displayMgr->redrawProgressArea(160, 120, 34);
    }
    _isHolding = false;
    _completed = false;
    _lastPercent = -1;
  }
}

// =========================
// handleSettings: logic đầy đủ
// =========================
void DualButtonProgress::handleSettings() {
    bool b1 = (digitalRead(_btn1) == HIGH); // OK
    bool b2 = (digitalRead(_btn2) == HIGH); // DOWN
    bool b3 = (digitalRead(_btn3) == HIGH); // UP
    unsigned long now = millis();

    static unsigned long okHoldStart = 0;

    // ---------- Nếu đang ở hộp thoại xác nhận ----------
    if (_inConfirm) {
        if (b2 && !_lastDownState) { // DOWN → chọn NO
            _confirmChoice = 1;
            _displayMgr->displayConfirmSaveExit(_confirmChoice);
        }
        if (b3 && !_lastUpState) { // UP → chọn YES
            _confirmChoice = 0;
            _displayMgr->displayConfirmSaveExit(_confirmChoice);
        }
        if (b1 && !_lastOKState && !_ignoreNextOK) { // OK → chọn
            if (_confirmChoice == 0) {
                // YES → lưu và thoát
                saveMinMaxToEEPROM(_displayMgr->tempMin,
                                   _displayMgr->tempMax,
                                   _displayMgr->driveAlertEnabled);
                _inConfirm = false;
                _displayMgr->setScreen(SCREEN_INFO_1);
            } else {
                // NO → quay lại Settings
                _inConfirm = false;
                _displayMgr->setScreen(SCREEN_SETTINGS);
                _displayMgr->displaySettings(_displayMgr->tempMin,
                                             _displayMgr->tempMax,
                                             _settingField);
            }
        }
        if (_ignoreNextOK && !b1) _ignoreNextOK = false;

        _lastOKState   = b1;
        _lastDownState = b2;
        _lastUpState   = b3;
        return;
    }

    // ---------- Nếu đang chỉnh Min/Max/Drive Alert ----------
    if (b2 && !_lastDownState) { // DOWN
        if (_settingField == 0) {
            _displayMgr->tempMin = min(_displayMgr->tempMin + 1,
                                       _displayMgr->tempMax - 1);
        } else if (_settingField == 1) {
            _displayMgr->tempMax = min(_displayMgr->tempMax + 1, 100);
        } else if (_settingField == 2) {
            _displayMgr->driveAlertEnabled = !_displayMgr->driveAlertEnabled;
            //_displayMgr->resetDriveAlertText(); // ép redraw
        }
        _displayMgr->displaySettings(_displayMgr->tempMin,
                                     _displayMgr->tempMax,
                                     _settingField);
    }

    if (b3 && !_lastUpState) { // UP
        if (_settingField == 0) {
            _displayMgr->tempMin = max(-100, _displayMgr->tempMin - 1);
        } else if (_settingField == 1) {
            _displayMgr->tempMax = max(_displayMgr->tempMax - 1,
                                       _displayMgr->tempMin + 1);
        } else if (_settingField == 2) {
            _displayMgr->driveAlertEnabled = !_displayMgr->driveAlertEnabled;
            //_displayMgr->resetDriveAlertText(); // ép redraw
        }
        _displayMgr->displaySettings(_displayMgr->tempMin,
                                     _displayMgr->tempMax,
                                     _settingField);
    }

    // OK: giữ 3s để Save & Exit, nhấn ngắn để chuyển field
    if (b1) {
        if (!_lastOKState) {
            okHoldStart = now; // bắt đầu giữ
        } else if (now - okHoldStart >= 3000 && !_inConfirm) {
            // Giữ OK ≥ 3s → vào hộp thoại Save & Exit
            _inConfirm = true;
            _confirmChoice = 0;
            _displayMgr->displayConfirmSaveExit(_confirmChoice);
            okHoldStart = 0;
        }
    } else {
        if (_lastOKState && !_inConfirm && okHoldStart > 0 &&
            (now - okHoldStart < 3000)) {
            // Nhấn OK ngắn → chuyển field
            _settingField = (_settingField + 1) % 3; // Min → Max → Drive Alert
            _displayMgr->displaySettings(_displayMgr->tempMin,
                                         _displayMgr->tempMax,
                                         _settingField);
        }
        okHoldStart = 0;
    }

    if (_ignoreNextOK && !b1) _ignoreNextOK = false;

    _lastOKState   = b1;
    _lastDownState = b2;
    _lastUpState   = b3;
}



void DualButtonProgress::drawProgress(int percent) {
  Adafruit_ILI9341* _tft = _displayMgr->getTFT();

  int cx = 160, cy = 120, r_outer = 34, r_inner = 30;

  _displayMgr->redrawProgressArea(cx, cy, r_outer);

  float angle_end = (float)percent * 3.6;

  _tft->drawCircle(cx, cy, r_inner, ILI9341_DARKGREY);

  for (int i = 0; i < angle_end; i += 2) {
    float rad = (float)i * 0.0174533;
    int x1 = cx + cos(rad) * r_inner;
    int y1 = cy + sin(rad) * r_inner;
    int x2 = cx + cos(rad) * (r_inner + 4);
    int y2 = cy + sin(rad) * (r_inner + 4);
    _tft->drawLine(x1, y1, x2, y2, ILI9341_GREEN);
  }
}
