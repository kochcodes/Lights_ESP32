#ifndef __ESP_NOW_MANAGER__
#define __ESP_NOW_MANAGER__

#include <WiFi.h>
#include <esp_now.h>
#include "./meshConfig.h"

// forward declarations
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len);
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t sendStatus);
class EspNowManager;

typedef struct struct_message
{
    int mode;
} struct_message;

EspNowManager *_delegate;

class EspNowManager
{
private:
    static const uint8_t ipLength = IP_LENGTH;
    uint8_t mac[ipLength];
    bool connected = false;

    int mode = 0;
    std::function<void(int)> onModeChangeCB;

public:
    EspNowManager()
    {
        _delegate = this;
    }
    void OnDataRecvCB(const uint8_t *mac, const uint8_t *incomingData, int len);
    void OnDataSentCB(const uint8_t *mac_addr, esp_now_send_status_t sendStatus);
    void init();
    void setConnectionChangeCB(std::function<void(int)> cb)
    {
        this->onModeChangeCB = cb;
    }
};

void EspNowManager::init()
{
    WiFi.macAddress(mac);
    WiFi.mode(WIFI_STA);
    // Init ESP-NOW
    if (esp_now_init() != 0)
    {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    esp_now_register_recv_cb(OnDataRecv);
    esp_now_register_send_cb(OnDataSent);
}

void EspNowManager::OnDataRecvCB(const uint8_t *mac, const uint8_t *incomingData, int len)
{
    Serial.println("Data received");
    this->connected = true;

    // last_msg_from_master = millis();
    struct_message data;
    memcpy(&data, incomingData, sizeof(data));
    Serial.print("Bytes received: ");
    Serial.println(len);
    Serial.print("Int: ");
    Serial.println(data.mode);

    if (this->mode != data.mode)
    {
        this->mode = data.mode;
        this->onModeChangeCB(this->mode);
    }

    /*
    if (blinkRoutineIndex != data.mode)
    {
        blinkRoutineIndex = data.mode;
        save_mode_to_eeprom(blinkRoutineIndex);
        leds.setRoutine(blinkRoutines[blinkRoutineIndex], last_msg_from_master);
    }
    else
    {
        leds.synchronize(last_msg_from_master);
    }
    if (role == ESP_NOW_ROLE_CONTROLLER)
    {

        Serial.println("Master found. Becoming slave!");

        role = ESP_NOW_ROLE_SLAVE;
        esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
        setupPeers(ESP_NOW_ROLE_CONTROLLER);
    }
    */
}

void EspNowManager::OnDataSentCB(const uint8_t *mac_addr, esp_now_send_status_t sendStatus)
{
    Serial.print("Last Packet Send Status: ");
    /*
    if (this->sendStatus == 0)
        Serial.println("Delivery success");
    else
        Serial.println("Delivery fail");
    */
}

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
    _delegate->OnDataRecvCB(mac, incomingData, len);
}
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t sendStatus)
{
    _delegate->OnDataSentCB(mac_addr, sendStatus);
}

#endif