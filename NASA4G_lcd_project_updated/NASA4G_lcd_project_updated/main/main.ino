#include <Arduino.h>
#include "PacketParser.h"
#include "DisplayManager.h"

#define RXD2 20
#define TXD2 21
#define BACKLIGHT_PIN 3

HardwareSerial SerialRS232(1);
DisplayManager displayManager;

void setup() {
  Serial.begin(115200);
  SerialRS232.begin(9600, SERIAL_8N1, RXD2, TXD2);

  displayManager.init();
  pinMode(BACKLIGHT_PIN, OUTPUT);
  analogWrite(BACKLIGHT_PIN, 205);
  displayManager.showLogo();
  String fakePacket = "!NASA,1,2025-07-27 21:00:00,145,-36.59,HO VA TEN,GPLX123456,30,140,1,123";
  ParsedData fakeData = parsePacket(fakePacket);
  displayManager.display(fakeData);
}

void loop() {
  if (SerialRS232.available()) {
    String packet = SerialRS232.readStringUntil('\r');
    ParsedData info = parsePacket(packet);
    displayManager.display(info);
  }
  delay(50);
}