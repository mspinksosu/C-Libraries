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
    uint16_t period;
    uint16_t count;
    bool pending;
    uint8_t priority;
    MCUTask *nextPending;
};

/**
 * Description of struct members:
 * 
 * // TODO details
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

/* TODO Someday I might add suspend task */

/***************************************************************************//**
 * @brief Tick the Task Loop
 * 
 * Every time this function is called, each task's counter will be decremented.
 * Any task that is ready will have a pending flag set. This function should
 * ideally be called via an interrupt so that your task's counter are updated 
 * regularly. 
 * 
 * The period that you tick the timer and the period for your tasks is entirely 
 * up to you. You could call this function via a 1 ms system tick or a hardware 
 * timer.
 */
void MCU_TaskTick(void);

/***************************************************************************//**
 * @brief Delay
 * 
 * Decrements the count and do nothing until the count hits zero.
 * 
 * @param count  how long to delay
 */
void MCU_Delay(uint32_t count);

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
 * @brief Delay microseconds
 * 
 * The clock frequency is the main system clock frequency in Hertz. Not just 
 * the crystal, or fosc/4. Add a "UL" to the end of the number i.e. 32000000UL.
 * It might also help to have a #define for this value.
 * 
 * @param microseconds  the number of microseconds to delay
 * 
 * @param clkInHz  the clock frequency in Hz
 */
void MCU_DelayUs(uint16_t microseconds, uint32_t clkInHz);

/***************************************************************************//**
 * @brief Delay milliseconds
 * 
 * The clock frequency is the main system clock frequency in Hertz. Not just 
 * the crystal, or fosc/4. Add a "UL" to the end of the number i.e. 32000000UL.
 * It might also help to have a #define for this value.
 * 
 * @param milliseconds  the number of milliseconds to delay
 * 
 * @param clkInHz  the clock frequency in Hz
 */
void MCU_DelayMs(uint16_t milliseconds, uint32_t clkInHz);

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