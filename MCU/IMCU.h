/***************************************************************************//**
 * @brief MCU Interface Header File
 * 
 * @author Matthew Spinks
 * 
 * @date 10/14/22  Original creation
 * 
 * @file IMCU.h
 * 
 * @details
 *      An interface that will handle tasks such as sleep, shutdown, delay.
 * It will also have a very simple scheduler.
 * // TODO more details
 * 
 ******************************************************************************/

#ifndef IMCU_H
#define IMCU_H

#include <stdint.h>
#include <stdbool.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

typedef enum MCUPowerModeTag
{
    MCU_LPM_LEVEL_1,
    MCU_LPM_LEVEL_2,
    MCU_LPM_LEVEL_3, // lowest power consumption
} MCUPowerMode;

typedef struct MCUTaskTag MCUTask;

struct MCUTaskTag
{
    MCUTask *next;
    void (*Function)(void);
    unsigned int period;
    unsigned int count;
    // TODO add a pending flag and priority someday
};

/**
 * Description of struct members:
 * 
 */

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Add a task to the list
 * 
 * A task is just a function that you want to call periodically. The time 
 * period is completely arbitrary. Each call to the TaskTick function is one 
 * tick.
 * 
 * @param self  pointer to the task to be added
 * 
 * @param period  the desired period in ticks
 * 
 * @param Function  the function to be called. Format: void someFunction(void)
 */
void MCU_AddTask(MCUTask *self, unsigned int period, void (*Function)(void));

/***************************************************************************//**
 * @brief Main Task Timer
 * 
 * A simple timer that calls tasks. Call this function in your main loop. Each 
 * time the Tick function is executed, TaskLoop goes through the list of tasks. 
 * 
 * Be aware that this is a "run to completion" scheduler. Tasks are not 
 * suspended and no context is saved. So, if you have a task running every 
 * millisecond, be careful not to let any task take longer than that to execute.
 * 
 * The task scheduler is called via a flag. In the event that something does
 * block your fastest task, it will be called the next loop after the blocking
 * function finishes.
 */
void MCU_TaskLoop(void);

/***************************************************************************//**
 * @brief Tick the Task Loop
 * 
 * Every time this function is called, it will set a flag to notify the 
 * TaskLoop function that it needs to run. The period that you tick the timer 
 * and the period for your tasks is entirely up to you. You could call this 
 * function via a 1 ms system tick or a hardware timer.
 */
void MCU_TaskTick(void);

/* TODO Someday I might add suspend task */

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Delay microseconds
 * 
 * @param microseconds  the number of microseconds to delay
 */
void MCU_DelayUs(uint16_t microseconds);

/***************************************************************************//**
 * @brief Delay milliseconds
 * 
 * @param milliseconds  the number of milliseconds to delay
 */
void MCU_DelayMs(uint16_t milliseconds);

/***************************************************************************//**
 * @brief Enter low power mode
 * 
 * Because different manufacturers have different definitions of sleep, standby,
 * and idle, I've listed three levels of low power mode:
 * 
 * Level 1: Some or all clocks running, wake up from most interrupts
 * Level 2: Halt clocks and peripherals, wake up from select interrupts
 * Level 3: Lowest power consumption possible
 * 
 * Some processors may only have two levels of sleep. If that is the case, 
 * levels 2 and 3 should do the same thing. All three cases must be implemented
 * in some way.
 * 
 * @param powerMode  MCU_LPM_LEVEL_1, MCU_LPM_LEVEL_2, MCU_LPM_LEVEL_3
 */
void MCU_EnterLowPowerMode(MCUPowerMode powerMode);

#endif  /* IMCU_H */