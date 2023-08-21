/***************************************************************************//**
 * @brief Hardware Timer 2 Implementation Header (Non-Processor Specific)
 * 
 * @file HWTimer2.h
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

#ifndef HW_TIM2_H
#define HW_TIM2_H

#include "IHardwareTimer.h"

/* Include processor specific header files here if needed */


// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/* Declare and define this variable in your implementation's .c file */
extern HWTimerInterface HWTimer2_FunctionTable;

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions with Inheritance  *******************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Simple Interface Functions ******************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/* These functions should be implemented for your processor exactly as they are
written. */

HWTimerSize HWTimer2_GetSize(void);

void HWTimer2_Start(void);

void HWTimer2_Stop(void);

void HWTimer2_Reset(void);

bool HWTimer2_IsRunning(void);

void HWTimer2_SetCount(uint16_t count);

uint16_t HWTimer2_GetCount(void);

void HWTimer2_AddToCount(uint16_t addToCount);

uint8_t HWTimer2_GetNumCompareChannels(void);

void HWTimer2_SetCompare16Bit(uint8_t compChan, uint16_t compValue);

uint16_t HWTimer2_GetCompare16Bit(uint8_t compChan);

void HWTimer2_SetComparePercent(uint8_t compChan, uint8_t percent);

uint8_t HWTimer2_GetComparePercent(uint8_t compChan);

void HWTimer2_EnableCompare(uint8_t compChan, bool useInterrupt);

void HWTimer2_DisableCompare(uint8_t compChan);

bool HWTimer2_GetOverflow(void);

bool HWTimer2_GetCompareMatch(uint8_t compChan);

void HWTimer2_ClearOverflowFlag(void);

void HWTimer2_ClearCompareMatchFlag(uint8_t compChan);

void HWTimer2_OverflowEvent(void);

void HWTimer2_CompareMatchEvent(void);

void HWTimer2_SetOverflowCallback(void (*Function)(void));

void HWTimer2_SetCompareMatchCallback(void (*Function)(uint8_t compChan));

// TODO This would require us to store our period in us. But it would be very convenient
void HWTimer2_SetCompareInUs(uint8_t compChan, uint32_t desiredTimeInUs);

// TODO
uint32_t HWTimer2_GetPeriodInUs(void);

#endif  /* HW_TIM2_H */