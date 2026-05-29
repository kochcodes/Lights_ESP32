# Lights ESP32

ESP32-based BLE-enabled light controller for synchronized wireless lighting systems. Acts as the central BLE hub for controlling multiple ESP-NOW synchronized lights via iOS app.

## Description

This project implements the ESP32 firmware for a wireless lighting control system. The ESP32 serves as the BLE bridge between the iOS control app and the ESP-NOW mesh network of synchronized lights.

## Features

- **BLE Server** - Provides BLE connectivity for iOS/macOS control apps
- **ESP-NOW Integration** - Communicates with mesh network of lights
- **Multi-Device Sync** - Coordinates with ESP8266-based light nodes
- **Animation Modes** - Supports various lighting animations and patterns
- **Power Control** - Adjustable brightness and power settings
- **Low Latency** - Sub-millisecond synchronization with mesh

## System Architecture

```
iOS App (Lights_ios)
       ↓ BLE
   ESP32 Hub ← → ESP8266 Lights
   (This repo)    ↑ ESP-NOW ↑
                  └─ Mesh ──┘
```

## Related Projects

- **[Lights_ios](https://github.com/kochcodes/Lights_ios)** - iOS control app with BLE interface
- **[ESP8266MultiDeviceSyncLight](https://github.com/kochcodes/ESP8266MultiDeviceSyncLight)** - Synchronized ESP8266 light nodes

## Hardware Requirements

- ESP32 Development Board (any variant with BLE support)
- LED strips or addressable LEDs
- Power supply appropriate for your LED configuration

## Installation

### Prerequisites

- [PlatformIO](https://platformio.org/) installed
- USB cable for programming
- ESP32 board

### Build and Upload

1. Clone the repository:
```bash
git clone https://github.com/kochcodes/Lights_ESP32.git
cd Lights_ESP32
```

2. Build and upload:
```bash
pio run -t upload
```

## Usage

1. Upload firmware to ESP32
2. Power on the ESP32 - BLE service starts automatically
3. Launch the iOS app (Lights_ios)
4. Connect to the ESP32 via BLE
5. Control lights - Switch modes, adjust power, change animations

## License

Open source - check repository for license details.
