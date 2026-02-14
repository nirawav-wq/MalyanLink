# MalyanLink 🚀
**Developed by:** PURPLNOVA  
**Hardware:** ESP8266EX + ILI9488 (Recycled Malyan M300/M320 UI Board)

MalyanLink is a high-performance system monitor interface that revives old 3D printer displays. It features a custom **3-Wire 9-Bit SPI** implementation to overcome the lack of a D/C pin on these specific modules.

## 🏆 The Technical Challenge: 9-Bit SPI
The display modules found in Malyan M200/M300 and Monoprice Select Mini printers do not use standard 4-wire SPI. They lack a **D/C (Data/Command) pin**. Instead, they require a **9th bit** to be sent before every byte to signal whether it is a command or data.

MalyanLink solves this with a custom bit-banging driver that uses direct register access (`GPOS`/`GPOC`) on the ESP8266 for high-speed updates.



## ⚡ Features
- **Custom 9-Bit SPI Driver:** Optimized for ESP8266 register logic.
- **Real-Time Telemetry:** CPU load, RAM usage, Temps, and Time.
- **Hardware Interaction:** Control Windows Volume via physical board buttons.
- **Low Latency:** Lightweight UDP communication over WiFi.

## 🔌 Hardware Pinout
| Signal | ESP8266 GPIO | Malyan PCB Label |
| :--- | :--- | :--- |
| **CS** | 15 | LCD_CS |
| **RST** | 2 | LCD_RST |
| **MOSI** | 13 | LCD_SDI |
| **CLK** | 14 | LCD_SCK |
| **VOL_DN** | 4 | BTN_1 |
| **VOL_UP** | 5 | BTN_2 |

## 🚀 Installation
1. Flash the firmware from `/Firmware` to your ESP8266.
2. Install Python dependencies: `pip install psutil pycaw comtypes`.
3. Run `python MalyanLink_Host.py` on your PC.

## 📜 License
MIT License - Developed by PURPLNOVA
