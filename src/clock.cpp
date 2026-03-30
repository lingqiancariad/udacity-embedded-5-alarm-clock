/* 
*   FUNCTION CHECKLIST:
*   1. CLOCK:               done
*   2. SET TIME:            done
*   3. ALARM:               done
    4. ALARM ON/OFF SWITCH: 
*   4. BUTTONS:
*   ... 
*/

#include "clock.h"
#include "stdio.h"

/* TASK 1:  Create the global clock pointer to help loading the update_time() into timer interrupt
*/
Clock* global_clock_ptr = nullptr;

/* TASK 1:  Static function: Update time, show things on display and check alarm trigger
*           The IRAM_ATTR tells the ESP32 to keep this function in RAM for fast execution
*           This function should be called every 1 sec by the timer interrupt in Clock::run()
*/
static void IRAM_ATTR update_time()
{
    // Only if the gloabl_clock_ptr has been loaded in the Clock::run() 
    if (global_clock_ptr != nullptr) {
        global_clock_ptr->tick();
    }
}

// Empty constructor
Clock::Clock() {}

/* TASK 1:  Setup local display and alarm_tone variables in the Clock object.
*/
void Clock::init(TM1637* display, uint8_t buzzer_pin)
{ 
    this->display = display;
    alarm_tone.init(buzzer_pin);
    
    /*TASK 1:   This ensures the clock will display exactly the init_data in the first sec after the power on!
    *           Because the clock function relies on the 1 sec timer interrupt.
    *           Thus the display in the first sec is decided by init_data[]   
    */
    //int8_t init_data[] = {1, 2, 3, 4}; 
    int8_t init_data[] = {0, 0, 0, 0}; 
    this->display->display(init_data);
}


/* TASK 2:  Set the time hours, minutes and seconds to internal binary representation.
*           Inputs will be saved into local clock private variables.
*           Then they will be called by the Clock()::show() and with it displayed. 
*/
void Clock::set_time(uint8_t hour, uint8_t minutes, uint8_t seconds)
{   
    this->hour_time = hour;
    this->minutes_time = minutes;
    this->seconds_time = seconds;
    this->time_current = (hour * 3600) + (minutes * 60) + seconds;
}

/* Task 3:  Set the alarm hours and minutes 
*/
void Clock::set_alarm(uint8_t hour, uint8_t minutes)
{
    this->hour_alarm = hour;
    this->minutes_alarm = minutes;
    this->time_alarm = (hour * 3600) + (minutes * 60);
}

// Clock::menu_pressed(): Tell the internal clock that a button was pressed
//                        type: The button that was pressed
void Clock::button_pressed(ButtonType type) 
{
    /*
    switch (type) {
        case ButtonType::BUTTON_MENU:
            // todo
            break;
        case ButtonType::BUTTON_MINUS:
            // todo
            break;
        case ButtonType::BUTTON_OK:
            // todo
            break;
        case ButtonType::BUTTON_PLUS:
            // todo
            break;
        default:
            std::cout << "[Error:] ButtonType not valid!\n"; 
        
    }*/
}

/* Task 3:  Enable or disable alarm
*/
void Clock::turn_alarm(bool on_off)
{
    if (on_off) {
        this->alarm_on = true;
    } else {
        this->alarm_on = false;
    }
}

/* BASIC FUNCTION:  Load and show current time in THIS Clock object on display
*/
void Clock::show()
{
    int8_t display_buffer[4];

    // Split hours into two digits (e.g., 12 -> 1 and 2)
    display_buffer[0] = hour_time / 10;
    display_buffer[1] = hour_time % 10;

    // Split minutes into two digits (e.g., 45 -> 4 and 5)
    display_buffer[2] = minutes_time / 10;
    display_buffer[3] = minutes_time % 10;

    // Turn on the colon (:) in the middle
    // display->point(true);

    /* Task 5:  Make the colon blinks every two seconds.
    *           Use the modulo operator (%) to check if the second is even or odd.
    */
    if (seconds_time % 2 == 0) {
        display->point(true);   
    } else {
        display->point(false);
    }
    
    // Send to the hardware
    display->display(display_buffer);
}

/*  Task 4:  Alarm will be triggerd, if alarm_time matches current_time.            
*/
void Clock::check_alarm()
{
    /* Task 11: If the physical alarm switch is OFF, ensure the buzzer is quiet.
    */
    if (!this->alarm_on) {
        alarm_tone.stop();
        return; 
    }

    // If the switch is ON, check if the current time matches the alarm time
    if (this->hour_time == this->hour_alarm && this->minutes_time == this->minutes_alarm) {
        // Play the alarm tone! 
        // (Because this is called every second, it will keep ringing for the whole minute)
        alarm_tone.play();
    } else {
        // If the times don't match, make sure the buzzer is quiet
        alarm_tone.stop();
    }
}

/* TASK 1:  Update current time variables every time it got called by update_time() in the interrupt.
*           Call show() to display current time.
*  Task 4:  Run check_alarm() in everytime tick() to sound the alarm if alarm_time matches current_time.
*/
void Clock::tick()
{
    time_current++;
    seconds_time++;
    // Handle rollover for seconds, minutes, and hours
    if (seconds_time >= 60) {
        seconds_time = 0;
        minutes_time++;
        if (minutes_time >= 60) {
            minutes_time = 0;
            hour_time++;
            if (hour_time >= 24) {
                hour_time = 0;
            }
        }
    }

    // Update the display with the new time
    show();

    // Check if the alarm should go off (if you've implemented it)
    check_alarm(); 

    // If current time reaches 24:00:00, reset the current time variable back to 00:00:00. 
    if (time_current == 86400) {
        time_current = 0;
    }
}

/* BASIC FUNCTION:  Start running the clock
*                   Will only be dealing with interrupts.
*/
void Clock::run()
{   
    /* TASK 1:  Allow to trigger timer every 1 sec in the run()
                every timeout will update the time variables in THIS Clock object.
    */
    // 1. Load THIS specific clock object into our global pointer
    global_clock_ptr = this;
    // 2. Initialize the timer-> Timer 0, prescaler of 80 (makes the timer tick at 1 MHz, or 1 microsecond per tick)
    // timer = timerBegin(0, 80, true);
    // 3. Attach the interrupt function. The function will call Clock::tick() and with it the show() 
    // timerAttachInterrupt(timer, &update_time, true);
    // 4. Set the timer to trigger every 1,000,000 microseconds (1 second) -> the update_time() will be called every one sec.
    // timerAlarmWrite(timer, 1000000, true);
    // 5. Start the timer!
    // timerAlarmEnable(timer);

    ///* timer solution for Wokwi online for esp32 arduino core 3.0

    global_clock_ptr = this;
    timer = timerBegin(1000000); // V3 API
    timerAttachInterrupt(timer, &update_time); // V3 API
    timerAlarm(timer, 1000000, true, 0); // V3 API
    
    
}
