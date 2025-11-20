// ===============================================
// ESP32 PIR Motion Alarm System
// BLYNK INTEGRATION: Final Code
// 
// --- REQUIRED DEFINITIONS MOVED TO THE TOP ---
//
// BLYNK TEMPLATE IDENTIFIERS (Provided by user)
#define BLYNK_TEMPLATE_ID "TMPL3vfqKlKGg"
#define BLYNK_TEMPLATE_NAME "Intrusion Detection System"
// --- END OF REQUIRED DEFINITIONS ---
// ===============================================

// --- BLYNK & WIFI LIBRARIES ---
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// --- BLYNK AUTH AND WIFI CREDENTIALS ---
// BLYNK AUTH TOKEN (Provided by user)
char auth[] = "06CoLebSrHYTvmfYTUtJ2v-F_j5N2fsw";

// !!! REPLACE these with your actual WiFi credentials !!!
char ssid[] = "Wifi_ssid"; 
char pass[] = "Wifi_password";

// --- BLYNK VIRTUAL PIN DEFINITIONS ---
#define V0_ALARM_STATUS 0    // Alarm status (0/1) for LED Widget
#define V1_STATUS_LOG 1      // Text log for alerts (Terminal Widget)

// --- INPUT PINS ---
const int kPirPin = 15; 

// --- OUTPUT PINS ---
const int kBuzzerPin = 26; // Buzzer pin
const int kLed1Pin = 13;   // LED 1 pin
const int kLed2Pin = 12;   // LED 2 pin
const int kLed3Pin = 14;   // LED 3 pin
const int kLed4Pin = 27;   // LED 4 pin

// --- ALARM TIMING & TONES ---
const unsigned long kBlinkIntervalMs = 150; // LED blink frequency
const int kToneFrequency = 2500;            // Buzzer tone frequency (2500 Hz)
const unsigned long kPIR_RESET_TIME_MS = 2500; // 2.5 seconds cooldown

// --- STATE VARIABLES ---
bool isAlarmActive = false; 
unsigned long lastBlinkTime = 0;
unsigned long motionStoppedTime = 0;
bool isLedsOn = false; 

// --- CORE FUNCTIONS ---

// Function to handle the physical alarm outputs
void handleAlarmOutputs() {
  if (isAlarmActive) {
    // 1. BUZZER: Ring using tone()
    tone(kBuzzerPin, kToneFrequency);

    // 2. LEDS: Blink using non-blocking timer 
    if (millis() - lastBlinkTime >= kBlinkIntervalMs) {
      isLedsOn = !isLedsOn; 
      lastBlinkTime = millis();
    }
    
    // Apply the current blinking state to all 4 LEDs
    int ledState = isLedsOn ? HIGH : LOW;
    digitalWrite(kLed1Pin, ledState);
    digitalWrite(kLed2Pin, ledState);
    digitalWrite(kLed3Pin, ledState);
    digitalWrite(kLed4Pin, ledState);

  } else {
    // Ensure ALL outputs are OFF when no alarm is active
    noTone(kBuzzerPin); 
    digitalWrite(kLed1Pin, LOW);
    digitalWrite(kLed2Pin, LOW);
    digitalWrite(kLed3Pin, LOW);
    digitalWrite(kLed4Pin, LOW);
    isLedsOn = false; 
  }
}

// --- SETUP ---
void setup() {
  Serial.begin(115200); 

  // Configure Input Pin
  pinMode(kPirPin, INPUT);

  // Configure Output Pins (Initialize OFF)
  pinMode(kBuzzerPin, OUTPUT);
  pinMode(kLed1Pin, OUTPUT);
  pinMode(kLed2Pin, OUTPUT);
  pinMode(kLed3Pin, OUTPUT);
  pinMode(kLed4Pin, OUTPUT);
  
  digitalWrite(kLed1Pin, LOW); 
  digitalWrite(kLed2Pin, LOW);
  digitalWrite(kLed3Pin, LOW);
  digitalWrite(kLed4Pin, LOW);

  // Initialize Blynk Connection
  Blynk.begin(auth, ssid, pass);

  Serial.println("------------------------------------------");
  Serial.println("Attempting to connect to WiFi and Blynk...");
  Serial.println("------------------------------------------");
}

// --- MAIN LOOP ---
void loop() {
  // Must run constantly to maintain connection and process data
  Blynk.run();
  
  // 1. --- READ PIR STATE ---
  bool currentPIRState = (digitalRead(kPirPin) == HIGH);
  
  if (currentPIRState) {
    if (!isAlarmActive) {
      String alertMsg = "\n*** ALARM TRIGGERED: MOTION DETECTED ***";
      Serial.println(alertMsg);
      Blynk.virtualWrite(V1_STATUS_LOG, alertMsg); // Send text alert
      Blynk.virtualWrite(V0_ALARM_STATUS, 1);     // Turn ON Blynk LED
      isAlarmActive = true;
    }
    motionStoppedTime = 0; 
    
  } else {
    if (isAlarmActive && motionStoppedTime == 0) {
      motionStoppedTime = millis();
    }

    if (motionStoppedTime != 0 && millis() - motionStoppedTime >= kPIR_RESET_TIME_MS) {
      if (isAlarmActive) {
        String resetMsg = "--- Alarm Reset: Cooldown Expired ---";
        Serial.println(resetMsg);
        Blynk.virtualWrite(V1_STATUS_LOG, resetMsg); // Send reset alert
        Blynk.virtualWrite(V0_ALARM_STATUS, 0);     // Turn OFF Blynk LED
        isAlarmActive = false; 
      }
      motionStoppedTime = 0; 
    }
  }
  
  // 2. --- HANDLE PHYSICAL ALARM OUTPUTS ---
  handleAlarmOutputs();
}
