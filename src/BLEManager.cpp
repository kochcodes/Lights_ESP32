#include "./BLEManager.h"

#define BATTERY_UUID BLEUUID((uint16_t)0x180F)
BLECharacteristic BatteryLevelCh(BLEUUID((uint16_t)0x2A19), BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor BatteryLevelDescriptor(BLEUUID((uint16_t)0x2901));

#define PERCENTAGE_UUUID BLEUUID((uint16_t)0x27AD)
BLECharacteristic PercentageCh(BLEUUID((uint16_t)0x2A6E), BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor PercentageDescriptor(BLEUUID((uint16_t)0x0000));

#define MODE_UUUID BLEUUID((uint16_t)0x27AE)
BLECharacteristic ModeCh(BLEUUID((uint16_t)0x2A6F), BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor ModeDescriptor(BLEUUID((uint16_t)0x0000));

void BLEManager::init()
{
    BLEDevice::init("Lights");
    pServer = BLEDevice::createServer();
    MyServerCallbacks *serverCB = new MyServerCallbacks();
    serverCB->attachDelegate(this);
    pServer->setCallbacks(serverCB);

    BLEService *batteryService = pServer->createService(BATTERY_UUID);
    batteryService->addCharacteristic(&BatteryLevelCh);
    BatteryLevelDescriptor.setValue("Battery Level");
    BatteryLevelCh.addDescriptor(&BatteryLevelDescriptor);
    BatteryLevelCh.addDescriptor(new BLE2902());

    pServer->getAdvertising()->addServiceUUID(BATTERY_UUID);
    batteryService->start();

    BLEService *percentageService = pServer->createService(PERCENTAGE_UUUID);
    percentageService->addCharacteristic(&PercentageCh);
    PercentageDescriptor.setValue("Light Percentage");
    PercentageCh.addDescriptor(&PercentageDescriptor);
    PercentageCallbacks *percentageCB = new PercentageCallbacks();
    percentageCB->attachDelegate(this);
    PercentageCh.setCallbacks(percentageCB);
    PercentageCh.addDescriptor(new BLE2902());

    pServer->getAdvertising()->addServiceUUID(PERCENTAGE_UUUID);
    percentageService->start();

    BLEService *modeService = pServer->createService(MODE_UUUID);
    modeService->addCharacteristic(&ModeCh);
    ModeDescriptor.setValue("Light Percentage");
    ModeCh.addDescriptor(&ModeDescriptor);
    PercentageCallbacks *modeCB = new PercentageCallbacks();
    modeCB->attachDelegate(this);
    ModeCh.setCallbacks(modeCB);
    ModeCh.addDescriptor(new BLE2902());

    pServer->getAdvertising()->addServiceUUID(MODE_UUUID);
    modeService->start();

    pServer->getAdvertising()->start();
}

void BLEManager::connected()
{
    Serial.println("Connected in BLEManager");
    this->bleClientConnected = true;
}

void BLEManager::disconnected()
{
    Serial.println("Disconnected in BLEManager");
    this->bleClientConnected = false;
}

void BLEManager::updateBatteryLevel(uint8_t value)
{
    this->batteryLevel = value;
    BatteryLevelCh.setValue(&this->batteryLevel, 1);
}
void BLEManager::updatePercentage(uint8_t value)
{
    this->percentage = value;
    Serial.println("Update Percentage CB in BLEManager");
    PercentageCh.setValue(&this->percentage, 1);
}

void BLEManager::updateMode(uint8_t value)
{
    this->mode = value;
    Serial.println("Update Mode CB in BLEManager");
    ModeCh.setValue(&this->mode, 1);
}

void BLEManager::notify()
{
    if (this->bleClientConnected == true)
    {
        PercentageCh.notify();
        BatteryLevelCh.notify();
        ModeCh.notify();
    }
}