#ifdef ESP32

#include "./BLEManager.h"

#define BATTERY_UUID BLEUUID((uint16_t)0x180F)
BLECharacteristic BatteryLevelCh(BLEUUID((uint16_t)0x2A19), BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor BatteryLevelDescriptor(BLEUUID((uint16_t)0x2901));

#define PERCENTAGE_UUUID BLEUUID((uint16_t)0x27AD)
BLECharacteristic RoutineCh(BLEUUID((uint16_t)0x2A6E), BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor RoutineDescriptor(BLEUUID((uint16_t)0x0000));

#define MODE_UUUID BLEUUID((uint16_t)0x27AE)
BLECharacteristic ModeCh(BLEUUID((uint16_t)0x2A6F), BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor ModeDescriptor(BLEUUID((uint16_t)0x0000));

#define STATE_SENT_MESSAGES_UUUID BLEUUID((uint16_t)0x27AF)
BLECharacteristic StateSentMessagesCh(BLEUUID((uint16_t)0x2A70), BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor StateSentMessagesDescriptor(BLEUUID((uint16_t)0x0000));
BLECharacteristic StateReceivedMessagesCh(BLEUUID((uint16_t)0x2A71), BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor StateReceivedMessagesDescriptor(BLEUUID((uint16_t)0x0000));
BLECharacteristic StateDeliveredMessagesCh(BLEUUID((uint16_t)0x2A72), BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor StateDeliveredMessagesDescriptor(BLEUUID((uint16_t)0x0000));

BLEManager::BLEManager(State *state)
{
    this->state = state;
    this->last_loop = 0;
}

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

    BLEService *routineService = pServer->createService(PERCENTAGE_UUUID);
    routineService->addCharacteristic(&RoutineCh);
    RoutineDescriptor.setValue("Light Routine");
    RoutineCh.addDescriptor(&RoutineDescriptor);
    RoutineCallbacks *routineCB = new RoutineCallbacks();
    routineCB->attachDelegate(this);
    RoutineCh.setCallbacks(routineCB);
    RoutineCh.addDescriptor(new BLE2902());

    pServer->getAdvertising()->addServiceUUID(PERCENTAGE_UUUID);
    routineService->start();

    BLEService *modeService = pServer->createService(MODE_UUUID);
    modeService->addCharacteristic(&ModeCh);
    ModeDescriptor.setValue("Controller Mode");
    ModeCh.addDescriptor(&ModeDescriptor);
    ModeCallbacks *modeCB = new ModeCallbacks();
    modeCB->attachDelegate(this);
    ModeCh.setCallbacks(modeCB);
    ModeCh.addDescriptor(new BLE2902());

    pServer->getAdvertising()->addServiceUUID(MODE_UUUID);
    modeService->start();

    BLEService *stateService = pServer->createService(STATE_SENT_MESSAGES_UUUID);
    stateService->addCharacteristic(&StateSentMessagesCh);
    StateSentMessagesDescriptor.setValue("Sent Messages");
    StateSentMessagesCh.addDescriptor(&StateSentMessagesDescriptor);
    StateSentMessagesCh.addDescriptor(new BLE2902());

    stateService->addCharacteristic(&StateDeliveredMessagesCh);
    StateDeliveredMessagesDescriptor.setValue("Delivered Messages");
    StateDeliveredMessagesCh.addDescriptor(&StateDeliveredMessagesDescriptor);
    StateDeliveredMessagesCh.addDescriptor(new BLE2902());

    stateService->addCharacteristic(&StateReceivedMessagesCh);
    StateReceivedMessagesDescriptor.setValue("Received Messages");
    StateReceivedMessagesCh.addDescriptor(&StateReceivedMessagesDescriptor);
    StateReceivedMessagesCh.addDescriptor(new BLE2902());

    pServer->getAdvertising()->addServiceUUID(STATE_SENT_MESSAGES_UUUID);
    stateService->start();

    pServer->getAdvertising()->start();
}

void BLEManager::loop()
{
    unsigned long int t = millis();
    if ((t - last_loop) >= 200)
    {
        last_loop = t;
        if (this->bleClientConnected && this->state->hasUpdate())
        {
            this->state->reset();
            this->updateState();
            this->notify();
        }
    }
}

void BLEManager::connected()
{
    Serial.println("Connected in BLEManager");
    this->bleClientConnected = true;
    this->state->update();
}

void BLEManager::disconnected()
{
    Serial.println("Disconnected in BLEManager");
    this->bleClientConnected = false;
}

void BLEManager::updateState()
{
    uint8_t data1 = this->state->sent_messages;
    StateSentMessagesCh.setValue(&data1, 1);
    uint8_t data2 = this->state->delivered_messages;
    StateDeliveredMessagesCh.setValue(&data2, 1);
    uint8_t data3 = this->state->received_messages;
    StateReceivedMessagesCh.setValue(&data3, 1);
    uint8_t mode = this->state->isSlave() ? 1 : 0;
    ModeCh.setValue(&mode, 1);
    uint8_t routine = this->state->blink_routine;
    RoutineCh.setValue(&routine, 1);
}

void BLEManager::updateBatteryLevel(uint8_t value)
{
    this->batteryLevel = value;
    BatteryLevelCh.setValue(&this->batteryLevel, 1);
}
void BLEManager::setRoutine(uint8_t value)
{
    this->state->blink_routine = value;
    Serial.println("Update Percentage CB in BLEManager");
    RoutineCh.setValue(&value, 1);
}

void BLEManager::updateMode(uint8_t value)
{
    Serial.print("BLEManager: Mode: ");
    Serial.println(value > 0 ? "Slave" : "Master");
    this->state->setSlave(value > 0 ? true : false);
    uint8_t mode = this->state->isSlave() ? 1 : 0;
    ModeCh.setValue(&mode, 1);
}

void BLEManager::notify()
{
    if (this->bleClientConnected == true)
    {
        RoutineCh.notify();
        BatteryLevelCh.notify();
        StateSentMessagesCh.notify();
        StateDeliveredMessagesCh.notify();
        StateReceivedMessagesCh.notify();
        ModeCh.notify();
    }
}

#endif