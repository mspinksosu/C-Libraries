/***************************************************************************//**
 * @brief MCU Interface Header File
 * 
 * @file IMCU.h
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 10/14/22  Original creation
 * 
 * @details
 *      An interface that will handle tasks such as sleep, shutdown, delay.
 * It will also have a very simple scheduler.
 * // TODO more details
 * 
 * @section license License
 * SPDX-FileCopyrightText: © 2022 Matthew Spinks
 * SPDX-License-Identifier: Zlib
 * 
 * This software is released under the Zlib license. You are free alter and
 * redistribute it, but you must not misrepresent the origin of the software.
 * This notice may not be removed. <http://www.zlib.net/zlib_license.html>
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
    MCUTask *nextPending;
    void (*Function)(void);
    uint16_t period;
    uint16_t count;
    bool addToPending;
    bool pending;
    uint8_t priority;
};

/**
 * Description of struct members: You shouldn't need to mess with any of these
 * variables directly. That is why I made functions for you to use.
 * 
 * next  A pointer to the next task in the list of tasks
 * 
 * nextPending  A pointer to the next task in the pending task list. This list
 *              contains just the currently pending tasks sorted by their 
 *              priority.
 * 
 * Function  A pointer to the function/task that you wish to have called
 * 
 * period  How often you wish to call your function (in ticks)
 * 
 * count  How long until it's time to call your function (in ticks)
 * 
 * addToPending  Signals to the task loop to insert this task into the pending
 *               task list to be executed
 * 
 * pending  Set to true when a task is added to the pending task list. Set to
 *          false after the task/function has finished
 * 
 * priority  The priority of the task. 0 is the highest priority.
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
 * period is just an arbitrary number measured in ticks. It could be in
 * milliseconds, or whatever you want it to be. Each call to the TaskTick 
 * function is one tick.
 * 
 * @param self  pointer to the task to be added
 * 
 * @param period  the desired period in ticks (1 is lowest)
 * 
 * @param priority  from 0 to 127. 0 is highest priority
 * 
 * @param Function  the function to be called. Format: void someFunction(void)
 */
void MCU_AddTask(MCUTask *self, uint16_t period, uint8_t priority, void (*Function)(void));

/***************************************************************************//**
 * @brief Main Task Scheduler
 * 
 * This is a simple non-preemptive, priority based scheduler. Call this 
 * function in your main loop.
 * 
 * It will go through the list of tasks and look for any pending tasks. If 
 * more than one task is pending, the one with highest priority (lowest number)
 * gets executed next. If two tasks have the same priority, they are executed 
 * on a first come, first serve basis.
 * 
 * Tasks are not suspended and no context is saved. Each task will run to 
 * completion, so be careful not to let your task take too long. If you have a 
 * high priority task that takes lots of time to execute, or you have a high 
 * priority task executing every tick, you run the risk of starving the low 
 * priority tasks.
 */
void MCU_TaskLoop(void);

/***************************************************************************//**
 * @brief Tick the Task Loop
 * 
 * Every time this function is called, each task's counter will be decremented.
 * Any task that is ready will have a pending flag set. This function should
 * ideally be called via an interrupt so that your tasks' counters are updated 
 * regularly even if one task is taking a bit too long. 
 * 
 * The period that you tick the timer and the period for your tasks is entirely 
 * up to you. You could call this function via a 1 ms system tick or a hardware 
 * timer.
 */
void MCU_TaskTick(void);

/***************************************************************************//**
 * @brief Delay
 * 
 * Decrements the count and does nothing until the count hits zero.
 * 
 * @param count  how long to delay
 */
void MCU_Delay(uint32_t count);

/***************************************************************************//**
 * @brief Test if your processor is little endian or big endian
 * 
 * @return true  little endian
 */
bool MCU_IsLittleEndian(void);

/***************************************************************************//**
 * @brief Copy bytes in reverse order
 * 
 * Useful if you need to send something like a float from a big endian 
 * processor to a little endian processor or vice versa.
 * 
 * @param dst  destination address
 * 
 * @param src  source address
 * 
 * @param n  number of bytes to copy
 * 
 * @return void*  pointer to the destination
 */
void *MCU_ReverseMemcpy(void *dst, const void *src, uint16_t n);

/***************************************************************************//**
 * @brief Simple Max function
 * 
 * Avoids side effects of using preprocessor macro such as double evaluation
 * 
 * @param a 
 * 
 * @param b 
 * 
 * @return int32_t  the largest of the two parameters
 */
inline int32_t MCU_Max(int32_t a, int32_t b) { return ((a) > (b) ? a : b); }

/***************************************************************************//**
 * @brief Simple Min function
 * 
 * Avoids side effects of using preprocessor macro such as double evaluation
 * 
 * @param a 
 * 
 * @param b 
 * 
 * @return int32_t  the smallest of the two parameters
 */
inline int32_t MCU_Min(int32_t a, int32_t b) { return ((a) < (b) ? a : b); }

/***************************************************************************//**
 * @brief Simple Max function unsigned
 * 
 * @param a 
 * 
 * @param b 
 * 
 * @return uint32_t  the largest of the two parameters
 */
inline uint32_t MCU_MaxU32(uint32_t a, uint32_t b) { return ((a) > (b) ? a : b); }

/***************************************************************************//**
 * @brief Simple Min function unsigned
 * 
 * @param a 
 * 
 * @param b 
 * 
 * @return uint32_t  the smallest of the two parameters
 */
inline uint32_t MCU_MinU32(uint32_t a, uint32_t b) { return ((a) < (b) ? a : b); }

/***************************************************************************//**
 * @brief Simple Limit function
 * 
 * @param a 
 * 
 * @param min
 * 
 * @param max
 * 
 * @return int32_t  the input limited by the two parameters
 */
inline int32_t MCU_Limit(int32_t a, int32_t min, int32_t max) 
{
    return ((a) > (max) ? max : (a) < (min) ? min : a);
}

/***************************************************************************//**
 * @brief Simple Limit function unsigned
 * 
 * @param a 
 * 
 * @param min
 * 
 * @param max
 * 
 * @return uint32_t  the input limited by the two parameters
 */
inline uint32_t MCU_LimitU32(uint32_t a, uint32_t min, uint32_t max) 
{
    return ((a) > (max) ? max : (a) < (min) ? min : a);
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Initialize the system clock
 * 
 * You may use this function to initialize your main clock and all of your 
 * peripheral clocks, or just the main clock if desired. At the very least, I
 * like to store the value passed in so I can use it with the DelayUs function
 * if needed.
 * 
 * The value passed in is whatever the clock is that goes to the CPU. If you 
 * have a PLL before the CPU, then this is what the output of that PLL should 
 * be. The reason I say this is sometimes Microchip will have a Fosc/4 
 * instruction clock. In that case the system clock is just Fosc, not Fosc/4.
 * 
 * @param desiredClkInHz  the desired clock frequency in Hz
 * 
 * @param xtalInHz  the crystal in Hz (0 if not used)
 * 
 * @return uint32_t  the clock frequency chosen in Hz
 */
uint32_t MCU_InitSystemClock(uint32_t desiredClkInHz, uint32_t xtalInHz);

/***************************************************************************//**
 * @brief Reset the watch dog timer
 * 
 */
void MCU_WatchdogPet(void);

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