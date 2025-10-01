#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <BleKeyboard.h>

// =========================================================
// ESP32 Pin Config
// =========================================================
// Potentiometer middle pin (VP - Analog input)
#define POT_PIN 36 

// OLED I2C Pins 
#define OLED_SDA 17
#define OLED_SCL 5
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
// The last parameter -1 indicates no reset pin is used.
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, -1); 

// Bluetooth HID
BleKeyboard bleKeyboard("ESP32_Vol_Controller", "Espressif", 100);

// =========================================================
// Global State Variables
// =========================================================
// Variables for smoothing the potentiometer readings (Moving Average Filter)
const int numReadings = 5; 
int readings[numReadings];
int readIndex = 0;
long total = 0;
int averagePotValue = 0;
int lastAverageValue = -1;
int lastVolumePercent = -1;
// Increased sensitivity (deadzone = 2) for responsive control
const int deadzone = 2; 

// Display Message State
String displayMessage = "Active";
unsigned long displayMessageEndTime = 0;
const unsigned long actionMessageDuration = 1000; // Show action message for 1 second

// =========================================================
// Setup
// =========================================================
void setup() {
  Serial.begin(115200);
  Serial.println("--- ESP32 Volume Controller Initializing ---");

  // --- Initialize OLED ---
  // Use secondary I2C bus for the OLED
  Wire1.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
      Serial.println(F("SSD1306 allocation failed"));
      // Stay in an infinite loop if initialization fails
      for (;;);
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("System Initializing...");
  
  // CRITICAL: Enable word wrapping for the display
  display.setTextWrap(true);

  display.display();
  delay(1000);

  // --- Initialize Bluetooth HID ---
  bleKeyboard.begin();
  
  // Initialize all readings for the smoothing filter
  for (int i = 0; i < numReadings; i++) {
      readings[i] = 0;
  }
}

// =========================================================
// Main Loop
// =========================================================
void loop() {
  
  // ---------------------------------------------------------
  // 1. Handle Potentiometer (Volume Control)
  // ---------------------------------------------------------
  if (bleKeyboard.isConnected()) {
      // --- Analog Averaging Filter (Smoothing) ---
      total = total - readings[readIndex];
      readings[readIndex] = analogRead(POT_PIN);
      total = total + readings[readIndex];
      readIndex = (readIndex + 1) % numReadings;

      // Calculate the average
      averagePotValue = total / numReadings;

      // --- Map the smoothed value to a percentage ---
      // ESP32 analog read is 0 to 4095
      int volumePercent = map(averagePotValue, 0, 4095, 0, 100);

      // --- Send Volume Commands ---
      // Check if the change exceeds the sensitivity threshold (deadzone)
      if (abs(averagePotValue - lastAverageValue) > deadzone) {
          
          if (volumePercent > lastVolumePercent) {
              // Volume Up
              bleKeyboard.write(KEY_MEDIA_VOLUME_UP);
              displayMessage = "Volume Up";
              displayMessageEndTime = millis() + actionMessageDuration;
          } else if (volumePercent < lastVolumePercent) {
              // Volume Down
              bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN);
              displayMessage = "Volume Down";
              displayMessageEndTime = millis() + actionMessageDuration;
          }
          
          lastAverageValue = averagePotValue;
          lastVolumePercent = volumePercent;
      }

      // ---------------------------------------------------------
      // 2. Update Display
      // ---------------------------------------------------------
      
      // Check if the action message timeout has expired
      if (millis() > displayMessageEndTime) {
          // Revert to "Active" status if the knob isn't being moved
          if (abs(averagePotValue - lastAverageValue) <= deadzone) {
              displayMessage = "Active"; 
          }
      }

      display.clearDisplay();
      
      // Status Line (Top, Size 1) - Shows BT Connection Status
      display.setTextSize(1);
      display.setCursor(0, 0);
      display.print("BT Status: ");
      display.println(bleKeyboard.isConnected() ? "Connected" : "Scanning");

      // Main Action Message (Middle, Size 2) - Shows Volume Up/Down or Active/Disconnected
      display.setTextSize(2);
      display.setCursor(0, 15); // Moved down slightly to give space for the title
      display.println(displayMessage);
      
      display.display();
  } else {
      // If not connected, display scanning status
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.println("BT Status: Scanning...");
      display.setCursor(0, 10);
      display.println("Pair with iPhone now.");
      display.display();

      // Reset action message when disconnected
      displayMessage = "BT Disconnected";
  }

  delay(10); // Maintain a fast loop cycle
}
