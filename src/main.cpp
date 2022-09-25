#include <Arduino.h>
#include "./BLEManager.h"
#include "./EspNowManager.h"

EspNowManager *espNowManager;
BLEManager *bleManager;

long t = 0;
int level = 0;
int _mode = 0;

void esp_now_mode_update(int mode)
{
  Serial.print("Mode ");
  Serial.println(mode);
  _mode = mode;
  bleManager->updateMode(_mode);
  bleManager->notify();
}

void setup()
{
  Serial.begin(115200);
  bleManager = new BLEManager();
  bleManager->init();

  espNowManager = new EspNowManager();
  espNowManager->init();
  espNowManager->setConnectionChangeCB(esp_now_mode_update);

  delay(500);
  Serial.println("Lights...");
}

void loop()
{
  if (millis() > t)
  {
    t = millis() + 1000;

    level += 1;
    if (level > 100)
    {
      level = 0;
    }

    bleManager->updateBatteryLevel(level);
    bleManager->notify();
  }
}