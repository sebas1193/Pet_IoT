# Smart Pet Food Dispenser

This project aims to implement fundamental IoT concepts to develop an automated pet food dispensing system. The system collects data on presence, food level, and schedules, allowing feeding to be optimized through a predictive model. It also considers the future development of a module for water flow control.

## Objectives

- Automate the feeding process.
- Collect and analyze relevant data to predict feeding times.
- Develop a predictive model that optimizes food dispensing.
- Explore new functionalities, such as water flow control.

## Tools Used

- Maquiatto
- Node-RED
- MySQL
- PlatformIO

## Project Components

- ESP32
- Infrared Sensor
- SG90 Servo Motor
- Ultrasonic Sensor

## Project Structure

```plaintext
.
├── README.md
├── lib                # Custom libraries for better code organization (OOP)
│   ├── Credentials
│   │   └── Credentials.h   # Credential management
│   ├── Servo_Ctrl
│   │   └── Servo_Ctrl.h      # Servo motor control
│   └── WiFiConnection
│       └── WiFiConnection.h  # WiFi connection management
├── platformio.ini     # ESP module configuration
└── src
    └── main.cpp        # Main code
```
