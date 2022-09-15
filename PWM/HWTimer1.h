/***************************************************************************//**
 * @brief Timer 1 Implementation Header (Non-Processor Specific)
 * 
 * @author Matthew Spinks
 * 
 * @date 9/17/22  Original creation
 * 
 * @file HWTimer1.h
 * 
 * @details
 *      TODO
 * 
 ******************************************************************************/

#ifndef HW_TIM1_H
#define HW_TIM1_H

#include "IHardwareTimer.h"

/* Include processor specific header files here if needed */


// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/* Declare and define this variable in your implementation's .c file */
extern HWTimerInterface HWTimer1_FunctionTable;

/* If you need to extend the base class, then declare your processor specific
class here. Your processor specific functions should all use this type in place 
of the base class type. */
// typedef struct Foo_MCU1Tag
// {
//     Foo *super; // include the base class first
//     /* Add any processor specific variables you need here */
// } Foo_MCU1;

/** 
 * Description of struct
 * 
 * member1      description of variable member1
 * 
 */

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

// TODO Probably don't need this or care
uint32_t HWTimer1_ComputePeriodUs(HWTimer *self, uint32_t desiredPeriodUs, uint32_t clkInHz);

void HWTimer1_Init(HWTimer *self, uint32_t periodInUs, uint32_t clkInHz);

HWTimerSize HWTimer1_GetSize(HWTimer *self);

void HWTimer1_Start(HWTimer *self);

void HWTimer1_Stop(HWTimer *self);

void HWTimer1_Reset(HWTimer *self);

bool HWTimer1_IsRunning(HWTimer *self);

void HWTimer1_SetCount(HWTimer *self, uint16_t count);

uint16_t HWTimer1_GetCount(HWTimer *self);

uint8_t HWTimer1_GetNumCompareChannels(HWTimer *self);

// For PWM generation
void HWTimer1_SetCompare(HWTimer *self, uint8_t compChan, uint16_t compValue);

void HWTimer1_SetComparePercent(HWTimer *self, uint8_t compChan, uint8_t percent);

uint16_t HWTimer1_GetCompare(HWTimer *self, uint8_t compChan);

void HWTimer1_EnableCompare(HWTimer *self, uint8_t compChan);

void HWTimer1_DisableCompare(HWTimer *self, uint8_t compChan);

bool HWTimer1_GetOverflow(HWTimer *self);

bool HWTimer1_GetCompareMatch(HWTimer *self, uint8_t compChan);

void HWTimer1_ClearOverflowFlag(HWTimer *self);

void HWTimer1_ClearCompareMatchFlag(HWTimer *self, uint8_t compChan);

void HWTimer1_CompareOverflowEvent(HWTimer *self);

void HWTimer1_CompareMatchEvent(HWTimer *self, uint8_t compChan);

void HWTimer1_SetOverflowCallback(HWTimer *self, HWTimerOverflowCallbackFunc Function);

void HWTimer1_SetCompareMatchCallback(HWTimer *self, HWTimerCompareMatchCallbackFunc Function);

// TODO This would require us to store our period in us. But it would be very convenient
void HWTimer1_SetCompareInUs(HWTimer *self, uint8_t compChan, uint32_t desiredTimeInUs);

// TODO
uint32_t HWTimer1_GetPeriodInUs(HWTimer *self);

#endif  /* HW_TIM1_H */