# Pulse Oximeter - MAX30102 SpO2 Monitor

A real-time blood oxygen saturation (SpO2) monitoring system using the MAX30102 sensor and Arduino Nano 33 BLE Sense Rev2.

![Device in use](images/device_demo.jpg)

## Overview

This project implements a functional pulse oximeter that measures blood oxygen saturation levels using photoplethysmography (PPG). The device displays real-time SpO2 readings on an OLED screen and has been optimized for accuracy across different skin tones.

**Key Features:**
- Real-time SpO2 measurement (updates every 1 second)
- 128x64 OLED display with clear readings
- Signal validation to ensure accurate measurements
- Optimized LED brightness for universal usability
- Works across different skin tones

## Project Motivation

As a freshman in Electrical and Computer Engineering at USC, I built this project to:
- Understand biomedical signal processing
- Learn embedded systems integration (I2C communication, sensor interfacing)
- Gain hands-on experience with real-world medical device technology
- Develop debugging and optimization skills

## Hardware Components

| Component | Model/Type | Purpose |
|-----------|------------|---------|
| Microcontroller | Arduino Nano 33 BLE Sense Rev2 | Main processor |
| SpO2 Sensor | MAX30102 | Pulse oximetry sensor (Red + IR LEDs) |
| Display | SSD1306 OLED 128x64 (I2C) | Real-time reading display |
| Breadboard | Standard 830-point | Prototyping |
| Jumper Wires | Male-to-male | Connections |
| USB Cable | Micro-USB | Power & programming |

**Total Cost:** ~$25-30

## Wiring Diagram
```
MAX30102 Sensor:
  VIN  → 3.3V
  GND  → GND
  SDA  → A4 (SDA)
  SCL  → A5 (SCL)

SSD1306 OLED Display:
  VCC  → 3.3V
  GND  → GND
  SDA  → A4 (SDA)
  SCL  → A5 (SCL)
```

![Wiring Diagram](images/wiring_diagram.png)

## Software Implementation

### Libraries Used:
- `Wire.h` - I2C communication
- `MAX30105.h` - MAX30102 sensor driver
- `spo2_algorithm.h` - Maxim Integrated's SpO2 calculation algorithm
- `SSD1306Ascii.h` / `SSD1306AsciiWire.h` - OLED display driver

### Key Algorithm:
The device uses Maxim Integrated's proprietary algorithm for calculating SpO2 from photoplethysmography signals. The algorithm analyzes the ratio of AC/DC components in red and infrared light absorption to determine oxygen saturation.

**Signal Processing Pipeline:**
1. Collect 100 samples over 4 seconds (25 samples/second effective rate)
2. Extract AC (pulsatile) and DC (baseline) components
3. Calculate ratio: R = (AC_red/DC_red) / (AC_ir/DC_ir)
4. Convert to SpO2 percentage using calibration curve
5. Validate signal quality before displaying

## Optimization Process

### LED Brightness Calibration:
Through empirical testing, I optimized the LED brightness for reliable readings:

| Brightness | Result | Signal Range |
|------------|--------|--------------|
| 60 | Too high for dark skin | 200k-202k (near saturation) |
| 55 | **Optimal** | 150k-190k (stable) |
| 40 | Too low | <50k (weak signal) |

**Final setting: 55** - Provides robust performance across different skin tones while avoiding sensor saturation.

### Design Decisions:
1. **Removed heart rate display**: Motion artifacts made HR readings unreliable; focused on SpO2 accuracy
2. **Validation logic**: Only displays readings when `validSPO2 == 1` to ensure data quality
3. **Sliding window**: Uses 100-sample buffer with 25-sample updates for smooth, responsive measurements

## Testing & Validation

### Test Results:
- **Accuracy**: Consistent readings of 99-100% SpO2 (healthy baseline)
- **Stability**: Maintains accurate readings for 5+ minutes
- **Universality**: Successfully tested on subjects with dark and light skin tones
- **Response Time**: Initial reading in 4 seconds, updates every 1 second

