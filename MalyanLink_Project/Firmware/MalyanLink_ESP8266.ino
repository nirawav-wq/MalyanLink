/**
 * Project: MalyanLink Dashboard
 * Developer: PURPLNOVA
 * Hardware: ESP8266EX + ILI9488 (3-Wire 9-Bit SPI)
 */
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char* ssid = "YOUR_WLAN_NAME";
const char* password = "YOUR_WLAN_PASSWORD";
const char* pc_ip = "192.168.188.XX";

#define TFT_CS 15
#define TFT_RST 2
#define TFT_MOSI 13
#define TFT_CLK 14

void send9Bit(uint8_t data, bool isData) {
  GPOC = (1 << TFT_CS);
  if (isData) GPOS = (1 << TFT_MOSI); else GPOC = (1 << TFT_MOSI);
  GPOS = (1 << TFT_CLK); GPOC = (1 << TFT_CLK);
  for (uint8_t i = 0; i < 8; i++) {
    if (data & 0x80) GPOS = (1 << TFT_MOSI); else GPOC = (1 << TFT_MOSI);
    data <<= 1;
    GPOS = (1 << TFT_CLK); GPOC = (1 << TFT_CLK);
  }
  GPOS = (1 << TFT_CS);
}

void setup() {
  pinMode(TFT_CS, OUTPUT); pinMode(TFT_RST, OUTPUT);
  pinMode(TFT_MOSI, OUTPUT); pinMode(TFT_CLK, OUTPUT);
  digitalWrite(TFT_RST, LOW); delay(100); digitalWrite(TFT_RST, HIGH);
  // Initialization commands...
}

void loop() {
  // Main logic
}
