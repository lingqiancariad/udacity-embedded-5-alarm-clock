
#include "clock.h"
#include "stdio.h"
#include <vector>

// Create the global pointer (starts as null)
Clock* global_clock_ptr = nullptr;

// Static function: Update time, show things on display
//                  and check alarm trigger

// The IRAM_ATTR tells the ESP32 to keep this function in RAM for fast execution
static void IRAM_ATTR update_time()
{
    // Only if the gloabl_clock_ptr has been loaded in the Clock::run() 
    if (global_clock_ptr != nullptr) {
        global_clock_ptr->tick();
    }
}

// Empty constructor
Clock::Clock() {}

// Clock::init(): Initialize internal variables,
//                set display to use and buzzer pin
void Clock::init(TM1637* display, uint8_t buzzer_pin)
{ 
    this->display = display; // Store the pointer so other methods can use it!
    alarm_tone.init(buzzer_pin);
    
    // Simple way to clear/init display with zeros
    int8_t init_data[] = {5, 5, 2, 2}; 
    this->display->display(init_data);
}

// Clock::set_time(): Set the time hour, minutes and seconds
//                    to internal binary representation
void Clock::set_time(uint8_t hour, uint8_t minutes, uint8_t seconds)
{   
    this->hour_time = hour;
    this->minutes_time = minutes;
    this->seconds_time = seconds;
}

// Clock::set_alarm(): Set the alarm hour, minutes and seconds
//                     to internal binary representation
void Clock::set_alarm(uint8_t hour, uint8_t minutes)
{
    this->hour_alarm = hour;
    this->minutes_alarm = hour;
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

// Clock::turn_alarm(): Enable or disable alarm
void Clock::turn_alarm(bool on_off)
{
    if (on_off) {
        this->alarm_on = true;
    } else {
        this->alarm_on = false;
    }
}

// Clock::show(): Show time or menu on display
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
    display->point(true); 
    
    // Send to the hardware
    display->display(display_buffer);
}

// Clock::check_alarm(): Check if alarm needs to be triggered
void Clock::check_alarm()
{
}

void Clock::tick()
{
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
    // check_alarm(); 
}

// Clock::run(): Start running the clock
//               This function MUST not block, everything should be handled
//               by interrupts
void Clock::run()
{
    // For the first try, just show the initial time
    // 1. Initialize the timer
    // Timer 0, prescaler of 80 (makes the timer tick at 1 MHz, or 1 microsecond per tick)
    // 1. Save THIS specific clock object into our global pointer
    global_clock_ptr = this;

    // 2. Initialize the timer (Timer 0, prescaler 80 = 1 tick per microsecond)
    timer = timerBegin(0, 80, true);
    
    // 3. Attach the interrupt using the standard function
    timerAttachInterrupt(timer, &update_time, true);
    
    // 4. Set the timer to trigger every 1,000,000 microseconds (1 second)
    timerAlarmWrite(timer, 1000000, true);
    
    // 5. Start the timer!
    timerAlarmEnable(timer);
    
    show();

}
