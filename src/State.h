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
    bool slave;

public:
    uint sent_messages;
    uint delivered_messages;
    uint received_messages;
    unsigned long last_message_received_at;
    unsigned long sync;
    uint blink_routine;

public:
    State()
    {
        this->data.mode = 0;
        this->sent_messages = 0;
        this->delivered_messages = 0;
        this->received_messages = 0;
        this->updates = 0;
        this->slave = true;
        this->last_message_received_at = 0;
        this->sync = 0;
        this->blink_routine = 0;
    }
    void update()
    {
        this->updates++;
    }
    bool isSlave()
    {
        return this->slave;
    }
    void setSlave(bool s)
    {
        if (this->slave == s)
        {
            return;
        }
        this->slave = s;
        // check if should be slave
        // update last received message at to avoid an immediate switch back to master
        if (this->slave)
        {
            this->last_message_received_at = millis();
        }
        Serial.printf("Switching to %s mode!\n", s ? "slave" : "master");
        this->update();
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
        this->update();
    }
    void setPercentage(uint8_t p)
    {
        this->data.mode = p;
        this->update();
    }
    void print()
    {
        Serial.println("State");
        Serial.printf(" - %d sent messages\n", this->sent_messages);
        Serial.printf(" - %d delivered messages\n", this->delivered_messages);
        Serial.printf(" - %d received messages\n", this->received_messages);
        Serial.printf(" - %d updates\n", this->updates);
        Serial.printf(" - %d last_message_received_at\n", this->last_message_received_at);
    }
    esp_now_message pack()
    {
        return this->data;
    }
};

#endif