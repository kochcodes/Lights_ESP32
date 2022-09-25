#ifndef __ESP_NOW_MANAGER__
#define __ESP_NOW_MANAGER__

#include <WiFi.h>
#include <esp_now.h>

class EspNowManager
{
private:
    static const uint8_t ipLength = 6;
    uint8_t mac[ipLength];

public:
    EspNowManager() {}

    void init()
    {
        WiFi.macAddress(mac);
        WiFi.mode(WIFI_STA);
        // Init ESP-NOW
        if (esp_now_init() != 0)
        {
            Serial.println("Error initializing ESP-NOW");
            return;
        }
    }
};

#endif