# Project Phases

This document outlines the iterative development process of the pulse oximeter system. The project evolved from a basic SpO₂ monitor into a modular embedded sensing and wireless telemetry platform with real-time waveform visualization and browser-based monitoring.

---

# Phase 0 — Baseline SpO₂ Monitor

## Objective
Develop a functional pulse oximeter capable of estimating blood oxygen saturation using the MAX30102 optical sensor and displaying measurements on an OLED display.

## Features
- MAX30102 optical sensor integration
- SpO₂ estimation using Maxim Integrated's algorithm
- OLED text display
- Real-time sensor acquisition
- Sliding-window sample buffering
- Serial debugging output

## Hardware
- Arduino Nano 33 BLE Sense Rev2
- MAX30102 pulse oximeter sensor
- SSD1306 OLED display

## Outcome
A stable embedded system capable of displaying real-time SpO₂ estimates under controlled measurement conditions.

---

# Phase 1 — Waveform Visualization and Real-Time Rendering

## Objective
Extend the system from a numerical monitor into a physiological signal visualization platform.

## Features Added
- Real-time PPG waveform rendering
- Circular waveform buffer
- Dynamic signal scaling
- Continuous OLED graph updates
- Graphics-based OLED rendering

## Technical Changes
- Migrated from SSD1306Ascii to Adafruit_SSD1306
- Added graphics rendering using Adafruit_GFX
- Implemented waveform plotting using infrared (IR) sensor samples
- Improved display refresh behavior for continuously updating waveform visualization

## Outcome
The OLED display evolved from a static text interface into a continuously updating physiological waveform monitor capable of visualizing pulse-related signal behavior in real time.

---

# Phase 2 — Firmware Refactor

## Objective
Improve maintainability, modularity, and scalability of the firmware.

## Features Added
- Modular function-based architecture
- Separation of acquisition, computation, rendering, and communication logic

## Refactored Components
- Sensor initialization
- OLED initialization
- Sample acquisition
- Buffer shifting
- SpO₂ computation
- Waveform rendering
- Screen rendering

## Outcome
The codebase became significantly easier to maintain and extend for future wireless and networking features.

---

# Phase 3 — BLE Telemetry

## Objective
Enable wireless transmission of physiological data using Bluetooth Low Energy (BLE).

## Features Added
- BLE peripheral advertising
- BLE telemetry characteristics
- Wireless SpO₂ broadcasting

## Technical Changes
- Integrated ArduinoBLE library
- Added BLE service and characteristic definitions
- Added wireless telemetry updates within acquisition loop

## Outcome
The Nano 33 BLE Sense Rev2 became both:
- a local embedded monitor
- a wireless BLE sensing device

---

# Phase 4 — Browser Dashboard Bridge

## Objective
Create a browser-based monitoring interface using BLE telemetry.

## System Architecture

Nano BLE Device → Python Bridge → Flask Web Dashboard

## Features Added
- Python BLE bridge using Bleak
- Flask-based browser dashboard
- Live telemetry display
- Wireless monitoring interface

## Technologies Used
- Python
- Bleak
- Flask
- ArduinoBLE

## Outcome
The system evolved into a complete embedded-to-web telemetry pipeline capable of wireless browser-based physiological monitoring.

---

# Future Phase — ESP32 Standalone Dashboard

## Planned Objective
Eliminate the laptop bridge and host the web dashboard directly on embedded hardware.

## Planned Features
- ESP32 migration
- Native Wi-Fi dashboard hosting
- Browser-based waveform streaming
- Mobile-friendly interface
- Reduced system complexity

## Planned Architecture

ESP32 → Wi-Fi → Browser Dashboard

## Motivation
The ESP32 platform provides integrated Wi-Fi support, enabling direct browser communication without requiring an external BLE bridge application.

---

# Future Improvements

Potential future improvements include:

- Signal smoothing and visualization improvements
- Motion artifact reduction
- Historical data logging
- Battery-powered operation
- Improved UI/UX
- Real-time browser waveform plotting
- WebSocket-based streaming
- Cloud telemetry integration

---

# Summary

The project evolved through multiple iterative phases:

1. Baseline SpO₂ monitor
2. Real-time waveform visualization
3. Firmware modularization
4. BLE telemetry integration
5. Browser-based dashboarding

The resulting system combines embedded systems engineering, biomedical sensing, real-time visualization, wireless communication, and web-based telemetry into a unified pulse oximeter platform.