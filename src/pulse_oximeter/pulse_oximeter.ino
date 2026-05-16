// max30102 libraries
#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"

// BLE library
#include <ArduinoBLE.h>

// graphics oled libraries
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

MAX30105 particleSensor;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDR 0x3C
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define MAX_BRIGHTNESS 255
#define ANALYSIS_BUFFER_LENGTH 100
#define WAVE_BUFFER_LENGTH 128

uint32_t irBuffer[ANALYSIS_BUFFER_LENGTH]; // infrared LED sensor data
uint32_t redBuffer[ANALYSIS_BUFFER_LENGTH]; // red LED sensor data

int32_t bufferLength; // data length
int32_t spo2; // SPO2 value
int8_t validSPO2; // indicator to show if the SPO2 calculation is valid
int32_t heartRate; // heart rate value (still needed for algorithm function)
int8_t validHeartRate; // indicator to show if the heart rate calculation is valid

byte readLED = 13; // Blinks with each data read

// waveform settings
const int WAVE_TOP = 26;
const int WAVE_LEFT = 0;
const int WAVE_WIDTH = 128;
const int WAVE_HEIGHT = 38;

uint32_t irWave[WAVE_BUFFER_LENGTH];
int waveIndex = 0;
bool waveFilled = false;

// BLE service/characteristics
BLEService pulseOxService("19B10000-E8F2-537E-4F6C-D104768A1214");
BLEUnsignedCharCharacteristic spo2Characteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);
BLEUnsignedCharCharacteristic signalCharacteristic("19B10002-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);

void clearWaveBuffer()
{
  for (int i = 0; i < WAVE_BUFFER_LENGTH; i++)
  {
    irWave[i] = 0;
  }
  waveIndex = 0;
  waveFilled = false;
}

void pushWaveSample(uint32_t sample)
{
  irWave[waveIndex] = sample;
  waveIndex = (waveIndex + 1) % WAVE_BUFFER_LENGTH;
  if (waveIndex == 0)
  {
    waveFilled = true;
  }
}

void initializeOLED()
{
  if (!oled.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR))
  {
    Serial.println(F("SSD1306 allocation failed"));
    while (1);
  }

  oled.clearDisplay();
  oled.display();
}

void initializeSensor()
{
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST))
  {
    Serial.println(F("MAX30105 was not found. Please check wiring/power."));
    while (1);
  }

  byte ledBrightness = 55;
  byte sampleAverage = 4;
  byte ledMode = 2;
  byte sampleRate = 100;
  int pulseWidth = 411;
  int adcRange = 4096;

  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);
}

void initializeBLE()
{
  if (!BLE.begin())
  {
    Serial.println(F("BLE initialization failed"));
    while (1);
  }

  BLE.setLocalName("PulseOxNano");
  BLE.setDeviceName("PulseOxNano");
  BLE.setAdvertisedService(pulseOxService);

  pulseOxService.addCharacteristic(spo2Characteristic);
  pulseOxService.addCharacteristic(signalCharacteristic);
  BLE.addService(pulseOxService);

  spo2Characteristic.writeValue((byte)0);
  signalCharacteristic.writeValue((byte)0);

  BLE.advertise();
  Serial.println(F("BLE advertising started"));
}

void showStartupScreen()
{
  Serial.println(F("Attach sensor to finger with rubber band. Press any key to start conversion"));

  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
  oled.setCursor(0, 0);
  oled.println("Place finger on");
  oled.println("sensor");
  oled.display();

  while (Serial.available() == 0)
    ;
  Serial.read();
}

void readInitialSamples()
{
  bufferLength = ANALYSIS_BUFFER_LENGTH;

  for (byte i = 0; i < bufferLength; i++)
  {
    while (particleSensor.available() == false)
    {
      particleSensor.check();
      BLE.poll();
    }

    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    pushWaveSample(irBuffer[i]);
    particleSensor.nextSample();
    BLE.poll();

    Serial.print(F("red="));
    Serial.print(redBuffer[i], DEC);
    Serial.print(F(", ir="));
    Serial.println(irBuffer[i], DEC);
  }
}

void calculateVitals()
{
  maxim_heart_rate_and_oxygen_saturation(
    irBuffer,
    bufferLength,
    redBuffer,
    &spo2,
    &validSPO2,
    &heartRate,
    &validHeartRate
  );
}

