#ifndef __BLE_MANAGER__
#define __BLE_MANAGER__

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <Arduino.h>

class BLEManager
{
private:
    BLEServer *pServer;
    byte flags = 0b0000000000010;

    bool bleClientConnected = false;

    uint8_t batteryLevel = 0;
    uint8_t percentage = 0;
    uint8_t mode = 0;

public:
    void init();
    void connected();
    void disconnected();
    void updateBatteryLevel(uint8_t value);
    void updatePercentage(uint8_t value);
    void updateMode(uint8_t value);
    void notify();
};

class MyServerCallbacks : public BLEServerCallbacks
{
private:
    BLEManager *delegate;

public:
    MyServerCallbacks() {}
    void attachDelegate(BLEManager *delegate)
    {
        this->delegate = delegate;
    }
    void onConnect(BLEServer *pServer)
    {
        delegate->connected();
    };

    void onDisconnect(BLEServer *pServer)
    {
        delegate->disconnected();
        pServer->getAdvertising()->start();
    }
};

class PercentageCallbacks : public BLECharacteristicCallbacks
{
private:
    BLEManager *delegate;

public:
    void attachDelegate(BLEManager *delegate)
    {
        this->delegate = delegate;
    }
    void onWrite(BLECharacteristic *pCharacteristic)
    {
        uint8_t *value = pCharacteristic->getData();
        Serial.println(*value);
        this->delegate->updatePercentage(*value);
    }
};

class ModeCallbacks : public BLECharacteristicCallbacks
{
private:
    BLEManager *delegate;

public:
    void attachDelegate(BLEManager *delegate)
    {
        this->delegate = delegate;
    }
    void onWrite(BLECharacteristic *pCharacteristic)
    {
        uint8_t *value = pCharacteristic->getData();
        Serial.println(*value);
        this->delegate->updateMode(*value);
    }
};

#endif