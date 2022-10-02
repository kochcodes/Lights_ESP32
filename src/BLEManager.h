#ifndef __BLE_MANAGER__
#define __BLE_MANAGER__

#ifdef ESP32

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <Arduino.h>
#include "./State.h"

class BLEManager
{
private:
    State *state;
    BLEServer *pServer;
    byte flags = 0b0000000000010;

    bool bleClientConnected = false;

    uint8_t batteryLevel = 0;
    uint8_t percentage = 0;
    unsigned int last_loop = 0;

public:
    BLEManager(State *state);
    void init();
    void connected();
    void disconnected();
    void updateBatteryLevel(uint8_t value);
    void updatePercentage(uint8_t value);
    void setRoutine(uint8_t routine);
    void updateMode(uint8_t value);
    void updateState();
    void notify();
    void loop();
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

class RoutineCallbacks : public BLECharacteristicCallbacks
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
        this->delegate->setRoutine(*value);
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
        this->delegate->updateMode(*value);
    }
};

#endif

#endif