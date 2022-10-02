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
  state = new State();

  Serial.begin(115200);
#ifdef ESP32
  bleManager = new BLEManager(state);
  bleManager->init();
#endif

  delay(500);
  espNowManager = new EspNowManager(state);
  espNowManager->init();
  espNowManager->setOnStateChangeCallback(stateChangeCallback);

  blinkRoutineManager = new BlinkRoutineManager();

  leds = new LED();
  timerDelay = leds->setRoutine(blinkRoutineManager->get(blinkRoutineIndex), 0);

  delay(500);
  Serial.println("Lights...");
}

void loop()
{
  long t = millis();
  if ((t - lastTime) >= timerDelay)
  {
    lastTime = t;
    leds->setRoutine(blinkRoutineManager->get(blinkRoutineIndex), state->sync);
  }
#ifdef ESP32
  bleManager->loop();
#endif
  espNowManager->loop();
  leds->loop();
}