/***************************************************************************//**
 * @brief Timer Library Header File
 * 
 * @author  Matthew Spinks
 * 
 * @date 1/17/19   Original creation
 * @date 10/1/21    Updated documention
 * @date 2/21/22    Added doxygen
 * @date 8/6/22     Updated doxygen
 * 
 * @file Timer.h
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

#ifndef TIMER_H
#define TIMER_H

// ***** Includes **************************************************************

#include <stdint.h>
#include <stdbool.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

typedef struct Timer Timer;

/*  callback function pointer. The context is so that you can know which timer 
    initiated the callback. This is so that you can service multiple timer 
    callbacks with the same function if you desire. */
typedef void (*TimerCallbackFunc)(Timer *timerContext);

// Free timer (with bit field)
struct Timer
{
    uint16_t period;
    uint16_t count;
    
    TimerCallbackFunc timerCallbackFunc;
    
    // bit field
    union {
        struct {
            unsigned start      :1;
            unsigned active     :1;
            unsigned expired    :1;
            unsigned            :0; // fill to nearest byte
        };
        uint8_t all;
    } flags;
};

/** These variable should be treated as private. You should only access them   
 *  with the use of a function.
 * 
 * period   The period of the timer. When the count reaches this number, a flag
 *          will be set
 * 
 * count    The current value of the timer
 * 
 * start    When this flag is set, the timer will begin counting down
 *          The active bit will be set as well
 * 
 * active   This bit is 1 whenever the timer is running. Clear this bit to stop
 *          the timer
 * 
 * expired  This flag is set whenever the timer period reaches the specified 
 *          count. You must clear this flag yourself
 */

// ***** Function Prototypes ***************************************************

/***************************************************************************//**
 * @brief Initialize a Timer object.
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
void Timer_InitMs(Timer *self, uint16_t periodMs, uint16_t tickMs);

/***************************************************************************//**
 * @brief Start the timer.
 * 
 * @param self  pointer to the Timer that you are using
 */
void Timer_Start(Timer *self);

/***************************************************************************//**
 * @brief Stop the timer.
 * 
 * When the timer stops the count is not reset until the timer starts again.
 * 
 * @param self  pointer to the Timer that you are using
 */
void Timer_Stop(Timer *self);

/***************************************************************************//**
 * @brief Update the timer.
 * 
 * Update the timer, check the period, and set flags if necessary. This 
 * function must be called periodically in the time that you specified when you 
 * initialized the Timer object.
 * 
 * @param self  pointer to the Timer that you are using
 */
void Timer_Tick(Timer *self);

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
uint16_t Timer_GetCount(Timer *self);

/***************************************************************************//**
 * @brief Get the period of the timer.
 * 
 * @param self  pointer to the Timer that you are using
 *
 * @return  value of the period (in ticks)
 */
uint16_t Timer_GetPeriod(Timer *self);

/***************************************************************************//**
 * @brief Check if timer is running.
 * 
 * @param self  pointer to the Timer that you are using
 *
 * @return  true if timer is running
 */
bool Timer_IsRunning(Timer *self);

/***************************************************************************//**
 * @brief Check if timer is finished. 
 * 
 * This flag is not cleared automatically.
 * 
 * @param self  pointer to the Timer that you are using
 *
 * @return  true if timer is finished
 */
bool Timer_IsFinished(Timer *self);

/***************************************************************************//**
 * @brief Clear the timer finished flag.
 * 
 * @param self  pointer to the Timer that you are using
 */
void Timer_ClearFlag(Timer *self);

/***************************************************************************//**
 * @brief Set a function to be called when the timer finishes.
 * 
 * The function prototype must have a pointer to a Timer as its argument. 
 * The context is so that multiple callbacks can be serviced by the same 
 * function if desired.
 * 
 * @param self  pointer to the Timer that you are using
 * 
 * @param Function  format: void SomeFunction(Timer *context)
 */
void Timer_SetFinishedCallback(Timer *self, TimerCallbackFunc);

#endif /* TIMER_H */