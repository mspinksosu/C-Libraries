/***************************************************************************//**
 * @brief Hardware Timer 3 Implementation Header (STM32F1)
 * 
 * @file HWTimer3_STM32F1.h
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 9/18/22  Original creation
 * 
 * @details
 *      // TODO Add more notes
 * 
 * Fortunately, on this micro the registers that deal with basic timer settings 
 * are the same across timers. That doesn't mean it will true for other 
 * microcontrollers. Other timer implementations may need to be altered. Also 
 * some timers have more advanced features than others, but I won't be dealing 
 * with any of that yet.
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

#ifndef HW_TIM3_STM32F1_H
#define HW_TIM3_STM32F1_H

#include "HWTimer_STM32F1.h"

/* Include processor specific header files here if needed */


// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/* Declare and define this variable in your implementation's .c file */
extern HWTimerInterface HWTimer3_FunctionTable;

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

HWTimerPrescaleOptions HWTimer3_STM32_GetPrescaleOptions(void);

void HWTimer3_STM32_ComputePeriod(HWTimerInitType_STM32 *retParams, 
    uint32_t desiredFreqHz, uint32_t clkInHz, uint16_t *retDiffInTicks);

void HWTimer3_STM32_Init(HWTimerInitType_STM32 *params);

HWTimerSize HWTimer3_STM32_GetSize(void);

void HWTimer3_STM32_Start(void);

void HWTimer3_STM32_Stop(void);

void HWTimer3_STM32_Reset(void);

bool HWTimer3_STM32_IsRunning(void);

void HWTimer3_STM32_SetCount(uint16_t count);

uint16_t HWTimer3_STM32_GetCount(void);

void HWTimer3_STM32_AddToCount(uint16_t addToCount);

uint8_t HWTimer3_STM32_GetNumCompareChannels(void);

void HWTimer3_STM32_SetCompare16Bit(uint8_t compChan, uint16_t compValue);  // TODO change to duty?

uint16_t HWTimer3_STM32_GetCompare16Bit(uint8_t compChan);

void HWTimer3_STM32_SetComparePercent(uint8_t compChan, uint8_t percent);

uint8_t HWTimer3_STM32_GetComparePercent(uint8_t compChan);

void HWTimer3_STM32_EnableComparePWM(uint8_t compChan);

void HWTimer3_STM32_DisableComparePWM(uint8_t compChan);

bool HWTimer3_STM32_GetOverflow(void);

void HWTimer3_STM32_ClearOverflowFlag(void);

bool HWTimer3_STM32_GetCompareMatch(uint8_t compChan);

void HWTimer3_STM32_ClearCompareMatchFlag(uint8_t compChan);

void HWTimer3_STM32_OverflowEvent(void);

void HWTimer3_STM32_CompareMatchEvent(void);

void HWTimer3_STM32_SetOverflowCallback(void (*Function)(void));

void HWTimer3_STM32_SetCompareMatchCallback(void (*Function)(uint8_t compChan));

// TODO This would require us to store our period in us. But it would be very convenient
void HWTimer3_STM32_SetCompareInUs(uint8_t compChan, uint32_t desiredTimeInUs);

// TODO
uint32_t HWTimer3_STM32_GetPeriodInUs(void);

#endif  /* HW_TIM3_STM32F1_H */