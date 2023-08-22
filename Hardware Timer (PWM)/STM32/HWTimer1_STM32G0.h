/***************************************************************************//**
 * @brief Hardware Timer 1 Implementation Header (STM32G0)
 * 
 * @file HWTimer1_STM32G0.h
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 9/18/22  Original creation
 * 
 * @details
 *      // TODO
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

#ifndef HW_TIM1_STM32G0_H
#define HW_TIM1_STM32G0_H

#include "HWTimer_STM32G0.h"

/* Include processor specific header files here if needed */


// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/* Declare and define this variable in your implementation's .c file */
extern HWTimerInterface HWTimer1_FunctionTable;

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

HWTimerPrescaleOptions HWTimer1_STM32_GetPrescaleOptions(void);

void HWTimer1_STM32_ComputePeriod(HWTimerInitType_STM32 *retParams, 
    uint32_t desiredFreqHz, uint32_t clkInHz, uint16_t *retDiffInTicks);

void HWTimer1_STM32_Init(HWTimerInitType_STM32 *params);

HWTimerSize HWTimer1_STM32_GetSize(void);

void HWTimer1_STM32_Start(void);

void HWTimer1_STM32_Stop(void);

void HWTimer1_STM32_Reset(void);

bool HWTimer1_STM32_IsRunning(void);

void HWTimer1_STM32_SetCount(uint16_t count);

uint16_t HWTimer1_STM32_GetCount(void);

void HWTimer1_STM32_AddToCount(uint16_t addToCount);

uint8_t HWTimer1_STM32_GetNumCompareChannels(void);

void HWTimer1_STM32_SetCompare16Bit(uint8_t compChan, uint16_t compValue);

uint16_t HWTimer1_STM32_GetCompare16Bit(uint8_t compChan);

void HWTimer1_STM32_SetComparePercent(uint8_t compChan, uint8_t percent);

uint8_t HWTimer1_STM32_GetComparePercent(uint8_t compChan);

void HWTimer1_STM32_EnableCompare(uint8_t compChan, bool useInterrupt);

void HWTimer1_STM32_DisableCompare(uint8_t compChan);

bool HWTimer1_STM32_GetOverflow(void);

bool HWTimer1_STM32_GetCompareMatch(uint8_t compChan);

void HWTimer1_STM32_ClearOverflowFlag(void);

void HWTimer1_STM32_ClearCompareMatchFlag(uint8_t compChan);

void HWTimer1_STM32_OverflowEvent(void);

void HWTimer1_STM32_CompareMatchEvent(void);

void HWTimer1_STM32_SetOverflowCallback(void (*Function)(void));

void HWTimer1_STM32_SetCompareMatchCallback(void (*Function)(uint8_t compChan));

// TODO This would require us to store our period in us. But it would be very convenient
void HWTimer1_STM32_SetCompareInUs(uint8_t compChan, uint32_t desiredTimeInUs);

// TODO
uint32_t HWTimer1_STM32_GetPeriodInUs(void);

#endif  /* HW_TIM1_STM32G0_H */