#include <Arduino.h>
#ifdef ESP32
#include "./BLEManager.h"
#endif
#include "./EspNowManager.h"
#include "./State.h"
#include "./BlinkRoutines.h"
#include "./LED.h"

#ifdef ESP32
BLEManager *bleManager;
#endif

EspNowManager *espNowManager;
BlinkRoutineManager *blinkRoutineManager;
LED *leds;

unsigned long t = 0;
int level = 0;
int _mode = 0;

State *state;

void stateChangeCallback()
{
  Serial.println("main:stateChangeCallback()");
}

unsigned long lastTime = 0;
unsigned long timerDelay = 2000;
uint8_t blinkRoutineIndex = 0;

void setup()
{
  Serial.begin(115200);
  delay(500);

  state = new State();

#ifdef ESP32
  bleManager = new BLEManager(state);
  bleManager->init();
#endif

  espNowManager = new EspNowManager(state);
  espNowManager->init();
  espNowManager->setOnStateChangeCallback(stateChangeCallback);

  blinkRoutineManager = new BlinkRoutineManager();

  leds = new LED();
  timerDelay = leds->setRoutine(blinkRoutineManager->get(state->getRoutine()), 0);

  Serial.println("Ready...");
}

void loop()
{
  long t = millis();
  if ((t - lastTime) >= timerDelay)
  {
    lastTime = t;
    timerDelay = leds->setRoutine(blinkRoutineManager->get(state->getRoutine()), state->sync);

    espNowManager->setSlave(state->isSlave());
    if (state->isSlave())
    {
      if (state->last_message_received_at + espNowManager->wait_for_master_time <= t)
      {
        espNowManager->setSlave(false);
      }
    }
    else
    {
      Serial.print("Delay:");
      Serial.println(timerDelay);
      Serial.print("Sync: ");
      Serial.println(t);
      espNowManager->send();
    }
  }
#ifdef ESP32
  bleManager->loop();
#endif
  leds->loop();
}