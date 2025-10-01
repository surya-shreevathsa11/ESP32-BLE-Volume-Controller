# ESP32 BLE Rotary Volume Controller

This project implements a dedicated, highly responsive **Bluetooth Low Energy (BLE) Human Interface Device (HID)** using an ESP32 microcontroller.  
Its sole function is to act as a wireless media controller, specifically designed to control system volume via a rotary potentiometer and provide real-time status updates on an OLED screen.

---

## What is HID?

**HID (Human Interface Device)** is a device class specification for computer peripherals.  
By utilizing the **BleKeyboard** library, the ESP32 emulates a standard Bluetooth keyboard/media device to a host system (such as iOS, macOS, or Windows).  

This emulation allows the device to send universal media commands:
- `KEY_MEDIA_VOLUME_UP`
- `KEY_MEDIA_VOLUME_DOWN`

These are immediately recognized by the connected system.

---

## Hardware and Wiring

### Components Used
- ESP32 Development Board  
- 10k Rotary Potentiometer  
- 128x64 SSD1306 OLED Display (I2C)  
- 10µF Electrolytic Capacitor (for power smoothing)  
- Jumper Wires  

### Critical Connections

| Component               | ESP32 Pin / Connection | Note                                      |
|--------------------------|-------------------------|-------------------------------------------|
| Potentiometer Wiper (Pin 2) | GPIO 36 (VP)            | Analog Input for Volume Reading           |
| Potentiometer Pin 1     | GND                     |                                           |
| Potentiometer Pin 3     | VCC (3.3V/5V)           |                                           |
| OLED SDA                | GPIO 17                 | I2C Data Line (for Wire1 instance)        |
| OLED SCL                | GPIO 5                  | I2C Clock Line (for Wire1 instance)       |
| 10µF Capacitor          | Between VCC and GND     | Smooths power supply                      |

---

## Software Setup and Dependencies

### ESP32 Board Manager Version (**CRITICAL**)

This code requires **ESP32 Board Manager v2.0.17**.  
Newer versions may cause compilation errors.

1. Open **File > Preferences** and locate your *Board Manager URLs*.  
2. Open **Tools > Board > Boards Manager...**  
3. Search for `esp32` and select **version 2.0.17**, then click *Install*.  

**Why v2.0.17?**  
It ensures full stability and compatibility with the BleKeyboard library and analog input functions.

---

### Required Libraries

Install the following libraries in Arduino IDE (**Sketch > Include Library > Manage Libraries...**):

- **Adafruit GFX Library**  
- **Adafruit SSD1306**  

⚠️ **ESP32 BLE Keyboard**: This library is not in the standard Arduino Library Manager.  
You must download the ZIP from GitHub and install it manually:  
**Sketch > Include Library > Add .ZIP Library...**

---

## Troubleshooting and Lessons Learned

- **Potentiometer Jitter and Insensitivity**  
  Solved by implementing a **Moving Average Filter (numReadings = 5)** with a **tight deadzone (deadzone = 2)**.  
  This ensures smooth and consistent volume adjustments.

- **Compilation Errors**  
  Resolved by running exclusively on **ESP32 Core v2.0.17**.  

---

## License
This project is open-source and available under the MIT License.  

