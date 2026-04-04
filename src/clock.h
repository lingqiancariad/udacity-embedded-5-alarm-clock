#pragma once

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

enum MenuStatus
{
  MENU_IDLE,
  MENU_SETTIME,
  MENU_SETALARM,
};

enum OKStatus
{
  OK_IDLE,
  OK_SETHOUR,
  OK_SETMINUTE,
};

class Clock
{
private:
    // 7-segment Display
    TM1637* display = NULL;

    // Timer variable to count time
    hw_timer_t* timer = NULL;

    /* Task1:   Create AlarmTone object. Will be initialized by init()
    */
    AlarmTone alarm_tone;

    /* Task1:   Both hour and minutes equal 0 this makes the clock time starts at 00:00
    */
    uint8_t hours_time = 0;
    uint8_t minutes_time = 0;
    uint8_t seconds_time = 0;
    uint32_t time_current = 0; 

    /* Task3:   Both hour and minutes equal 0 this makes the alarm time set at 00:00 if no setup.
    *  Task4:   Save alarm time.
    */
    uint8_t hours_alarm = 0;
    uint8_t minutes_alarm = 0;
    uint32_t time_alarm = 0; 

    /* Task 6 & 7: Status of 'OK' and 'MENU' buttons.
    */
    MenuStatus status_button_menu = MENU_IDLE;
    OKStatus status_button_OK  = OK_IDLE;

    bool alarm_on = true;                             // Status of slide switch
    bool alarm_blinking = false;                      // Status of if alarm should blink
    bool alarm_display_OFF = false;                   // Status of if alarm should display 'OFF'
    uint32_t target_time_alarm_off_display = 0;       // The target time the alarm should stop displying 'OFF'
    bool alarm_off_ok_button = false;                 // Status of if OK is confirmed to stop alarm.

 

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

    // Main function which being called by timer interrupts every sec
    void tick();

    // Display functions 
    void show();
    void show_time();
    void show_time_hour_blink();
    void show_time_minute_blink();
    void show_alarm_hour_blink();
    void show_alarm_minute_blink();
    void show_alarm_blink();
    void show_alarm_off();
    void run();
};