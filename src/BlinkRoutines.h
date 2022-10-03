#ifndef __BLINK_ROUTINES__
#define __BLINK_ROUTINES__

#include <Arduino.h>
#include <vector>

class BlinkRoutine
{

public:
    BlinkRoutine()
    {
        duration = 1000;
    }

    // will be overwritten by child class
    virtual int loop(long t)
    {
        return 0;
    }

    int getDuration()
    {
        return this->duration;
    }

protected:
    long duration;
};

class DoubleBlinkSemiRoutine : public BlinkRoutine
{
private:
    int state_1_on;
    int state_2_off;
    int state_3_on;
    int state_4_off;
    int state_5_semi;

public:
    DoubleBlinkSemiRoutine()
    {
        this->state_1_on = 25;
        this->state_2_off = state_1_on + 100;
        this->state_3_on = state_2_off + 25;
        this->state_4_off = state_3_on + 100;
        this->state_5_semi = state_4_off + 1200;
        this->duration = state_5_semi + 100;
    }

    virtual int loop(long t)
    {
        int v = t % this->duration;
        if (v < state_1_on)
        {
            return 255; // first pulse
        }
        else if (v < state_2_off)
        {
            return 0; // off after first pulse
        }
        else if (v < state_3_on)
        {
            return 255; // second pulse
        }
        else if (v < state_4_off)
        {
            return 0; // off after second pulse
        }
        else if (v < state_5_semi)
        {
            return 20; // semi on
        }
        else
        {
            return 0; // off before first pulse
        }
    }
};

class SingleBlinkRoutine : public BlinkRoutine
{
private:
    long on_state;
    long off_state;

public:
    SingleBlinkRoutine()
    {
        this->on_state = 80;                   // ms
        this->off_state = 900;                 // ms
        this->duration = on_state + off_state; // ms
    }
    SingleBlinkRoutine(int on_time_in_ms, int off_time_in_ms)
    {
        on_state = on_time_in_ms;                          // ms
        off_state = off_time_in_ms;                        // ms
        this->duration = this->on_state + this->off_state; // ms
    }
    virtual int loop(long t)
    {
        return (t % this->duration <= on_state) ? 255 : 0;
    }
};

class DoubleBlinkRoutine : public BlinkRoutine
{
private:
    long on_state;
    long off_state;
    long on_state_2;

public:
    DoubleBlinkRoutine()
    {
        this->on_state = 80;    // ms
        this->off_state = 160;  // ms
        this->on_state_2 = 240; // ms
        this->duration = 1000;
    }
    DoubleBlinkRoutine(int on_time_in_ms, int off_time_in_ms, int on_time_in_ms_2, int off_time_in_ms_2)
    {
        this->on_state = on_time_in_ms;                 // ms
        this->off_state = on_state + off_time_in_ms;    // ms
        this->on_state_2 = off_state + on_time_in_ms_2; // ms
        this->duration = on_state_2 + off_time_in_ms_2; // ms
    }
    virtual int loop(long t)
    {
        if (t % this->duration <= on_state)
            return 255;
        if (t % this->duration <= off_state)
            return 0;
        if (t % this->duration <= on_state_2)
            return 255;
        return 0;
    }
};

class FadeRoutine : public BlinkRoutine
{
public:
    FadeRoutine()
    {
        this->duration = 1000;
    };
    FadeRoutine(long duration)
    {
        this->duration = duration;
    }
    int loop(long t)
    {
        return abs(((t % duration) / (duration * 1.0) * 2 * 255) - 255);
    }
};

class BlinkRoutineManager
{
private:
    std::vector<BlinkRoutine *> list;

public:
    BlinkRoutineManager()
    {
        this->list = list;
        this->addRoutine(new DoubleBlinkSemiRoutine());
        this->addRoutine(new SingleBlinkRoutine());
        this->addRoutine(new DoubleBlinkRoutine());
        this->addRoutine(new FadeRoutine());
    }
    void addRoutine(BlinkRoutine *routine)
    {
        list.push_back(routine);
    }
    void printRoutines()
    {
        std::vector<BlinkRoutine>::iterator it;
        for (auto const &i : this->list)
        {
            Serial.println("Routine");
        }
    }
    BlinkRoutine *get(uint8_t index)
    {
        return this->list[index % this->list.size()];
    }
};

#endif