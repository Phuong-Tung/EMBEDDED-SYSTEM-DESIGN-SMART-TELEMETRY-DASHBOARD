# 🚀 GPS Telemetry Display & Warning Extension System
### Advanced Embedded Solution for Automotive Safety | ESP32-C3 & FreeRTOS

---

## 📌 Tổng quan dự án (Project Overview)
Hệ thống hiển thị và cảnh báo thông minh được thiết kế như một module mở rộng cho thiết bị giám sát hành trình **NASA-4G**. Sản phẩm tập trung vào việc xử lý dữ liệu viễn thám (telemetry) thời gian thực, cung cấp giao diện trực quan và cảnh báo giọng nói để hỗ trợ tài xế tuân thủ các quy định an toàn giao thông.

## 🛠 Kiến trúc hệ thống & Công nghệ (System Architecture)
Dự án được xây dựng trên nền tảng **ESP32-C3** với tư duy lập trình nhúng hiện đại:
* **OS:** Sử dụng **FreeRTOS** để quản lý đa nhiệm (Multi-tasking), tách biệt luồng xử lý dữ liệu, hiển thị và âm thanh.
* **Communication:** Giao tiếp **RS232 (UART)** với Checksum validation, đảm bảo dữ liệu không bị sai lệch trong môi trường nhiễu công nghiệp.
* **Storage:** Tích hợp **EEPROM** để lưu trữ các tham số cấu hình hệ thống (Min/Max settings).
* **Audio:** Công nghệ **I2S (MAX98357)** phát âm thanh WAV chất lượng cao từ bộ nhớ Flash.

## 🚀 Tính năng nổi bật (Key Features)

### 1. Xử lý dữ liệu thời gian thực (Real-time Parsing)
Hệ thống giải mã các gói tin phức tạp từ GPS Gateway với độ chính xác tuyệt đối:
```cpp
/Trích đoạn logic từ PacketParser.cpp
d.speed = parts[3].toInt();
d.temperature = parts[4].toFloat();
d.continuousDrive = parts[7].toInt();
d.engineOn = parts[9].toInt() == 0;

