#include <SPI.h>
#include <Arduino_LSM6DS3.h>
#include <Firebase_Arduino_WiFiNINA.h>
#include <ArduinoBLE.h>
#include <Adafruit_MLX90614.h> 
#include "utility/wifi_drv.h"
#include <stdlib.h>
#include <PulseSensorPlayground.h>     // Includes the PulseSensorPlayground Library for heart rate sensor

// Firebase and Wi-Fi credentials
#define FIREBASE_HOST "tottracker-238bb-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "W99dF8coTpnTF6WKKCcxufPW8PYmwORxOhjbYBAQ"
#define WIFI_SSID "wifi_name"
#define WIFI_PASSWORD "wifi_pass"

// Firebase path for storing vitals data
String path = "/Vitals";
byte dataR;  // Variable to hold cry detection state
int status = WL_IDLE_STATUS;  // Wi-Fi connection status

// Heart rate sensor setup
const int PulseWire = 0;       // 'S' Signal pin connected to A0
const int LED13 = 13;          // The on-board Arduino LED
int Threshold = 550;           // Signal threshold to determine heart beat
PulseSensorPlayground pulseSensor;  // Creates a PulseSensor object

// MLX90614 temperature sensor setup
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

// Function to read room temperature
float roomtemp(){
  mlx.begin();
  float x = mlx.readAmbientTempC();  // Read and return ambient room temperature
  return x;
}

// Function to read body temperature
float bodytemp(){
  mlx.begin();
  float x = mlx.readObjectTempC();  // Read and return object (body) temperature
  return x;
}

// Function to detect baby crying based on input (1 for crying, 0 for not crying)
float cryI(byte x){
  if(x == 1) {
    return 1;  // Cry detected
  } else {
    return 0;  // No cry
  }
}

// Wi-Fi setup function
void wifisetup(){
  wiFiDrv.wifiDriverDeinit();  // Deinitialize previous Wi-Fi settings
  wiFiDrv.wifiDriverInit();    // Initialize Wi-Fi
  while (status != WL_CONNECTED) {
    status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);  // Try to connect to Wi-Fi
    Serial.print(".");
    delay(300);
  }
  Serial.println("\nConnected. ");
  Serial.print(" IP: ");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH, WIFI_SSID, WIFI_PASSWORD);  // Initialize Firebase
  Firebase.reconnectWiFi(true);  // Reconnect Wi-Fi automatically if disconnected
}

// Function to send vital data to Firebase
void sendvitals(float cry){
  // Sending room temp, body temp, heart rate, and cry detection status to Firebase
  Firebase.setFloat(firebaseData, path + "/1-setFloat/rt", roomtemp());
  Firebase.setFloat(firebaseData, path + "/1-setFloat/bt", bodytemp());
  Firebase.setFloat(firebaseData, path + "/1-setFloat/ht", heartrate());  // Heart rate from pulse sensor
  Firebase.setFloat(firebaseData, path + "/1-setFloat/c", cry);  // Cry status
  Firebase.setFloat(firebaseData, path + "/1-setFloat/htf", 1);  // Mark heart rate as available
  
  Serial.println("Data sent to Firebase");
}

// BLE initialization function
void start_ble(){
  if (!BLE.begin()){
    Serial.println("Failed to start Bluetooth® Low Energy module!");
    while (1);  // Stop execution if BLE initialization fails
  }
  Serial.println("Bluetooth® Low Energy Central ");
  BLE.scan();  // Start scanning for BLE peripherals
}

// Heart rate function that gets BPM from the Pulse Sensor
float heartrate() {
  int myBPM = pulseSensor.getBeatsPerMinute();  // Get Beats Per Minute from the pulse sensor
  if (pulseSensor.sawStartOfBeat()) {  // Check if a beat happened
    Serial.println("♥  A HeartBeat Happened !");
    Serial.print("BPM: ");
    Serial.println(myBPM);  // Print the heart rate (BPM) to the Serial Monitor
  }
  return myBPM;  // Return BPM value
}

