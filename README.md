# Motion_detection_and_Alarming
An Embedded environment interfacing sensors with a microcontroller for intruder detection 

Components used:
•	ESP32 S3 Microcontroller
•	PIR Motion Sensor
•	LEDs and Resistor
•	Buzzer
•	Power Source
•	SSD-1306 OLED Display

Working Principle:
The project utilizes an ESP32-S3 microcontroller to manage a simple perimeter security system. In the setup() phase, the ESP32 initializes all GPIO pins: the buzzer (GPIO 1) and the four LEDs (GPIO 4-7) are configured as outputs and set to LOW (off), while the Passive Infrared (PIR) motion sensor (GPIO 2) is set as an input. The I2C communication bus is also initialized to control the SSD1306 OLED display.
The system operates within the continuous loop() function, constantly checking the state of the PIR sensor.
1.	"Clear" State (No Motion): When no movement is detected, the PIR sensor output is LOW. The ESP32 maintains the buzzer and all four LEDs in the OFF state. The OLED screen displays the status message: "System: CLEAR".
2.	"Alarm" State (Motion Detected): If the PIR sensor detects movement, its output goes HIGH. The logic immediately triggers the alarm: the LEDs are set to HIGH (constantly ON) and the buzzer is activated with a continuous tone (1000 Hz). The OLED display is updated instantly to show the alert message: "Intruder!!!".
3.	State Management: A simple tracking variable (is\_motion\_detected) ensures the display is only updated and the alarm messages are only printed to the Serial Monitor when the system transitions between the CLEAR and ALARM states, avoiding redundant updates.
