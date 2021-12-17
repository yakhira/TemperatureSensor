#ifndef UTILS_H_
#define UTILS_H_

#include <Arduino.h>
#include <Arduino_JSON.h>

#if defined(ESP32)
#include <LITTLEFS.h>
#include "ESP32httpUpdate.h"
#define LittleFS LITTLEFS
#else
#include <ESP8266HTTPClient.h>
#include "ESP8266httpUpdate.h"
#include <LittleFS.h>
#endif

class ESPUtils {
    public:
        void saveFile(String filename, JSONVar data);
        void saveFile(String filename, String data);
        void readFile(String filename, JSONVar &data);
        void readFile(String filename, String &data);
        void removeFile(String filename);

        bool getHTTPJsonData(String url, JSONVar &result);
        bool sendHTTPJsonData(String url, JSONVar data);

        bool fsMounted = false;

        void mountFS();

        t_httpUpdate_return updateSketch(String url);

        WiFiClient client;
        ESPUtils();
        ~ESPUtils();
};

#endif