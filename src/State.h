#ifndef __STATE__
#define __STATE__

#include <Arduino.h>

typedef struct esp_now_message
{
    int mode;
} esp_now_message;

class State
{
private:
    esp_now_message data;
    uint updates;

public:
    State()
    {
        this->data.mode = 0;
        this->updates = 0;
    }
    bool hasUpdate()
    {
        return this->updates > 0 ? true : false;
    }
    void reset()
    {
        this->updates = 0;
    }
    void setMode(uint8_t m)
    {
        // this->data.mode = m;
        // this->updates++;
    }
    void setPercentage(uint8_t p)
    {
        this->data.mode = p;
        this->updates++;
        Serial.println("Reached main");
    }
    esp_now_message pack()
    {
        return data;
    }
};

#endif