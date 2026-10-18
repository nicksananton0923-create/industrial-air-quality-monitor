
/*
  Project: Smart Industrial Air Quality Monitoring System
  Board: Arduino Nano

  Example hardware:
  - Plantower PMS5003-compatible particulate sensor
  - DHT11 temperature and humidity sensor
  - 16x2 I2C LCD
  - Red and green LEDs

  IMPORTANT:
  Confirm the sensor model and supply voltage before wiring.
  This example expects the PMS5003 32-byte UART protocol.
*/

#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// ---------- Pin configuration ----------
const byte DUST_RX_PIN = 2;  // Arduino RX <- dust sensor TX
const byte DUST_TX_PIN = 3;  // Arduino TX -> dust sensor RX
const byte RED_LED_PIN = 5;
const byte GREEN_LED_PIN = 6;
const byte DHT_PIN = 7;

// ---------- Sensor configuration ----------
#define DHT_TYPE DHT11

// Change this address if your LCD uses another I2C address.
LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial dustSerial(DUST_RX_PIN, DUST_TX_PIN);
DHT dht(DHT_PIN, DHT_TYPE);

// Configurable demonstration threshold in micrograms per cubic metre.
// Choose a suitable threshold for your application.
const uint16_t PM25_WARNING_LEVEL = 35;

// Update interval: 5 seconds
const unsigned long UPDATE_INTERVAL_MS = 5000;

unsigned long lastUpdate = 0;

uint16_t pm1 = 0;
uint16_t pm25 = 0;
uint16_t pm10 = 0;

bool dustReadingAvailable = false;

// ---------- Read a PMS5003 data frame ----------
bool readDustSensor() {
  while (dustSerial.available() >= 32) {
    // Find the first frame header: 0x42 0x4D
    if ((uint8_t)dustSerial.peek() != 0x42) {
      dustSerial.read();
      continue;
    }

    uint8_t frame[32];

    if (dustSerial.read() != 0x42) {
      continue;
    }

    if (!dustSerial.available()) {
      return false;
    }

    if ((uint8_t)dustSerial.read() != 0x4D) {
      continue;
    }

    frame[0] = 0x42;
    frame[1] = 0x4D;

    // Read the remaining 30 bytes.
    size_t count = dustSerial.readBytes(&frame[2], 30);

    if (count != 30) {
      return false;
    }

    // PMS5003 frame length should normally be 28.
    uint16_t frameLength =
      ((uint16_t)frame[2] << 8) | frame[3];

    if (frameLength != 28) {
      continue;
    }

    // Verify frame checksum.
    uint16_t checksum = 0;

    for (byte i = 0; i < 30; i++) {
      checksum += frame[i];
    }

    uint16_t receivedChecksum =
      ((uint16_t)frame[30] << 8) | frame[31];

    if (checksum != receivedChecksum) {
      continue;
    }

    // Atmospheric environment readings:
    // PM1.0: bytes 10-11
    // PM2.5: bytes 12-13
    // PM10 : bytes 14-15
    pm1 = ((uint16_t)frame[10] << 8) | frame[11];
    pm25 = ((uint16_t)frame[12] << 8) | frame[13];
    pm10 = ((uint16_t)frame[14] << 8) | frame[15];

    return true;
  }

  return false;
}

// ---------- Set LED status ----------
void updateStatusLEDs(bool validReading) {
  if (!validReading) {
    // No valid dust reading: both LEDs off.
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, LOW);
    return;
  }

  if (pm25 >= PM25_WARNING_LEVEL) {
    digitalWrite(RED_LED_PIN, HIGH);
    digitalWrite(GREEN_LED_PIN, LOW);
  } else {
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, HIGH);
  }
}

// ---------- Display readings ----------
void displayReadings(float temperature, float humidity,
                     bool validReading) {
  lcd.clear();

  lcd.setCursor(0, 0);

  if (validReading) {
    lcd.print("PM2.5:");
    lcd.print(pm25);
    lcd.print(" ug/m3");
  } else {
    lcd.print("PM2.5: No data");
  }

  lcd.setCursor(0, 1);

  if (isnan(temperature) || isnan(humidity)) {
    lcd.print("DHT11: No data");
  } else {
    lcd.print(temperature, 1);
    lcd.print("C ");
    lcd.print(humidity, 0);
    lcd.print("% RH");
  }
}

// ---------- Setup ----------
void setup() {
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);

  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);

  Serial.begin(9600);
  dustSerial.begin(9600);

  // Helps prevent a long wait if the sensor sends an incomplete frame.
  dustSerial.setTimeout(100);

  dht.begin();
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Air Quality");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");

  Serial.println("Air Quality Monitor");
  Serial.println("Waiting for sensor readings...");

  delay(1500);
}

// ---------- Main loop ----------
void loop() {
  // Read all available sensor frames.
  while (readDustSensor()) {
    dustReadingAvailable = true;
  }

  // Refresh display and report every 5 seconds.
  if (millis() - lastUpdate >= UPDATE_INTERVAL_MS) {
    lastUpdate = millis();

    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    updateStatusLEDs(dustReadingAvailable);
    displayReadings(temperature, humidity,
                    dustReadingAvailable);

    Serial.println("------------------------------");

    if (dustReadingAvailable) {
      Serial.print("PM1.0: ");
      Serial.print(pm1);
      Serial.println(" ug/m3");

      Serial.print("PM2.5: ");
      Serial.print(pm25);
      Serial.println(" ug/m3");

      Serial.print("PM10: ");
      Serial.print(pm10);
      Serial.println(" ug/m3");

      Serial.print("Status: ");
      Serial.println(
        pm25 >= PM25_WARNING_LEVEL ? "WARNING" : "BELOW SET THRESHOLD"
      );
    } else {
      Serial.println("Dust sensor: No valid reading");
    }

    if (!isnan(temperature) && !isnan(humidity)) {
      Serial.print("Temperature: ");
      Serial.print(temperature);
      Serial.println(" C");

      Serial.print("Humidity: ");
      Serial.print(humidity);
      Serial.println(" %");
    } else {
      Serial.println("DHT11: No valid reading");
    }
  }
}