// Setup function runs once at the beginning
void setup() {
  Serial.begin(115200);  // Initialize serial communication
  start_ble();  // Initialize Bluetooth Low Energy (BLE)

  // PulseSensor setup
  pulseSensor.analogInput(PulseWire);  // Set analog input pin for Pulse Sensor
  pulseSensor.blinkOnPulse(LED13);  // Blink the on-board LED with each heartbeat
  pulseSensor.setThreshold(Threshold);  // Set threshold for heartbeat detection

  // Initialize the PulseSensor
  if (pulseSensor.begin()) {
    Serial.println("PulseSensor object created!");
  }
}

// Main loop runs repeatedly
void loop() {
  BLEDevice peripheral = BLE.available();  // Check if any BLE peripherals are available

  if (peripheral) {  // If a peripheral is found
    Serial.print("Found ");
    Serial.print(peripheral.address());  // Print the peripheral's address
    Serial.print(" '");
    Serial.print(peripheral.localName());  // Print the peripheral's name
    Serial.print("' ");
    
    if (peripheral.localName() == "Nicla") {  // Check if the peripheral is "Nicla"
      BLE.stopScan();  // Stop scanning for peripherals
      master(peripheral);  // Handle communication with the connected peripheral
      BLE.scan();  // Start scanning again after disconnecting
    }
  }
}

// Function to handle communication with the Nicla peripheral
void master(BLEDevice peripheral) {
  Serial.println("Connecting ...");
  if (peripheral.connect()) {
    Serial.println("Connected");
  } else {
    Serial.println("Failed to connect!");
    return;
  }

  // Discover the service UUID and characteristics
  Serial.println("Discovering service 0xffe0 ...");
  if (peripheral.discoverService("71195536-01c6-11ee-be56-0242ac120002")) {
    Serial.println("Service discovered");
  } else {
    Serial.println("Attribute discovery failed.");
    peripheral.disconnect();
    return;
  }

  // Get the characteristic for simple key (used to detect cry)
  BLECharacteristic simpleKeyCharacteristic = peripheral.characteristic("797584d4-01c6-11ee-be56-0242ac120002");

  Serial.println("Subscribing to simple key characteristic ...");
  if (!simpleKeyCharacteristic) {
    Serial.println("No simple key characteristic found!");
    peripheral.disconnect();
    return;
  } else if (!simpleKeyCharacteristic.canSubscribe()) {
    Serial.println("simple key characteristic is not subscribable!");
    peripheral.disconnect();
    return;
  } else if (!simpleKeyCharacteristic.subscribe()) {
    Serial.println("Subscription failed!");
    peripheral.disconnect();
    return;
  } else {
    Serial.println("Subscribed to characteristic");
    Serial.println("Press the right and left buttons on your SensorTag.");
  }

  while (peripheral.connected()) {  // While the peripheral is still connected
    delay(3000);  // Delay to reduce excessive polling
    if (simpleKeyCharacteristic.valueUpdated()) {  // Check if the characteristic value has changed
      byte value = 0;
      simpleKeyCharacteristic.readValue(value);  // Read the value of the characteristic
      dataR = value;

      if (value == 1) {  // If right button (cry detected)
        Serial.println("Baby is crying");
        Serial.println("Sending cry data to Firebase");
        peripheral.disconnect();
        wifisetup();  // Reconnect to Wi-Fi
        float cryStatus = cryI(value);  // Set cry status
        sendvitals(cryStatus);  // Send vitals data to Firebase
        
        dataR = 0;  // Reset cry detection status
      }
      if (value == 0) {  // If left button (no cry)
        Serial.println("No cry detected");
      }
    }

    // Reconnect to Wi-Fi and send vitals data
    Serial.println("Reconnecting Wi-Fi");
    peripheral.disconnect();
    wifisetup();
    float cryStatus = cryI(dataR);
    sendvitals(cryStatus);
  }

  Serial.println("Nicla disconnected!");
  BLE.begin();
  BLE.scan();  // Restart BLE scan after disconnection
}
