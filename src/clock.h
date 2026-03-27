#ifndef CLOCK_H
#define CLOCK_H

#include <cstdint>
#include <Arduino.h>
#include "tm1637.h"
#include "alarm_tone.h"

enum ButtonType
{
  BUTTON_MENU,
  BUTTON_PLUS,
  BUTTON_MINUS,
  BUTTON_OK,
};

class Clock
{
private:
    // 7-segment Display
    TM1637* display = NULL;

    // Timer variable to count time
    hw_timer_t* timer = NULL;

    // TODO: Add other private variables here
    uint8_t hour_time = 0;
    uint8_t minutes_time = 0;
    uint8_t seconds_time = 0;
    uint32_t time_current = 0;
    
    uint8_t hour_alarm = 0;
    uint8_t minutes_alarm = 0;
    uint32_t time_alarm = 0;

    bool alarm_on = false;

    ButtonType button_type;

    AlarmTone alarm_tone;


public:
    // Constructor
    Clock();

    // Init function
    void init(TM1637* display, uint8_t buzzer_pin);
    
    // Set time and alarm time
    void set_time(uint8_t hour, uint8_t minutes, uint8_t seconds);
    void set_alarm(uint8_t hour, uint8_t minutes);
    
    // Button pressed function
    void button_pressed(ButtonType type);
    
    // Alarm functions
    void turn_alarm(bool on_off);
    void check_alarm();

    void tick();

    // Clock functions 
    void show();
    void run();

    // TODO: Add other public variables/functions here
    
};

#endif