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

HWTimerPrescaleOptions HWTimer1_GetPrescaleOptions(void);

HWTimerInitType HWTimer1_ComputePeriodUs(uint32_t desiredPeriodUs, uint32_t clkInHz, uint16_t *retDiffInTicks);

void HWTimer1_Init(HWTimerInitType *params);

HWTimerSize HWTimer1_GetSize(void);

void HWTimer1_Start(void);

void HWTimer1_Stop(void);

void HWTimer1_Reset(void);

bool HWTimer1_IsRunning(void);

void HWTimer1_SetCount(uint16_t count);

uint16_t HWTimer1_GetCount(void);

void HWTimer1_AddToCount(uint16_t addToCount);

uint8_t HWTimer1_GetNumCompareChannels(void);

void HWTimer_SetCompare16Bit(uint8_t compChan, uint16_t compValue);

uint16_t HWTimer1_GetCompare16Bit(uint8_t compChan);

void HWTimer1_SetComparePercent(uint8_t compChan, uint8_t percent);

uint16_t HWTimer1_GetCompare(uint8_t compChan);

void HWTimer1_EnableCompare(uint8_t compChan);

void HWTimer1_DisableCompare(uint8_t compChan);

bool HWTimer1_GetOverflow(void);

bool HWTimer1_GetCompareMatch(uint8_t compChan);

void HWTimer1_ClearOverflowFlag(void);

void HWTimer1_ClearCompareMatchFlag(uint8_t compChan);

void HWTimer1_OverflowEvent(void);

void HWTimer1_CompareMatchEvent(uint8_t compChan);

void HWTimer1_SetOverflowCallback(HWTimerOverflowCallbackFunc Function);

void HWTimer1_SetCompareMatchCallback(HWTimerCompareMatchCallbackFunc Function);

// TODO This would require us to store our period in us. But it would be very convenient
void HWTimer1_SetCompareInUs(uint8_t compChan, uint32_t desiredTimeInUs);

// TODO
uint32_t HWTimer1_GetPeriodInUs(void);

#endif  /* HW_TIM1_H */