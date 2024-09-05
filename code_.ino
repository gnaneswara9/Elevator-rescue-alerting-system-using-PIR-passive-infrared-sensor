#include <BluetoothSerial.h>

#define VIN A0 // define the Arduino pin A0 as voltage input (V in)
const float VCC = 3.3; // supply voltage is 3.3V for ESP32
const int model = 2;   // enter the model number (see below)

float cutOffLimit = 1.01; // set the current which below that value, doesn't matter. Or set 0.5

float sensitivity[] = {
    0.185, // for ACS712ELCTR-05B-T
    0.100, // for ACS712ELCTR-20A-T
    0.066  // for ACS712ELCTR-30A-T
};

const float QOV = 0.5 * VCC; // set quiescent Output voltage of 0.5V
float voltage;                // internal variable for voltage

int pirSensor = 2;    // PIR sensor connected to digital pin 2
int state = LOW;      // by default, no motion detected

BluetoothSerial SerialBT; // Create an instance of BluetoothSerial

void setup() {
    Serial.begin(9600); // initialize serial monitor
    SerialBT.begin("ESP32_BT"); // Initialize Bluetooth with the name "ESP32_BT"
    Serial.println("ACS712 Current Sensor and PIR Sensor");
    pinMode(pirSensor, INPUT); // initialize PIR sensor as an input
}

void loop() {
    float voltageRaw = (VCC / 4095.0) * analogRead(VIN); // Read the voltage from sensor for ESP32
    voltage = voltageRaw - QOV + 0.012; // Adjust voltage
    float current = voltage / sensitivity[model]; // Calculate current

    if (abs(current) > cutOffLimit) {
        SerialBT.println("High Current Detected!"); // Send message over Bluetooth
    } else {
        int pirValue = digitalRead(pirSensor); // read PIR sensor value

        if (pirValue == HIGH) { // check if the PIR sensor is HIGH (motion detected)
            delay(100); // delay 100 milliseconds

            if (state == LOW) {
                SerialBT.println("Motion Detected!"); // Send message over Bluetooth
                state = HIGH; // update motion state to HIGH
            }
        } else {
            delay(200); // delay 200 milliseconds

            if (state == HIGH) {
                SerialBT.println("No Motion Detected."); // Send message over Bluetooth
                state = LOW; // update motion state to LOW
            }
        }
    }

    delay(5000); // delay between sensor readings
}
