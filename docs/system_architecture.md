# System Architecture

```text
[ Finger / Tissue ]
        →
[ MAX30102 Sensor ]
        →
[ Arduino Nano 33 BLE Sense Rev2 ]
        →
[ SpO₂ Computation + Waveform Buffering ]
        →
[ OLED Display + BLE Telemetry ]
        →
[ Python Bridge (Bleak) ]
        →
[ Flask Web Dashboard ]
```

This system uses optical photoplethysmography (PPG) to estimate blood oxygen saturation (SpO₂) from red and infrared light reflected from finger tissue.

The microcontroller acquires raw sensor data over I²C, stores samples in a sliding window, computes SpO₂ using Maxim's algorithm, renders the waveform on the OLED, and broadcasts telemetry over Bluetooth Low Energy.

A Python bridge reads BLE characteristics from the Nano and serves the live data to a browser dashboard. The design emphasizes modularity, iterative development, and real-time visualization.

The current implementation focuses on:
- embedded data acquisition
- waveform visualization
- BLE telemetry
- browser-based monitoring

Future work includes migrating the dashboard directly onto ESP32 hardware for standalone Wi‑Fi hosting.