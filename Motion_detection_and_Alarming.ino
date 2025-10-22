// Required Libraries for I2C and the OLED Display (SSD1306)
#include <Wire.h> 
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// --- HARDWARE PIN DEFINITIONS ---
// LEDs (Output)
const int LED_PINS[] = {4, 5, 6, 7};
const int NUM_LEDS = 4;
// Buzzer (Output)
const int BUZZER_PIN = 1;
// PIR Sensor (Input)
const int PIR_PIN = 2; 
// I2C Pins for SSD1306 OLED (Must be defined for ESP32-S3 pin mapping)
const int I2C_SDA_PIN = 8;
const int I2C_SCL_PIN = 9;
// --- DISPLAY SETTINGS ---
#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64   // OLED display height, in pixels
#define OLED_RESET -1      // Reset pin (-1 if sharing Arduino Reset pin)
#define SCREEN_ADDRESS 0x3C // Common I2C address for 128x64 OLED

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// State tracking (Used to update the display only on state change)
bool is_motion_detected = false; 

// --- FUNCTIONS ---

// Function to update the display content
void updateDisplay(const String& message) {
  display.clearDisplay();
  display.setTextSize(1.5);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 20); 

  if (message == "Intruder!!!") {
    display.setTextSize(3); // Larger text for alarm
    display.println("INTRUDER");
    display.setTextSize(2);
    display.setCursor(5, 45);
    display.println("!!!");
  } else {
    display.setTextSize(2);
    display.println("System:");
    display.setCursor(0, 40);
    display.println("CLEAR");
  }
  display.display();
}

// --- ARDUINO SETUP ---
void setup() {
  Serial.begin(115200);
  // Setup Buzzer and LED output pins
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW); 

  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    digitalWrite(LED_PINS[i], LOW); 
  }

  // Setup PIR Sensor input pin
  pinMode(PIR_PIN, INPUT); 

  // --- I2C/OLED INITIALIZATION ---
  
  // Initialize I2C Communication on custom pins (GPIO 8 and 9)
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN); 

  // Initialize OLED Display
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed. Check wiring/address."));
    for (;;); // Loop forever if initialization fails
  }

  // Initial display setup
  updateDisplay("System: Ready");
  Serial.println("System Ready.");
}
void loop() {
  // Read the PIR sensor state
  int pir_value = digitalRead(PIR_PIN);
  if (pir_value == HIGH) {  // Motion Detected
    // Buzzer (ring constantly)
    // Tone is used for a constant sound since simple digitalWrite(HIGH) might be silent on a passive buzzer
    tone(BUZZER_PIN, 1000); 
    // Update Display only when the state changes
    if (!is_motion_detected) {
      is_motion_detected = true;
      updateDisplay("Intruder!!!");
      Serial.println(">>> MOTION DETECTED! ALARM ACTIVE! <<<");
    }
    // LEDs (constantly blinking)
    for (int i = 0; i < NUM_LEDS; i++) {
      digitalWrite(LED_PINS[i], HIGH);
    }
    delay(500);
    for (int i = 0; i < NUM_LEDS; i++) {
      digitalWrite(LED_PINS[i], LOW);
    }

  } else {
    // 2. CLEAR STATE: Turn elements OFF   
    // Buzzer OFF
    noTone(BUZZER_PIN);
    // LEDs OFF
    for (int i = 0; i < NUM_LEDS; i++) {
      digitalWrite(LED_PINS[i], LOW);
    }    
    // Update Display only when the state changes
    if (is_motion_detected) {
      is_motion_detected = false;
      updateDisplay("System: Clear");
      Serial.println("--- System Clear ---");
    }
  } 
  // Add a small delay for stable operation
  delay(500); 
}
