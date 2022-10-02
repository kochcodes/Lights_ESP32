#ifndef __LED__
#define __LED__

#include <Arduino.h>
#include <BlinkRoutines.h>
#include <Adafruit_NeoPixel.h>

#define NUM_LEDS 29

// Adafruit_NeoPixel strip(NUM_LEDS, DATA_PIN, NEO_GRB + NEO_KHZ400);

enum LED_STATE
{
    LED_STATE_ON,
    LED_STATE_OFF,
    LED_STATE_DOUBLE_BLINK_ROUTINE
};
int col = 0;
class LED
{
public:
    LED()
    {
        pinMode(LED_PIN, OUTPUT);
        // strip.begin();
        // strip.show();
        // strip.setBrightness(50);
        // delay(1000);
    };

    long offset = 0;
    LED_STATE state;
    BlinkRoutine *routine;

    void loop()
    {
        unsigned long int t = millis();
        uint8_t val = this->routine->loop(t - offset);
        analogWrite(LED_PIN, val);
    }

    void synchronize(long t)
    {
        this->offset = t;
    }
    int setRoutine(BlinkRoutine *r, long sync)
    {
        this->routine = r;
        this->synchronize(sync);
        return this->routine->getDuration();
    }
};

#endif