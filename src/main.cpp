#include "setup/wifi.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Wire.h>

#define BMP280_ADDRESS 0x76

// -------- DEFAULT SKETCH PARAMETERS --------
const int SKETCH_VERSION = 5; 

ESPWiFi espwifi("ESP12-F");

void main_code(){
	JSONVar data;
	Adafruit_BMP280 bmp;

	pinMode(14, OUTPUT);
	digitalWrite(14, HIGH);

	espwifi.readFile("bmp.txt", data);

	Wire.begin(12, 13);

    if (bmp.begin(BMP280_ADDRESS)) {
		bmp.setSampling(Adafruit_BMP280::MODE_NORMAL, /* Operating Mode. */
					Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
					Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
					Adafruit_BMP280::FILTER_X16,      /* Filtering. */
					Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

		float temperature = bmp.readTemperature();
		float pressure = bmp.readPressure() / 100;

		if ((String)temperature != data["temperature"] || (String)pressure != data["pressure"]){
			espwifi.sendHTTPJsonData(
				espwifi.dataUrl + "/esp/temperature?mac=" + WiFi.macAddress(),
				data
			);

			data["temperature"] = temperature; 
			data["pressure"] = pressure;
			espwifi.saveFile("bmp.txt", data);
		}
	} else {
		Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
	}

	digitalWrite(14, LOW);
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