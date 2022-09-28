#ifndef __ESP_NOW_MANAGER__
#define __ESP_NOW_MANAGER__

#ifdef ESP32
#include <esp_now.h>
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include "./meshConfig.h"
#include "./State.h"

// forward declarations
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len);
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t sendStatus);
class EspNowManager;
EspNowManager *_delegate;

class EspNowManager
{
private:
    State *state;
    static const uint8_t ipLength = IP_LENGTH;
    uint8_t mac[ipLength];
    bool connected = false;

    std::function<void(int)> onModeChangeCB;

public:
    EspNowManager(State *state)
    {
        this->state = state;
        _delegate = this;
    }
    void OnDataRecvCB(const uint8_t *mac, const uint8_t *incomingData, int len);
    void OnDataSentCB(const uint8_t *mac_addr, esp_now_send_status_t sendStatus);
    void init();
    void send();
    void updatePeers();
    void setConnectionChangeCB(std::function<void(int)> cb)
    {
        this->onModeChangeCB = cb;
    }
};

void EspNowManager::init()
{
    WiFi.macAddress(mac);
    WiFi.mode(WIFI_AP);

    if (esp_now_init() != ESP_OK)
    {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    esp_now_register_recv_cb(OnDataRecv);
    esp_now_register_send_cb(OnDataSent);

    esp_now_peer_info_t peerInfo = {};
    peerInfo.ifidx = WIFI_IF_AP;
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    for (unsigned int i = 0; i < sizeof(network_members) / ipLength; i++)
    {
        memcpy(peerInfo.peer_addr, network_members[i], 6);
        if (esp_now_add_peer(&peerInfo) != ESP_OK)
        {
            Serial.println("Failed to add peer");
        }
    }
}

void EspNowManager::send()
{
    esp_now_message data = this->state->pack();
    for (unsigned int i = 0; i < sizeof(network_members) / ipLength; i++)
    {
        esp_err_t result = esp_now_send(network_members[i], (uint8_t *)&data, sizeof(data));
        if (result == ESP_OK)
        {
            Serial.println("Sent with success");
        }
        else
        {
            Serial.println("Error sending the data");
        }
    }
}

void EspNowManager::OnDataRecvCB(const uint8_t *mac, const uint8_t *incomingData, int len)
{
    Serial.println("Data received");
    this->connected = true;

    // last_msg_from_master = millis();
    esp_now_message data;
    memcpy(&data, incomingData, sizeof(esp_now_message));
    Serial.print("Bytes received: ");
    Serial.println(len);
    Serial.print("Int: ");
    Serial.println(data.mode);

    this->onModeChangeCB(data.mode);

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

    if (sendStatus == ESP_NOW_SEND_SUCCESS)
        Serial.println("Delivery success");
    else
        Serial.println("Delivery fail");
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