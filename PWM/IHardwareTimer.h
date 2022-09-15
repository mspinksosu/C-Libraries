/***************************************************************************//**
 * @brief Hardware Timer Interface Header File
 * 
 * @author Matthew Spinks
 * 
 * @date 9/17/22  Original creation
 * 
 * @file IHardwareTimer.h
 * 
 * @details
 *      An interface to make use of a hardware timer peripheral features.
 * // TODO more details
 * 
 ******************************************************************************/

#ifndef IHW_TIMER_H
#define IHW_TIMER_H

#include <stdint.h>
#include <stdbool.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

typedef enum HWTimerTypeTag
{
    HWTIM_FREE_RUNNING = 0,
    HWTIM_MONO_RESETTABLE,
    HWTIM_MONO_NONRESETTABLE
} HWTimerType;

typedef enum HWTimerSizeTag
{
    HWTIM_8_BIT = 0,
    HWTIM_16_BIT
} HWTimerSize;

typedef struct HWTimerInterfaceTag
{
    /* These are the functions that will be called. You will create your own
    interface object for your class that will have these function signatures.
    Set each of your functions equal to one of these pointers. */


} HWTimerInterface;

/* A forward declaration which will allow the compiler to "know" what a HWTimer
is before I use it in the callback function declaration below me */
typedef struct HWTimerTag HWTimer;

/* callback function pointer. The context is so that you can know which HWTimer 
initiated the callback. This is so that you can service multiple timers 
callbacks with the same function if you desire. */
typedef void (*HWTimerOverflowCallbackFunc)(HWTimer *timerContext);
typedef void (*HWTimerCompareMatchCallbackFunc)(HWTimer *timerContext, uint8_t compChan);

typedef struct HWTimerTag
{
    HWTimerInterface *interface;
    void *instance;
    HWTimerOverflowCallbackFunc overflowCallback;
    //HWTimerCompareMatchCallbackFunc compareMatchCallback;
    uint32_t periodInUs; // TODO not needed unless we want to set values in us
    //HWTimerType type;
    uint16_t period;
    uint16_t count;
};

/**
 * Description of struct members. You shouldn't really mess with any of these
 * variables directly. That is why I made functions for you to use.
 * 
 * 
 * 
 * 
 */

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Combine the base class, sub class, and function table
 * 
 * This function shouldn't be called directly. It is preferred that it is 
 * called from within the sub class constructor. This makes the function more 
 * type safe with the use of the void pointer
 * 
 * @param self 
 * @param instanceOfSubClass 
 * @param interface 
 */
void HWTimer_Create(HWTimer *self, void *instanceOfSubclass, HWTimerInterface *interface);

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

// TODO Probably don't need this or care
uint32_t HWTimer_ComputePeriodUs(HWTimer *self, uint32_t desiredPeriodUs, uint32_t clkInHz);

void HWTimer_Init(HWTimer *self, uint32_t periodInUs, uint32_t clkInHz);

HWTimerSize HWTimer_GetSize(HWTimer *self);

void HWTimer_Start(HWTimer *self);

void HWTimer_Stop(HWTimer *self);

void HWTimer_Reset(HWTimer *self);

bool HWTimer_IsRunning(HWTimer *self);

void HWTimer_SetCount(HWTimer *self, uint16_t count);

uint16_t HWTimer_GetCount(HWTimer *self);

uint8_t HWTimer_GetNumCompareChannels(HWTimer *self);

// For PWM generation
void HWTimer_SetCompare(HWTimer *self, uint8_t compChan, uint16_t compValue);

void HWTimer_SetComparePercent(HWTimer *self, uint8_t compChan, uint8_t percent);

uint16_t HWTimer_GetCompare(HWTimer *self, uint8_t compChan);

void HWTimer_EnableCompare(HWTimer *self, uint8_t compChan);

void HWTimer_DisableCompare(HWTimer *self, uint8_t compChan);

bool HWTimer_GetOverflow(HWTimer *self);

bool HWTimer_GetCompareMatch(HWTimer *self, uint8_t compChan);

void HWTimer_ClearOverflowFlag(HWTimer *self);

void HWTimer_ClearCompareMatchFlag(HWTimer *self, uint8_t compChan);

void HWTimer_CompareOverflowEvent(HWTimer *self);

void HWTimer_CompareMatchEvent(HWTimer *self, uint8_t compChan);

void HWTimer_SetOverflowCallback(HWTimer *self, HWTimerOverflowCallbackFunc Function);

void HWTimer_SetCompareMatchCallback(HWTimer *self, HWTimerCompareMatchCallbackFunc Function);

// TODO This would require us to store our period in us. But it would be very convenient
void HWTimer_SetCompareInUs(HWTimer *self, uint8_t compChan, uint32_t desiredTimeInUs);

// TODO
uint32_t HWTimer_GetPeriodInUs(HWTimer *self);

#endif  /* IHW_TIMER_H */