# 🛰️ NASA Journey Monitoring System
### *Hệ thống giám sát hành trình thông minh (ESP32 - ILI9341)*

---

## 📝 Giới thiệu
Hệ thống giám sát hành trình tích hợp hiển thị dữ liệu thời gian thực, quản lý ngưỡng nhiệt độ và cảnh báo lái xe quá giờ bằng giọng nói. Dự án sử dụng màn hình **ILI9341** và âm thanh **I2S**.

---

## ✨ Tính năng chính
* **🖥️ Hiển thị đa nhiệm:** 2 màn hình thông tin chính (Layout linh hoạt) và 1 màn hình cài đặt.
* **📊 Giám sát thời gian thực:** Tốc độ, nhiệt độ, thời gian lái xe liên tục (4h) và tổng thời gian trong ngày (10h).
* **⚙️ Hệ thống cài đặt:** Chỉnh ngưỡng Min/Max nhiệt độ và bật/tắt cảnh báo lái xe ngay trên thiết bị.
* **💾 Lưu trữ EEPROM:** Tự động ghi nhớ các thông số cài đặt ngay cả khi mất điện.
* **🗣️ Cảnh báo giọng nói:** Phát file âm thanh qua giao tiếp I2S khi vi phạm thời gian lái xe.
* **⭕ Progress UI:** Hiệu ứng vẽ vòng tròn tiến trình khi nhấn giữ tổ hợp phím để chuyển chức năng.

---

## 🛠️ Thành phần phần cứng

### 1. Linh kiện chính
* **Vi điều khiển:** ESP32 (hoặc tương đương).
* **Màn hình:** TFT LCD ILI9341 (320x240).
* **Âm thanh:** DAC I2S (MAX98357A hoặc tương tự).
* **Nút nhấn:** 3 nút (OK, UP, DOWN).

### 2. Sơ đồ chân (Pinout)
| Linh kiện | Chân ESP32 | Ghi chú |
| :--- | :--- | :--- |
| **TFT CLK** | SCK | SPI Clock |
| **TFT MOSI** | MOSI | SPI Data |
| **TFT CS/DC/RST** | Tùy chọn | Khai báo trong `DisplayManager.h` |
| **Button OK** | GPIO 8 | Xác nhận / Menu |
| **Button DOWN** | GPIO 3 | Giảm / Xuống |
| **Button UP** | GPIO 5 | Tăng / Lên |
| **I2S BCLK** | GPIO 0 | Bit Clock |
| **I2S LRC** | GPIO 1 | Word Select |
| **I2S DIN** | GPIO 2 | Data In |

---

## 📦 Thư viện yêu cầu
* `Adafruit_GFX` & `Adafruit_ILI9341`: Xử lý đồ họa.
* `ESP8266Audio`: Phát file WAV/MP3 từ bộ nhớ Flash.
* `EEPROM`: Quản lý bộ nhớ (có sẵn trong core ESP32).

---

## 🕹️ Hướng dẫn sử dụng

### 1. Thao tác nút nhấn
* **Nhấn giữ đồng thời 2 nút:** Hiển thị vòng tròn tiến trình để chuyển màn hình.
* **Giữ nút OK (5 giây):** Truy cập nhanh vào màn hình **Settings**.
* **Trong màn hình Settings:**
    * *Nhấn OK ngắn:* Chuyển mục (Min Temp -> Max Temp -> Drive Alert).
    * *UP/DOWN:* Tăng/Giảm giá trị hoặc ON/OFF cảnh báo.
    * *Giữ OK (3 giây):* Hiện hộp thoại **Save & Exit**.

### 2. Giao thức dữ liệu (Packet Format)
Hệ thống nhận dữ liệu qua **Serial (UART2)**:
`!NASA,ID,Timestamp,Speed,Temp,DriverName,LicenseID,ContDrive,DailyDrive,EngineState,Checksum`

*Ví dụ:* `!NASA,1,2025-07-27 21:00:00,60,25.5,NGUYEN VAN A,123456,120,300,0,123`

---

## 📂 Cấu trúc thư mục
* `main.ino`: Khởi tạo hệ thống và vòng lặp chính.
* `DisplayManager.cpp/h`: Quản lý giao diện và font chữ.
* `DualButtonProgress.cpp/h`: Xử lý logic nút nhấn và UI tiến trình.
* `PacketParser.cpp/h`: Tách chuỗi dữ liệu Serial.
* `MinMaxEEPROM.h`: Đọc/ghi cài đặt vào Flash.
* `voice.cpp/h`: Điều khiển âm thanh I2S.

> **Lưu ý:** Cần đảm bảo các file header âm thanh (`sound4h.h`, v.v.) đã được chuyển đổi sang mảng byte hex trong bộ nhớ `PROGMEM`.
