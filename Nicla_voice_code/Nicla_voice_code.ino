#include "NDP.h"
#include <ArduinoBLE.h>

// Constants for BLE and device configurations
const char* DEVICE_NAME = "Nicla";  // Name of the BLE device
const bool LOWEST_POWER = false;    // Flag for power management

// BLE UUIDs for the alert service and characteristics
const char* ALERT_SERVICE_UUID = "71195536-01c6-11ee-be56-0242ac120002";  // Service UUID
const char* ALERT_LEVEL_UUID = "797584d4-01c6-11ee-be56-0242ac120002";    // Characteristic UUID for alert level
const char* ALERT_STR_UUID = "b0e86dc6-01cd-11ee-be56-0242ac120002";      // Characteristic UUID for alert string

// BLE Service and Characteristics
BLEService alertService(ALERT_SERVICE_UUID);  // BLE service for alert system
BLEByteCharacteristic alertLevel(ALERT_LEVEL_UUID, BLERead | BLENotify);  // BLE characteristic to send alert level
BLECharacteristic alertStr(ALERT_STR_UUID, BLERead | BLENotify, "start recording");  // BLE characteristic for alert string

// Global variable to track BLE connection status
bool isConnected = false;

// Utility Functions for LED control
// Function to set the LED color for a given duration (default is 200ms)
void setLEDColor(const char* color, int duration = 200) {
    nicla::leds.begin();                // Initialize LED system
    nicla::leds.setColor(color);        // Set the LED to the specified color
    delay(duration);                    // Wait for the given duration
    nicla::leds.setColor(off);          // Turn off the LED
    nicla::leds.end();                  // Deinitialize the LED system
}

// Function to blink the LED a given number of times with a specified color and delay
void blinkLED(const char* color, int count, int delayMs = 200) {
    for (int i = 0; i < count; i++) {
        setLEDColor(color, delayMs);   // Blink the LED with the specified color
        delay(delayMs);                // Delay between blinks
    }
}

// Function to indicate an error by blinking the LED red indefinitely
void indicateError() {
    while (true) {
        blinkLED(red, 1, 500);         // Blink red LED with a 500ms delay
    }
}

// Callback function to send data over BLE based on the detected label
void BLEsend(char* label) {
    // Check if the label is "NN0:Cry" and set alert level to 1 (cry detected)
    if (strcmp(label, "NN0:Cry") == 0) {
        alertLevel.writeValue(1);     // Set alert level to 1 (cry detected)
        Serial.println("Cry detected.");
        NDP.noInterrupts();           // Disable interrupts temporarily
        setLEDColor(green, 3000);     // Set LED color to green for 3 seconds
        NDP.interrupts();             // Re-enable interrupts
    } 
    // If the label is "NN0:No_Cry", set alert level to 0 (no cry detected)
    else if (strcmp(label, "NN0:No_Cry") == 0) {
        alertLevel.writeValue(0);     // Set alert level to 0 (no cry detected)
        Serial.println("No Cry detected.");
        NDP.noInterrupts();           // Disable interrupts temporarily
        setLEDColor(red, 3000);       // Set LED color to red for 3 seconds
        NDP.interrupts();             // Re-enable interrupts
    }

    // If not in lowest power mode, print the label to the serial monitor
    if (!LOWEST_POWER) {
        Serial.println(label);
    }
}

// Function to set up BLE communication
void setupBLE() {
    if (!BLE.begin()) {  // Check if BLE initialization is successful
        Serial.println("Starting BLE failed!");  // Print error message if BLE setup fails
        indicateError();  // Call indicateError to blink red LED and halt
    }

    BLE.setLocalName(DEVICE_NAME);           // Set local device name for BLE
    BLE.setAdvertisedService(alertService);  // Advertise the alert service
    alertService.addCharacteristic(alertLevel);  // Add alert level characteristic
    alertService.addCharacteristic(alertStr);    // Add alert string characteristic
    BLE.addService(alertService);            // Add the service to the BLE stack
    alertLevel.writeValue(0);                // Set initial alert level to 0 (no alert)

    BLE.advertise();  // Start advertising the BLE service
    Serial.println("BLE setup complete.");  // Print setup complete message
}

// Function to set up NDP (Neural Detection Processor) for detecting events
void setupNDP() {
    NDP.onError(indicateError);   // Set callback for error handling
    NDP.onMatch(BLEsend);         // Set callback to send data over BLE on match
    NDP.onEvent([]() { setLEDColor(green); });  // Set LED color to green when an event occurs

    Serial.println("Loading synpackages...");
    NDP.begin("mcu_fw_120_v91.synpkg");  // Load MCU firmware
    NDP.load("dsp_firmware_v91.synpkg");  // Load DSP firmware
    NDP.load("ei_model.synpkg");          // Load EI model for event detection
    Serial.println("Packages loaded.");   // Print message after loading packages

    NDP.getInfo();                      // Get information from the NDP device
    Serial.println("Configuring microphone...");
    NDP.turnOnMicrophone();             // Enable microphone for sound detection
}

// Setup function to initialize the system
void setup() {
    Serial.begin(115200);  // Start serial communication at 115200 baud rate

    // Initialize Nicla system
    nicla::begin();
    nicla::disableLDO();  // Disable Low Dropout regulator (used for power optimization)
    nicla::leds.begin();   // Initialize LEDs

    // Set up BLE and NDP components
    setupBLE();
    setupNDP();

    nicla::leds.end();  // End LED initialization
}

// Main loop function for continuous operation
void loop() {
    BLEDevice central = BLE.central();  // Check if a central device is connected via BLE

    if (central) {  // If a central device is connected
        if (!isConnected) {  // If not already connected
            Serial.println("Connected to BLE central.");  // Print connection message
            blinkLED(blue, 3);  // Blink blue LED 3 times
            NDP.interrupts();   // Enable NDP interrupts
            isConnected = true;  // Set connected flag to true
        }

        // While the BLE central device is connected
        while (central.connected()) {
            NDP.onMatch(BLEsend);  // Continue processing matches (such as "NN0:Cry" or "NN0:No_Cry")
        }
    } 
    else {  // If no central device is connected
        if (isConnected) {  // If previously connected
            Serial.println("Disconnected from BLE central.");  // Print disconnection message
            isConnected = false;  // Set connected flag to false
            NDP.noInterrupts();   // Disable NDP interrupts
        }

        delay(1000);  // Delay before next polling
    }
}
