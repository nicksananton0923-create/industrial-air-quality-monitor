 Industrial Air-Quality Monitoring and Alert System

An educational Arduino Nano project designed to read particulate-matter data, monitor temperature and relative humidity, display readings, and provide a configurable PM2.5 status indication.

> **Project status:** The repository contains source code and supporting documentation. Physical hardware verification and measured performance have not been established here. This project is not a certified industrial or occupational air-quality instrument.

 📌 Project Overview

The firmware is written for an Arduino Nano and expects a PMS5003-compatible particulate sensor using a 32-byte UART data frame. A DHT11 provides temperature and relative-humidity readings. An I²C LCD displays information, while red and green LEDs indicate the PM2.5 threshold state when a valid particulate reading is available.

The exact sensor model, wiring, supply requirements, and library compatibility must be checked against the components actually used.

 ✨ Intended Features

- Read PM1.0, PM2.5, and PM10 values from a compatible particulate sensor.
- Check the expected frame length and checksum.
- Read temperature and relative humidity from a DHT11.
- Display readings on a 16×2 I²C LCD.
- Indicate the configured PM2.5 threshold state using LEDs.
- Report readings and status messages through the Serial Monitor.
- Refresh the displayed readings at a nominal five-second interval.

These are features described by the source code, not a claim that every feature has passed physical testing.

 🧰 Proposed Hardware

| Component | Purpose |
|---|---|
| Arduino Nano | Main controller |
| PMS5003-compatible particulate sensor | PM1.0, PM2.5, and PM10 readings |
| DHT11 sensor | Temperature and relative humidity |
| 16×2 I²C LCD | Local display |
| Red and green LEDs with suitable resistors | Status indication |
| Suitable regulated power supply | Power for the board and peripherals |
| Compatible wiring and connectors | Circuit connections |

Confirm the exact particulate-sensor model, its current requirements, and its interface-voltage requirements before connecting it.

 🔌 Pin Connections in the Current Sketch

| Device / signal | Arduino Nano pin |
|---|---|
| Particulate sensor TX → Arduino software RX | D2 |
| Arduino software TX → particulate sensor RX | D3 |
| Red status LED (through resistor) | D5 |
| Green status LED (through resistor) | D6 |
| DHT11 data | D7 |
| I²C LCD SDA | A4 |
| I²C LCD SCL | A5 |

The code configures the LCD address as `0x27`. Change it only after confirming the address of the actual LCD module.

 💻 Software and Libraries

The sketch is located at [`code/Air_quality_monitor.ino`](code/Air_quality_monitor.ino).

The source includes:

- `SoftwareSerial`
- `Wire`
- `LiquidCrystal_I2C`
- `DHT`

Install compatible versions of the required third-party libraries through the Arduino IDE Library Manager. The LCD library and constructor must match the library version selected.

 ⚙️ PM2.5 Warning Threshold

The current sketch uses:

`PM25_WARNING_LEVEL = 35`

The value is a configurable demonstration threshold expressed in micrograms per cubic metre (µg/m³). It is not a universal safety limit or a statement of regulatory compliance. Select a threshold for the intended application using appropriate authoritative guidance and a validated measurement method.

 📂 Existing Project Documentation

- [Component list and notes](<Component>)
- [Connection terminals](<Connections terminals>)
- [Library notes](<Manage Libraries.>)
- [Diagram image](<Diagram.jpg>)

These links retain the existing repository paths.

 🧪 Testing and Verification

Before describing the project as operational, record evidence for:

1. Correct sensor model and supply requirements.
2. UART frame reception, frame length, and checksum validation.
3. Comparison of readings against an appropriate reference instrument.
4. DHT11 readings and behaviour when the sensor is disconnected.
5. LCD output and LED threshold behaviour.
6. Behaviour after sensor disconnection or loss of valid data.
7. Stability of the power supply and wiring.

The current source retains the last valid particulate reading and does not expire it after a prolonged loss of sensor data. Treat this as a code-review item before relying on displayed readings.

 ⚠️ Safety and Limitations

- This is an educational project, not a certified industrial air-quality monitor.
- Do not use it as the sole basis for workplace safety decisions.
- Sensor readings can be affected by sensor model, calibration, placement, environmental conditions, and electrical noise.
- Check supply voltage, current requirements, grounding, and logic compatibility before wiring.
- A displayed value does not establish measurement accuracy.

 📈 Future Improvements

- Add a sensor-data timeout and an explicit stale-data or sensor-fault state.
- Record measurements with timestamps for later analysis.
- Validate readings against a suitable reference instrument.
- Document the exact component models, wiring, and test evidence.
- Add a clear test-results table that distinguishes measured results from expected behaviour.

 📄 Project Status

**Code and documentation are available; physical verification and measured performance remain unconfirmed.**
