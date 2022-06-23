/***************************************************************************//**
 * @brief Timer Library
 * 
 * @author  Matthew Spinks
 * 
 * @date 1/17/19    Original creation
 *       10/1/21    Updated documention
 *       2/21/22    Added doxygen
 * 
 * @file Timer.c
 * 
 * @details
 *      A generic, free-running timer to do whatever you need. To create a
 * timer, you will need a Timer object, the period in you want in milleseconds,
 * and the expected update rate in milliseconds (how often you call the timer 
 * tick function). This is not meant to be an extremely accurate timer. It's
 * useful for blinking LED's, buttons etc.
 * 
 *      Once the timer finishes, the "expire" flag is set. But the timer will 
 * continue to run as long as the "active" flag is set. The "expired" flag will 
 * not get cleared automatically. This is for you to check and decide what to 
 * do. How you choose to call the "Timer_Tick" function is up to you. Just be 
 * sure to call it periodically and make sure to initialize the timer with the 
 * expected rate. This lets the timer know how far it must count. 
 * 
 *      I've provided a timer finished callback function. The function pointer 
 * you create must follow the prototype listed in Timer.h. It must have a Timer 
 * object as the return type. The idea is that when you set up the callback 
 * function, you tell it which Timer object will be calling it. This provides 
 * context for you so that if you have multiple timers, you can tell which one 
 * executed the callback function. This way you can have multiple timers 
 * pointing to the same callback function if you desire. Then you could look at 
 * the Timer object to see which one called it and decide what to do.
 * 
 ******************************************************************************/

#include "Timer.h"

// ***** Defines ***************************************************************


// ***** Function Prototypes ***************************************************


// ***** Global Variables ******************************************************


/***************************************************************************//**
 * @brief Initializes a Timer object.
 * 
 * If you accidentally set the period to a value less than one tick, the timer
 * will never run.
 * 
 * @param self  pointer to the Timer that you are using
 * 
 * @param periodMs  the period of the timer in milliseconds
 * 
 * @param tickMs  how often you plan to call the Timer Tick function
 */
void Timer_InitMs(Timer *self, uint16_t periodMs, uint16_t tickMs)
{
    if(tickMs != 0)
        self->period = periodMs / tickMs;
}

/***************************************************************************//**
 * @brief Start the timer.
 * 
 * Once the timer has begun counting, it's status becomes active.
 * 
 * @param self  pointer to the Timer that you are using
 */
void Timer_Start(Timer *self)
{
    if(self->period != 0)
    {
        self->flags.start = 1;
    }
}

/***************************************************************************//**
 * @brief Stop the timer.
 * 
 * Once the timer stops, it's status is set to inactive. The count is not reset 
 * until the timer starts again.
 * 
 * @param self  pointer to the Timer that you are using
 */
void Timer_Stop(Timer *self)
{
    self->flags.start = 0;
    self->flags.active = 0;
}

/***************************************************************************//**
 * @brief Update the timer.
 * 
 * Update the timer, check the period, and set flags if necessary. This 
 * function must be called periodically in the time that you specified when you 
 * initialized the Timer object.
 * 
 * @param self  pointer to the Timer that you are using
 */
void Timer_Tick(Timer *self)
{
    // Check to see if timer is active and ready to start
    if(self->flags.start && self->period != 0)
    {
        self->flags.start = 0;
        self->count = self->period;
        self->flags.active = 1;
    }
    
    // Update active timers
    if(self->flags.active)
    {
        self->count--;
        
        if(self->count == 0)
        {
            self->flags.active = 0;
            self->flags.expired = 1;
            
            if(self->timerCallbackFunc)
            {
                self->timerCallbackFunc(self);
            }
        }
    }
}

/***************************************************************************//**
 * @brief Get the count of the timer.
 * 
 * I wrote my timers to count down to zero. I realize not everyone likes that,
 * so count returns the value as if it were counting up, not down.
 * 
 * @param self  pointer to the Timer that you are using
 *
 * @return  the current value of the count
 */
uint16_t Timer_GetCount(Timer *self)
{
    return (self->period - self->count);
}

/***************************************************************************//**
 * @brief Get the period of the timer.
 * 
 * @param self  pointer to the Timer that you are using
 *
 * @return  value of the period (in ticks)
 */
uint16_t Timer_GetPeriod(Timer *self)
{
    return self->period;
}

/***************************************************************************//**
 * @brief Check if timer is running.
 * 
 * @param self  pointer to the Timer that you are using
 *
 * @return  true if timer is running
 */
bool Timer_IsRunning(Timer *self)
{
    if(self->flags.active)
        return true;
    else
        return false;
}

/***************************************************************************//**
 * @brief Check if timer is finished. This flag is not cleared automatically.
 * 
 * @param self  pointer to the Timer that you are using
 *
 * @return  true if timer is finished
 */
bool Timer_IsFinished(Timer *self)
{
    if(self->flags.expired)
        return true;
    else
        return false;
}

/***************************************************************************//**
 * @brief Clear the timer finished flag.
 * 
 * @param self  pointer to the Timer that you are using
 */
void Timer_ClearFlag(Timer *self)
{
    self->flags.expired = 0;
}

/***************************************************************************//**
 * @brief A function pointer that is called when the timer finishes.
 * 
 * The function prototype must have a pointer to a Timer as its argument. 
 * The context is so that multiple callbacks can be serviced by the same 
 * function if desired.
 * See description at top of file.
 * 
 * @param self  pointer to the Timer that you are using
 * 
 * @param Function  format: void SomeFunction(Timer *context)
 */
void Timer_SetFinishedCallback(Timer *self, TimerCallbackFunc Function)
{
    self->timerCallbackFunc = Function;
}

/*
 End of File
 */