void updateBleTelemetry()
{
  byte spo2Value = 0;
  byte signalValue = 0;

  if (validSPO2 == 1 && spo2 >= 90 && spo2 <= 100)
  {
    spo2Value = (byte)spo2;
    signalValue = 1;
  }

  spo2Characteristic.writeValue(spo2Value);
  signalCharacteristic.writeValue(signalValue);
}

void shiftBuffersLeft(byte shiftCount)
{
  for (byte i = shiftCount; i < ANALYSIS_BUFFER_LENGTH; i++)
  {
    redBuffer[i - shiftCount] = redBuffer[i];
    irBuffer[i - shiftCount] = irBuffer[i];
  }
}

void readNewSamples(byte startIndex, byte endIndex)
{
  for (byte i = startIndex; i < endIndex; i++)
  {
    while (particleSensor.available() == false)
    {
      particleSensor.check();
      BLE.poll();
    }

    digitalWrite(readLED, !digitalRead(readLED));

    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    pushWaveSample(irBuffer[i]);
    particleSensor.nextSample();
    BLE.poll();

    Serial.print(F("red="));
    Serial.print(redBuffer[i], DEC);
    Serial.print(F(", ir="));
    Serial.print(irBuffer[i], DEC);
    Serial.print(F(", SPO2Valid="));
    Serial.println(validSPO2, DEC);
  }
}

void drawWaveform()
{
  int sampleCount = waveFilled ? WAVE_BUFFER_LENGTH : waveIndex;
  if (sampleCount < 2)
  {
    return;
  }

  uint32_t minVal = irWave[0];
  uint32_t maxVal = irWave[0];

  for (int i = 1; i < sampleCount; i++)
  {
    if (irWave[i] < minVal) minVal = irWave[i];
    if (irWave[i] > maxVal) maxVal = irWave[i];
  }

  if (maxVal <= minVal)
  {
    maxVal = minVal + 1;
  }

  int pointsToDraw = min(sampleCount, WAVE_WIDTH);
  int startIndex = waveFilled ? waveIndex : 0;

  for (int x = 1; x < pointsToDraw; x++)
  {
    int idx1 = (startIndex + x - 1) % WAVE_BUFFER_LENGTH;
    int idx2 = (startIndex + x) % WAVE_BUFFER_LENGTH;

    int y1 = map((long)irWave[idx1], (long)minVal, (long)maxVal, WAVE_TOP + WAVE_HEIGHT - 1, WAVE_TOP);
    int y2 = map((long)irWave[idx2], (long)minVal, (long)maxVal, WAVE_TOP + WAVE_HEIGHT - 1, WAVE_TOP);

    int x1 = WAVE_LEFT + x - 1;
    int x2 = WAVE_LEFT + x;

    oled.drawLine(x1, y1, x2, y2, SSD1306_WHITE);
  }

  oled.drawRect(WAVE_LEFT, WAVE_TOP, WAVE_WIDTH, WAVE_HEIGHT, SSD1306_WHITE);
}

void renderScreen()
{
  oled.clearDisplay();
  oled.setTextColor(SSD1306_WHITE);

  if (validSPO2 == 1 && spo2 >= 90 && spo2 <= 100)
  {
    oled.setTextSize(2);
    oled.setCursor(0, 0);
    oled.print("SPO2: ");
    oled.print(spo2, DEC);
    oled.println("%");

    oled.setTextSize(1);
    oled.setCursor(0, 18);
    oled.print("Signal: ");
    oled.println("OK");
  }
  else
  {
    oled.setTextSize(1);
    oled.setCursor(0, 0);
    oled.println("Calibrating...");
    oled.println("Looking for finger");
  }

  drawWaveform();
  oled.display();
}

void setup()
{
  Serial.begin(115200);
  Wire.begin();

  initializeOLED();

  pinMode(readLED, OUTPUT);

  initializeSensor();
  initializeBLE();
  showStartupScreen();
  clearWaveBuffer();
  readInitialSamples();
  calculateVitals();
  updateBleTelemetry();
  renderScreen();
}

void loop()
{
  BLE.poll();
  shiftBuffersLeft(25);
  readNewSamples(75, ANALYSIS_BUFFER_LENGTH);
  calculateVitals();
  updateBleTelemetry();
  renderScreen();
  BLE.poll();
}