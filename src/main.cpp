#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

byte flags = 0b0000000000010;

bool _BLEClientConnected = false;

uint8_t level = 0;
uint8_t percentage = 0;

#define LIGHT_OUTPUT_UUID BLEUUID((uint16_t)0x2BE2)

#define BATTERY_UUID BLEUUID((uint16_t)0x180F)
BLECharacteristic BatteryLevel(BLEUUID((uint16_t)0x2A19), BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor BatteryLevelDescriptor(BLEUUID((uint16_t)0x2901));

#define PERCENTAGE_UUUID BLEUUID((uint16_t)0x27AD)
BLECharacteristic Percentage(BLEUUID((uint16_t)0x2A6E), BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor PercentageDescriptor(BLEUUID((uint16_t)0x0000));

// #define BINARY_SENSOR BLEUUID((uint16_t)0x183B)
// #define LIMUNOUS_INTENSITY_UUID BLEUUID((uint16_t)0x2707)
// #define LIMUNANCE_UUID BLEUUID((uint16_t)0x2731)

BLEServer *pServer;

class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    _BLEClientConnected = true;
    Serial.println("Connected");
  };

  void onDisconnect(BLEServer *pServer)
  {
    _BLEClientConnected = false;
    pServer->getAdvertising()->start();
    Serial.println("Disconnected");
  }
};

class PercentageCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    uint8_t *value = pCharacteristic->getData();
    Serial.println(*value);
    percentage = *value;
  }
};

void InitBLE()
{
  BLEDevice::init("Lights");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *batteryService = pServer->createService(BATTERY_UUID);
  batteryService->addCharacteristic(&BatteryLevel);
  BatteryLevelDescriptor.setValue("Battery Level");
  BatteryLevel.addDescriptor(&BatteryLevelDescriptor);
  BatteryLevel.addDescriptor(new BLE2902());

  pServer->getAdvertising()->addServiceUUID(BATTERY_UUID);
  batteryService->start();

  BLEService *percentageService = pServer->createService(PERCENTAGE_UUUID);
  percentageService->addCharacteristic(&Percentage);
  PercentageDescriptor.setValue("Light Percentage");
  Percentage.addDescriptor(&PercentageDescriptor);
  Percentage.setCallbacks(new PercentageCallbacks());
  Percentage.addDescriptor(new BLE2902());

  pServer->getAdvertising()->addServiceUUID(BATTERY_UUID);
  percentageService->start();

  pServer->getAdvertising()->start();
}

void setup()
{
  Serial.begin(115200);
  InitBLE();
  pinMode(2, OUTPUT);
  delay(500);
  Serial.println("Lights...");
}

long t = 0;
void loop()
{
  if (_BLEClientConnected)
  {
    digitalWrite(2, HIGH);
  }
  else
  {
    digitalWrite(2, LOW);
  }
  if (millis() > t)
  {
    t = millis() + 1000;

    level += 1;
    if (level > 100)
    {
      level = 0;
    }
    BatteryLevel.setValue(&level, 1);
    BatteryLevel.notify();

    Percentage.setValue(&percentage, 1);
    Percentage.notify();
  }
}