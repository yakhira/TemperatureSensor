#define BLYNK_TEMPLATE_ID       "TMPL4nRADNRad"
#define BLYNK_TEMPLATE_NAME     "Temperature Sensor"
#define BOARD_BUTTON_PIN        5
#define BLYNK_FIRMWARE_VERSION  "0.1.0"
#define BLYNK_PRINT             Serial
#define APP_DEBUG
#define BLYNK_DEBUG

#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Wire.h>
#include "BlynkEdgent.h"

#define BMP280_ADDRESS 0x76
#define BMP280_POWER_PIN 14
#define BMP280_SDA_PIN 12
#define BMP280_SCL_PIN 13

bool dataSent = false;

void main_code()
{
    Serial.println("Reading BMP280 sensor data...");
    Adafruit_BMP280 bmp;

    digitalWrite(BMP280_POWER_PIN, HIGH);

    delay(3000);
    Wire.begin(BMP280_SDA_PIN, BMP280_SCL_PIN);

    if (bmp.begin(BMP280_ADDRESS)) {
		bmp.setSampling(Adafruit_BMP280::MODE_NORMAL, /* Operating Mode. */
					Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
					Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
					Adafruit_BMP280::FILTER_X16,      /* Filtering. */
					Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

        float temperature = bmp.readTemperature();
        float pressure = bmp.readPressure() / 100;

        delay(1000);
        if (!isnan(temperature) && !isnan(pressure))
        {
            Serial.println("Sending BMP280 sensor data to Blynk...");
            Serial.print("Temperature = "); Serial.print(temperature);
            Serial.println(" *C");
            Serial.print("Pressure = "); Serial.print(pressure);
            Serial.println(" hPa");

            Blynk.virtualWrite(V0, temperature);
            Blynk.virtualWrite(V1, pressure);

            Serial.println("Sent BMP280 sensor data...");
        }
        delay(1000);
    }
    else {
        Serial.println("Could not find a valid BMP280 sensor, check wiring, address, sensor ID!");
    }

    digitalWrite(BMP280_POWER_PIN, LOW);
    ESP.deepSleepInstant(3600e6);
}

void setup()
{
    Serial.begin(74880);
    delay(100);

    pinMode(BMP280_POWER_PIN, OUTPUT);
    BlynkEdgent.begin();
}

void loop() {
  BlynkEdgent.run();
  if (Blynk.connected() && !dataSent) {
    dataSent = true;
    unsigned long otaWindow = millis();
    while (millis() - otaWindow < 30000) {
      BlynkEdgent.run();
      delay(10);
    }
    main_code();
  } 
}
