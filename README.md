
# Smart Baby Monitor System - Real-time Cry Detection and Health Monitoring

This repository contains the code and documentation for a **Baby Monitor System** using two Arduino microcontrollers: **Arduino Nicla Voice** for real-time AI cry detection and **Arduino Nano 33 IoT** for health monitoring (body temperature, room temperature, and heart rate). The system provides caregivers with real-time information about the baby’s well-being and triggers alerts for different events.

## Features

- **Real-Time Cry Detection**: AI-powered cry detection using **Edge Impulse** for accurate classification of baby cries.
- **Health Monitoring**: Monitors baby’s **body temperature**, **room temperature**, and **heart rate**.
- **Mobile App Notifications**: Sends real-time alerts to the mobile app via Wi-Fi to notify caregivers.
- **Non-Invasive Measurement**: Uses **MLX90614 contactless infrared thermometer** for body and room temperature monitoring.
- **AI Integration**: The cry detection model is deployed on the **Arduino Nicla Voice** microcontroller, leveraging machine learning models trained with **Edge Impulse**.

## Hardware Setup

The Baby Monitor System consists of two main hardware units:

1. **Arduino Nicla Voice**:
   - Microcontroller used for **AI-based cry detection** using audio analysis.
   - It integrates with the **Edge Impulse** platform to run the trained AI model for detecting baby cries.

2. **Arduino Nano 33 IoT**:
   - Microcontroller responsible for **transmitting data** to the mobile app via Wi-Fi.
   - It handles **temperature measurements** using the **MLX90614 infrared thermometer** and monitors the **heart rate sensor**.

### Sensors and Modules:
- **MLX90614 Contactless Infrared Thermometer**: Used for measuring the baby's body temperature and room temperature.
- **Heart Rate Sensor**: Used to monitor the baby's heart rate.

## Software Setup

### Required Libraries:
- `ArduinoBLE` - For Bluetooth Low Energy (BLE) communication.
- `Edge Impulse` - For training and deploying the AI model for cry detection.
- `BLEDevice` - For handling BLE communication between devices.

### Deployment Options:
1. **Deployment using the Edge Impulse Deployment Library** (`NDP120`):
   - This option allows integration of the AI cry detection model into the firmware using the `NDP120` library, ensuring real-time cry detection on the **Nicla Voice** the AI model available in cry_detect-nicla-voice-v12 file.

2. **Deployment as a Binary File**:
   - The AI model is compiled into a binary file that is uploaded to the **Nicla Voice**, making the system operate autonomously without needing a connection to the Edge Impulse platform.

## Phases of Operation

### 1. **Initialization Phase**:
   - The system powers up and initializes the **Arduino Nicla Voice** and **Arduino Nano 33 IoT** microcontrollers, as well as other sensors like the **MLX90614 thermometer** and **heart rate sensor**.

### 2. **Cry Detection Phase**:
   - The **Nicla Voice** uses the AI model trained on **Edge Impulse** to analyze audio in real time and detect the presence of a baby’s cry. The result is transmitted to the **Arduino Nano 33 IoT** for further processing.

### 3. **Data Transmission Phase**:
   - The **Nano 33 IoT** transmits the cry detection status and other health data to the mobile app via Wi-Fi.

### 4. **Body Temperature Measurement Phase**:
   - The **MLX90614** thermometer measures the baby’s body temperature without physical contact and sends the data to the **Nano 33 IoT**, which then forwards it to the mobile app.

### 5. **Room Temperature Monitoring Phase**:
   - The **MLX90614** also monitors the room temperature and sends alerts if it detects dangerous conditions like high temperatures (potential fire risk).

### 6. **Heart Rate Monitoring Phase**:
   - The **Nano 33 IoT** acquires the baby’s heart rate data from the heart rate sensor and sends it to the mobile app for display and analysis.

## System Requirements

- **Arduino Nicla Voice** for AI-based cry detection.
- **Arduino Nano 33 IoT** for health monitoring and Wi-Fi communication.
- **MLX90614 Infrared Thermometer** for body and room temperature monitoring.
- **Heart Rate Sensor** for monitoring the baby's heart rate.
- **Mobile App** for receiving notifications and alerts (the app should be developed separately).
- **Edge Impulse** account for training and deploying the AI cry detection model.

## How to Use

1. **Set up the hardware**:
   - Connect the **Nicla Voice** and **Nano 33 IoT** according to the wiring diagrams provided in the repository.
   - Install the necessary sensors: **MLX90614** for temperature monitoring and a **heart rate sensor**.
   
2. **Train the AI model**:
   - Use **Edge Impulse** to collect and label cry samples for training.
   - Deploy the trained AI model on the **Nicla Voice** either using the **NDP120 library** or by flashing the model as a binary.

3. **Upload the Code**:
   - Upload the provided code to the **Nicla Voice** and **Nano 33 IoT** using the Arduino IDE.
   - Ensure the code for both microcontrollers is correctly uploaded to enable proper communication and functionality.

4. **Mobile App Integration**:
   - Ensure the mobile app is configured to receive data from the **Nano 33 IoT** and display real-time alerts for cry detection, body temperature, room temperature, and heart rate.

## Troubleshooting

- **Problem: No cry detection or false positives**:
  - Make sure the **Edge Impulse** model is properly trained with enough labeled cry data.
  - Ensure the microphone is clear of obstructions to allow accurate audio detection.

- **Problem: No Wi-Fi connection**:
  - Check the **Arduino Nano 33 IoT** Wi-Fi connection settings and ensure the network credentials are correctly configured in the code.

- **Problem: Temperature measurements not accurate**:
  - Verify that the **MLX90614** sensor is correctly calibrated and positioned for accurate readings.

## Contributing

Feel free to submit pull requests, report issues, or suggest improvements to this project. Contributions are welcome!

## License
=======
>>>>>>> 57c94e3dd9cc55fd52d03ef0f4b5b7faab5f35f1

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details.
