/**
 * Project: MalyanLink Dashboard
 * Developer: PURPLNOVA
 * Hardware: ESP8266EX + ILI9488 (3-Wire 9-Bit SPI)
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

// --- CONFIGURATION ---
const char* ssid = "YOUR_WLAN_NAME";
const char* password = "YOUR_WLAN_PASSWORD";
const char* pc_ip = "192.168.188.XX"; // Your PC IP

// --- PINS & REGISTERS ---
#define TFT_CS    15 
#define TFT_RST    2  
#define TFT_MOSI  13 
#define TFT_CLK   14 
#define BTN_DOWN   4
#define BTN_UP     5

#define CLK_HIGH  GPOS = (1 << TFT_CLK)
#define CLK_LOW   GPOC = (1 << TFT_CLK)
#define MOSI_HIGH GPOS = (1 << TFT_MOSI)
#define MOSI_LOW  GPOC = (1 << TFT_MOSI)
#define CS_HIGH   GPOS = (1 << TFT_CS)
#define CS_LOW    GPOC = (1 << TFT_CS)

WiFiUDP Udp;
char incomingPacket[255];

const uint8_t font57[] PROGMEM = {
  0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x5F,0x00,0x00, 0x00,0x07,0x00,0x07,0x00, 0x14,0x7F,0x14,0x7F,0x14,
  0x24,0x2A,0x7F,0x2A,0x12, 0x23,0x13,0x08,0x64,0x62, 0x36,0x49,0x55,0x22,0x50, 0x00,0x05,0x03,0x00,0x00,
  0x00,0x1C,0x22,0x41,0x00, 0x00,0x41,0x22,0x1C,0x00, 0x14,0x08,0x3E,0x08,0x14, 0x08,0x08,0x3E,0x08,0x08,
  0x00,0x50,0x30,0x00,0x00, 0x08,0x08,0x08,0x08,0x08, 0x00,0x60,0x60,0x00,0x00, 0x20,0x10,0x08,0x04,0x02,
  0x3E,0x51,0x49,0x45,0x3E, 0x00,0x42,0x7F,0x40,0x00, 0x42,0x61,0x51,0x49,0x46, 0x21,0x41,0x45,0x4B,0x31,
  0x18,0x14,0x12,0x7F,0x10, 0x27,0x45,0x45,0x45,0x39, 0x3C,0x4A,0x49,0x49,0x30, 0x01,0x71,0x09,0x05,0x03,
  0x36,0x49,0x49,0x49,0x36, 0x06,0x49,0x49,0x29,0x1E, 0x00,0x36,0x36,0x00,0x00, 0x00,0x56,0x36,0x00,0x00,
  0x08,0x14,0x22,0x41,0x00, 0x14,0x14,0x14,0x14,0x14, 0x00,0x41,0x22,0x14,0x08, 0x02,0x01,0x51,0x09,0x06,
  0x32,0x49,0x79,0x41,0x3E, 0x7E,0x11,0x11,0x11,0x7E, 0x7F,0x49,0x49,0x49,0x36, 0x3E,0x41,0x41,0x41,0x22,
  0x7F,0x41,0x41,0x22,0x1C, 0x7F,0x49,0x49,0x49,0x41, 0x7F,0x09,0x09,0x09,0x01, 0x3E,0x41,0x49,0x49,0x7A,
  0x7F,0x08,0x08,0x08,0x7F, 0x00,0x41,0x7F,0x41,0x00, 0x20,0x40,0x41,0x3F,0x01, 0x7F,0x08,0x14,0x22,0x41,
  0x7F,0x40,0x40,0x40,0x40, 0x7F,0x02,0x0C,0x02,0x7F, 0x7F,0x04,0x08,0x10,0x7F, 0x3E,0x41,0x41,0x41,0x3E,
  0x7F,0x09,0x09,0x09,0x06, 0x3E,0x41,0x51,0x21,0x5E, 0x7F,0x09,0x19,0x29,0x46, 0x46,0x49,0x49,0x49,0x31,
  0x01,0x01,0x7F,0x01,0x01, 0x3F,0x40,0x40,0x40,0x3F, 0x1F,0x20,0x40,0x20,0x1F, 0x3F,0x40,0x38,0x40,0x3F,
  0x63,0x14,0x08,0x14,0x63, 0x07,0x08,0x70,0x08,0x07, 0x61,0x51,0x49,0x45,0x43
};

void send9Bit(uint8_t data, bool isData) {
  CS_LOW;
  if (isData) MOSI_HIGH; else MOSI_LOW;
  CLK_HIGH; CLK_LOW;
  for (uint8_t i = 0; i < 8; i++) {
    if (data & 0x80) MOSI_HIGH; else MOSI_LOW;
    data <<= 1;
    CLK_HIGH; CLK_LOW;
  }
  CS_HIGH;
}

void setWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
  send9Bit(0x2A, false);
  send9Bit(x >> 8, true); send9Bit(x & 0xFF, true);
  send9Bit((x+w-1) >> 8, true); send9Bit((x+w-1) & 0xFF, true);
  send9Bit(0x2B, false);
  send9Bit(y >> 8, true); send9Bit(y & 0xFF, true);
  send9Bit((y+h-1) >> 8, true); send9Bit((y+h-1) & 0xFF, true);
  send9Bit(0x2C, false);
}

void drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b) {
  setWindow(x, y, w, h);
  for (uint32_t i = 0; i < (uint32_t)w * h; i++) {
    send9Bit(r, true); send9Bit(g, true); send9Bit(b, true);
    if (i % 2000 == 0) yield();
  }
}

void drawChar(uint16_t x, uint16_t y, char c, uint8_t size, uint8_t r, uint8_t g, uint8_t b, uint8_t br, uint8_t bg, uint8_t bb) {
  if (c < 32 || c > 90) return;
  uint16_t font_ptr = (c - 32) * 5;
  for (int8_t i = 0; i < 5; i++) {
    uint8_t line = pgm_read_byte(&font57[font_ptr + i]);
    for (int8_t j = 0; j < 8; j++) {
      if (line & 0x1) drawRect(x + i * size, y + j * size, size, size, r, g, b);
      else drawRect(x + i * size, y + j * size, size, size, br, bg, bb);
      line >>= 1;
    }
  }
}

void drawString(uint16_t x, uint16_t y, const char* str, uint8_t size, uint8_t r, uint8_t g, uint8_t b, uint8_t br, uint8_t bg, uint8_t bb) {
  while (*str) {
    drawChar(x, y, *str++, size, r, g, b, br, bg, bb);
    x += 6 * size;
  }
}

void updateBar(uint16_t y, int val, uint8_t r, uint8_t g, uint8_t b) {
  uint16_t startX = 110; uint16_t maxWidth = 340;
  int barWidth = (val * maxWidth) / 100;
  if (barWidth > 0) drawRect(startX, y, barWidth, 25, r, g, b);
  if (barWidth < maxWidth) drawRect(startX + barWidth, y, maxWidth - barWidth, 25, 30, 30, 45);
}

void setup() {
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(TFT_CS, OUTPUT); pinMode(TFT_RST, OUTPUT);
  pinMode(TFT_MOSI, OUTPUT); pinMode(TFT_CLK, OUTPUT);
  digitalWrite(TFT_RST, LOW); delay(100); digitalWrite(TFT_RST, HIGH); delay(100);
  send9Bit(0x01, false); delay(150);
  send9Bit(0x11, false); delay(150);
  send9Bit(0x36, false); send9Bit(0x28, true); 
  send9Bit(0x3A, false); send9Bit(0x66, true);
  send9Bit(0x29, false);

  drawRect(0, 0, 480, 320, 5, 5, 15); 
  drawRect(0, 0, 480, 50, 20, 20, 60);
  drawString(15, 15, "MALYAN MONITOR", 2, 0, 255, 255, 20, 20, 60);
  drawString(20, 80, "CPU:", 2, 255, 255, 255, 5, 5, 15);
  drawString(20, 130, "RAM:", 2, 255, 255, 255, 5, 5, 15);
  drawString(20, 180, "TMP:", 2, 255, 255, 255, 5, 5, 15);
  drawString(20, 230, "VOL:", 2, 255, 255, 255, 5, 5, 15);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { yield(); }
  Udp.begin(4210);
}

void loop() {
  if (digitalRead(BTN_UP) == LOW) {
    Udp.beginPacket(pc_ip, 4211); 
    Udp.write("V_UP");
    Udp.endPacket();
    delay(150);
  }
  if (digitalRead(BTN_DOWN) == LOW) {
    Udp.beginPacket(pc_ip, 4211);
    Udp.write("V_DOWN");
    Udp.endPacket();
    delay(150);
  }

  int packetSize = Udp.parsePacket();
  if (packetSize) {
    int len = Udp.read(incomingPacket, 255);
    if (len > 0) incomingPacket[len] = 0;
    String line = String(incomingPacket);
    int colon = line.indexOf(':');
    if (colon != -1) {
      String cmd = line.substring(0, colon);
      String val = line.substring(colon + 1);
      if (cmd == "CPU")      updateBar(78,  val.toInt(), 255, 80, 80);
      else if (cmd == "RAM") updateBar(128, val.toInt(), 80, 255, 80);
      else if (cmd == "TMP") updateBar(178, val.toInt(), 255, 160, 20);
      else if (cmd == "VOL") updateBar(228, val.toInt(), 80, 80, 255); 
      else if (cmd == "CLK") {
         drawRect(360, 15, 110, 25, 20, 20, 60); 
         drawString(360, 15, val.c_str(), 2, 255, 255, 255, 20, 20, 60);
      }
    }
  }
  yield();
}
