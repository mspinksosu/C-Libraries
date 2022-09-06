/***************************************************************************//**
 * @brief Timer Library Header File
 * 
 * @author  Matthew Spinks
 * 
 * @date 1/17/19    Original creation
 * @date 10/1/21    Updated documention
 * @date 2/21/22    Added doxygen
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
 *      I've provided a timer finished callback function. The function you 
 * create must follow the prototype listed for TimerCallbackFunc. It must have 
 * a void pointer as an argument. The idea is that when the callback function 
 * executes, you will get a pointer to the timer that called the function. This
 * way you can have multiple timers pointing to the same callback function if 
 * you desire. Then inside your callback function you can look at the context 
 * pointer to see which Timer object called the function and decide what to do.
 * The reason I chose a void pointer is so that you have the option of not 
 * using the contextPointer if you don't want to. This removes a dependency on 
 * needing to including Timer.h if your callback is in some other file. You're 
 * still going to get a pointer to the Timer, but it can be ignored.
 * 
 * Example Code:
 *      Timer startUpTimer;
 *      Timer_InitMs(&startupTimer, STARTUP_TIME_MS, TICK_1MS);
 *      Timer_Start(&startupTimer);
 *      if(Timer_IsFinished(&startupTimer))
 *      { 
 *          Timer_ClearFlag(&startupTimer);
 *          // do some stuff
 *      }
 ******************************************************************************/

#ifndef TIMER_H
#define TIMER_H

// ***** Includes **************************************************************

#include <stdint.h>
#include <stdbool.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/* callback function pointer. The context pointer will point to the Timer that 
initiated the callback. This is so that you can service multiple Timer 
callbacks with the same function if you desire. */
typedef void (*TimerCallbackFunc)(void *timerContext); 
/* TODO Experiment. Try with void pointer instead of pointer to timer. This could be useful for 
calling a function somewhere else without the need to include the Timer.h header file */

typedef struct TimerTag
{
    TimerCallbackFunc timerCallbackFunc;
    uint16_t period;
    uint16_t count;
    
    union {
        struct {
            unsigned start      :1;
            unsigned active     :1;
            unsigned expired    :1;
            unsigned            :0; // fill to nearest byte
        };
        uint8_t all;
    } flags;
} Timer;

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

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Function Prototypes *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

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
 * The function prototype must have a void pointer as its argument. The pointer
 * will be set to the Timer that called the function. This is so that multiple 
 * callbacks can be serviced by the same function if desired.
 * 
 * @param self  pointer to the Timer that you are using
 * 
 * @param Function  format: void SomeFunction(void *timerContext)
 */
void Timer_SetFinishedCallback(Timer *self, TimerCallbackFunc Function);

#endif /* TIMER_H */