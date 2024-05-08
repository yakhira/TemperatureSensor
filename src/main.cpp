#include "setup/wifi.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Wire.h>

#define BMP280_ADDRESS 0x76
#define BMP280_POWER_PIN 14
#define BMP280_SDA_PIN 12
#define BMP280_SCL_PIN 13

// -------- DEFAULT SKETCH PARAMETERS --------
const int SKETCH_VERSION = 11; 

ESPWiFi espwifi("ESP12-F");

void main_code(){
	JSONVar data;
	Adafruit_BMP280 bmp;

	pinMode(BMP280_POWER_PIN, OUTPUT);
	digitalWrite(BMP280_POWER_PIN, HIGH);

	delay(3000);

	espwifi.readFile("bmp.txt", data);

	Wire.begin(BMP280_SDA_PIN, BMP280_SCL_PIN);

    if (bmp.begin(BMP280_ADDRESS)) {
		bmp.setSampling(Adafruit_BMP280::MODE_NORMAL, /* Operating Mode. */
					Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
					Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
					Adafruit_BMP280::FILTER_X16,      /* Filtering. */
					Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

		String temperature = String(bmp.readTemperature(), 1);
		String pressure =  String(bmp.readPressure() / 100, 1);

		temperature.replace(".", ",");
		pressure.replace(".", ",");

		if ((String)temperature != data["temperature"] || (String)pressure != data["pressure"]){
			String dataUrl = espwifi.dataUrl + "/esp/temperature?mac=" + WiFi.macAddress();

			if (espwifi.sendHTTPJsonData(dataUrl, data)) {
				data["temperature"] = temperature; 
				data["pressure"] = pressure;
				espwifi.saveFile("bmp.txt", data);
			} else {
				Serial.println("Could not send data to " + dataUrl + "!");
			}
		}
	} else {
		Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
	}

	digitalWrite(BMP280_POWER_PIN, LOW);
	ESP.deepSleepInstant(3600e6);
}

void setup() {
	Serial.begin(74880);
	espwifi.wifiConnect();

	if (WiFi.getMode() == WIFI_STA){
		espwifi.updateSketch(SKETCH_VERSION);
		main_code();
	}
}

void loop() {
	if (WiFi.getMode() == WIFI_STA) {
		if (WiFi.status() != WL_CONNECTED) {
			WiFi.reconnect();
		}
	}

	espwifi.stateCheck();
}