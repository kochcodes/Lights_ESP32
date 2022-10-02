#ifndef __ESP_NOW_MANAGER__
#define __ESP_NOW_MANAGER__

#include "./meshConfig.h"
#include "./State.h"

#ifdef ESP32
#include <esp_now.h>
#include <WiFi.h>
#else
#include <espnow.h>
#include <ESP8266WiFi.h>

typedef int esp_now_send_status_t;
typedef int esp_err_t;
#define ESP_OK 0
#define ESP_NOW_SEND_SUCCESS 0
#endif

// forward declarations
#ifdef ESP32
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len);
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t sendStatus);
#else
void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len);
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus);
#endif

class EspNowManager;
EspNowManager *_delegate;

class EspNowManager
{
private:
    State *state;
    static const uint8_t ipLength = IP_LENGTH;
    uint8_t mac[ipLength];
    bool connected = false;
    unsigned long wait_for_master_time = 10000; // 10s
    unsigned long last_loop = 0;
    bool initialized_mode;

    std::function<void(void)> onStateChangeCallback;

public:
    EspNowManager(State *state)
    {
        this->state = state;
        this->initialized_mode = false;
        _delegate = this;
    }

    void setSlave(bool mode);
    void OnDataRecvCB(const uint8_t *mac, const uint8_t *incomingData, int len);
    void OnDataSentCB(const uint8_t *mac_addr, esp_now_send_status_t sendStatus);
    void init();
    void send();
    void updatePeers();
    void deletePeers();
    void setupPeers();
    void loop();
    void setOnStateChangeCallback(std::function<void(void)> cb)
    {
        this->onStateChangeCallback = cb;
    }
};

void EspNowManager::setSlave(bool mode)
{
    if (this->initialized_mode != mode)
    {
        this->state->setSlave(mode);
        this->initialized_mode = this->state->isSlave();
        if (this->state->isSlave())
        {
            Serial.println("Init slave mode");
#ifndef ESP32
            esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
#endif
            this->deletePeers();
        }
        else
        {
            Serial.println("Init master mode");
#ifndef ESP32
            esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
#endif
            this->setupPeers();
        }
    }
    this->state->update();
}
void EspNowManager::loop()
{
    unsigned long int t = millis();
    if ((t - last_loop) >= 1500)
    {
        this->setSlave(this->state->isSlave());
        last_loop = t;
        if (this->state->isSlave())
        {
            if (this->state->last_message_received_at + wait_for_master_time <= t)
            {
                this->setSlave(false);
            }
        }
        else
        {
            this->send();
        }
    }
}

void EspNowManager::deletePeers()
{
    for (unsigned int i = 0; i < sizeof(network_members) / ipLength; i++)
    {
        esp_now_del_peer(network_members[i]);
    }
}

void EspNowManager::setupPeers()
{
#ifdef ESP32
    esp_now_peer_info_t peerInfo = {};
    // peerInfo.ifidx = WIFI_IF_AP;
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    for (unsigned int i = 0; i < sizeof(network_members) / ipLength; i++)
    {
        memcpy(peerInfo.peer_addr, network_members[i], 6);
        int res = esp_now_add_peer(&peerInfo);
        if (res == ESP_OK)
        {
            Serial.println("Successfulf add peer");
        }
        else if (res == ESP_ERR_ESPNOW_NOT_INIT)
        {
            Serial.println("Failed to add peer, not init");
        }
        else if (res == ESP_ERR_ESPNOW_ARG)
        {
            Serial.println("Failed to add peer, wrong args");
        }
        else if (res == ESP_ERR_ESPNOW_NOT_FOUND)
        {
            Serial.println("Failed to add peer, peer not found");
        }
    }
#else
    esp_now_role peer_role = ESP_NOW_ROLE_SLAVE;
    for (unsigned int i = 0; i < sizeof(network_members) / ipLength; i++)
    {
        if (memcmp(network_members[i], mac, ipLength) != 0)
        {
            esp_now_add_peer(network_members[i], peer_role, 1, NULL, 0);
        }
    }
#endif
}

void EspNowManager::init()
{
#ifdef ESP8266
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
#else
    WiFi.mode(WIFI_MODE_STA);
#endif

    Serial.println(WiFi.macAddress());

    if (esp_now_init() != ESP_OK)
    {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    this->setSlave(true);
    esp_now_register_recv_cb(OnDataRecv);
    esp_now_register_send_cb(OnDataSent);
}

void EspNowManager::send()
{
    esp_now_message data = this->state->pack();
    for (unsigned int i = 0; i < sizeof(network_members) / ipLength; i++)
    {
        esp_err_t result = esp_now_send(network_members[i], (uint8_t *)&data, sizeof(data));
        if (result == 0)
        {
            this->state->sent_messages++;
        }
        this->state->sync = millis();
    }
    this->state->update();
}

void EspNowManager::OnDataRecvCB(const uint8_t *mac, const uint8_t *incomingData, int len)
{

    this->state->sync = millis();
    this->state->last_message_received_at = this->state->sync;
    Serial.println("Data received");
#ifndef ESP32
    if (!state->isSlave())
    {
        this->setSlave(true);
    }
#endif
    this->connected = true;

    esp_now_message data;
    memcpy(&data, incomingData, sizeof(esp_now_message));
    Serial.print("Bytes received: ");
    Serial.println(len);
    Serial.print("Int: ");
    Serial.println(data.mode);

    this->state->received_messages++;

    if (this->state->blink_routine != data.mode)
    {
        this->state->blink_routine = data.mode;
    }
    this->state->update();
}

void EspNowManager::OnDataSentCB(const uint8_t *mac_addr, esp_now_send_status_t sendStatus)
{
    if (sendStatus == ESP_NOW_SEND_SUCCESS)
    {
        this->state->delivered_messages++;
        this->state->update();
    }
    Serial.printf("Success rate: %d / %d\n", this->state->sent_messages, this->state->delivered_messages);
}

#ifdef ESP32
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
    _delegate->OnDataRecvCB(mac, incomingData, len);
}
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t sendStatus)
{
    _delegate->OnDataSentCB(mac_addr, sendStatus);
}
#else
void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len)
{
    Serial.println("OnDataRecv");
    _delegate->OnDataRecvCB(mac, incomingData, len);
}
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus)
{
    _delegate->OnDataSentCB(mac_addr, sendStatus);
}
#endif

#endif