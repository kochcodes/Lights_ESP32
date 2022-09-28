#include <Arduino.h>
#ifdef ESP32
#include "./BLEManager.h"
#endif
#include "./EspNowManager.h"
#include "./State.h"

EspNowManager *espNowManager;
#ifdef ESP32
BLEManager *bleManager;
#endif

long t = 0;
int level = 0;
int _mode = 0;

State *state;

void esp_now_mode_update(int mode)
{
  Serial.print("Mode ");
  Serial.println(mode);
  _mode = mode;
#ifdef ESP32
  bleManager->updateMode(_mode);
  bleManager->notify();
#endif
}

void setup()
{
  state = new State();

  Serial.begin(115200);
#ifdef ESP32
  bleManager = new BLEManager(state);
  bleManager->init();
#endif

  espNowManager = new EspNowManager(state);
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

#ifdef ESP32
    bleManager->updateBatteryLevel(level);
    bleManager->notify();
#endif
    if (state->hasUpdate())
    {
      state->reset();
      espNowManager->send();
    }
    espNowManager->send();
  }
}