### Known Limitations:
- Requires proper finger placement (firm but not tight contact)
- Motion artifacts can cause temporary invalid readings
- Breadboard setup is fragile (needs enclosure for practical use)
- No data logging or historical tracking (potential future enhancement)

## How to Use

1. **Upload Code:**
```bash
   # Install required libraries in Arduino IDE:
   # - Adafruit SSD1306
   # - SparkFun MAX3010x
   
   # Open pulse_oximeter.ino and upload to Arduino Nano 33 BLE
```

2. **Connect Hardware:**
   - Follow wiring diagram above
   - Ensure I2C connections are secure

3. **Power On:**
   - Connect USB cable to laptop/power source
   - Open Serial Monitor (115200 baud) for debug output

4. **Measure SpO2:**
   - Place finger on MAX30102 sensor
   - Press any key in Serial Monitor to start
   - Keep finger still for best results
   - Reading appears on OLED display when valid

## Troubleshooting

| Issue | Possible Cause | Solution |
|-------|----------------|----------|
| "Sensor Error!" | Wiring problem | Check I2C connections (SDA/SCL) |
| Random readings (40-89%) | Poor finger contact | Reposition finger, ensure firm contact |
| No display update | Invalid signal | Keep finger still, wait 4 seconds |
| Display shows "Reading..." | Signal too weak/strong | Adjust finger pressure |

## Technical Background

### Photoplethysmography (PPG):
Pulse oximetry works by shining red (660nm) and infrared (880nm) light through tissue and measuring absorption. Oxygenated hemoglobin (HbO2) and deoxygenated hemoglobin (Hb) absorb these wavelengths differently:

- **Red light**: More absorbed by Hb (deoxygenated blood)
- **IR light**: More absorbed by HbO2 (oxygenated blood)

The ratio of absorption at these wavelengths indicates oxygen saturation percentage.

### Why SpO2 is Stable but Heart Rate Isn't:
- **SpO2 calculation**: Based on ratios of signal components - relatively immune to motion
- **Heart rate calculation**: Requires precise peak detection - highly sensitive to artifacts

## What I Learned

### Technical Skills:
- Embedded C programming for Arduino
- I2C communication protocol implementation
- Real-time signal processing and filtering
- Hardware debugging (oscilloscope, multimeter)
- Biomedical sensor integration

### Engineering Process:
- Iterative testing and optimization (LED brightness calibration)
- Design trade-offs (accuracy vs. complexity)
- User experience considerations (display feedback, validation)
- Documentation and version control (Git/GitHub)

### Problem-Solving:
- Diagnosed and fixed LED saturation issues
- Optimized settings for different use cases
- Balanced signal quality with universal usability

## 🔮 Future Enhancements

Potential improvements for v2.0:
- [ ] Data logging to SD card or cloud
- [ ] Bluetooth connectivity for mobile app
- [ ] Real-time SpO2 trend graphing
- [ ] Battery power for portability
- [ ] 3D-printed enclosure
- [ ] Alert system for low SpO2 (buzzer/LED)
- [ ] Signal quality indicator on display

## 📖 References

- [MAX30102 Datasheet](https://datasheets.maximintegrated.com/en/ds/MAX30102.pdf)
- [Maxim Integrated Application Note: Pulse Oximeter Design](https://www.maximintegrated.com/en/design/technical-documents/app-notes/6/6845.html)
- [Arduino Example Code](https://docs.arduino.cc/tutorials/nano-33-ble-sense-rev2/max30102-sensor/) (adapted and optimized)
- [Photoplethysmography Theory](https://en.wikipedia.org/wiki/Photoplethysmogram)

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 👤 Author

**Evan Torrence**
- Electrical and Computer Engineering, University of Southern California
- [GitHub](https://github.com/etorrence)
- [LinkedIn](https://www.linkedin.com/in/evantorrence/)
- Email: etorrenc@usc.edu

## 🙏 Acknowledgments

- Maxim Integrated for the spo2_algorithm library
- Arduino community for sensor libraries
- USC Viterbi School of Engineering

---

**Status:** ✅ Functional prototype - tested and validated  
**Last Updated:** January 2026
