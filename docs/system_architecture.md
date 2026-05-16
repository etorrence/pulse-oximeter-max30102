[ Finger / Tissue ]
        ↓
[ MAX30102 Sensor ]
        ↓
[ Signal Conditioning (filtering) ]
        ↓
[ SpO₂ + BPM Computation ]
        ↓
[ OLED Display / Data Logging ]


This system is designed to estimate heart rate (BPM) and blood oxygen saturation (SpO₂) using optical photoplethysmography (PPG). Light emitted from red and infrared LEDs passes through finger tissue, and the reflected signals are captured by an optical sensor and converted into electrical signals.

These raw PPG signals are transmitted via I²C to a microcontroller, which performs signal conditioning and feature extraction to reduce noise and isolate the pulsatile components of the waveform. Processed signals are then used to compute BPM and SpO₂ estimates using established ratio-based methods.

The system is organized into modular functional blocks—sensing, signal processing, computation, and output—to improve clarity and support iterative development. Computed metrics are displayed in real time on an OLED screen and optionally logged for offline analysis. The design assumes controlled measurement conditions and acknowledges known limitations related to motion artifacts and ambient light interference.