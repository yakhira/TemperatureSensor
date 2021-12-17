#include "utils.h"

ESPUtils::ESPUtils(){
}

ESPUtils::~ESPUtils(){

}

void ESPUtils::mountFS(){
    if (!fsMounted) {
        LittleFS.begin();
    }
}

void ESPUtils::removeFile(String filename) {
	mountFS();
	LittleFS.remove(filename);
}

void ESPUtils::saveFile(String filename, JSONVar data) {
	mountFS();

	File file = LittleFS.open(filename, "w");
	if (file) {
      file.println(data); 
      file.close();
    }
}

void ESPUtils::saveFile(String filename, String data) {
	mountFS();

	File file = LittleFS.open(filename, "w");
	if (file) {
      file.println(data); 
      file.close();
    }
}

void ESPUtils::readFile(String filename, JSONVar &data) {
	mountFS();

	File file = LittleFS.open(filename, "r");
	while (file.available()) {
		data = JSON.parse(file.readStringUntil('\n'));
    }
	file.close();

}

void ESPUtils::readFile(String filename, String &data) {
	mountFS();

	File file = LittleFS.open(filename, "r");
	while (file.available()) {
		data = file.readStringUntil('\n');
    }
	file.close();
}

bool ESPUtils::getHTTPJsonData(String url, JSONVar &result) {
	HTTPClient http;

	http.begin(client, url);
	int httpCode = http.GET();

	if (httpCode == HTTP_CODE_OK) {
		result = JSON.parse(http.getString());

		if (JSON.typeof(result) == "undefined") {
			return false;
		} 
	}
	http.end();
	return (httpCode == HTTP_CODE_OK);
}

bool ESPUtils::sendHTTPJsonData(String url, JSONVar data) {
	HTTPClient http;

	http.begin(client, url);
	http.addHeader("Content-Type", "application/json");

	int httpCode = http.POST(JSON.stringify(data));
	http.end();

	return (httpCode == HTTP_CODE_OK);
}

t_httpUpdate_return ESPUtils::updateSketch(String url){
	#if defined(ESP32)
		return ESPhttpUpdate.update(url);
	#else
  		return ESPhttpUpdate.update(client, url);
	#endif
}