/***************************************************************************//**
 * @brief Hardware Timer 2 Implementation Header (STM32F1)
 * 
 * @file HWTimer2_STM32F1.h
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

#ifndef HW_TIM2_STM32F1_H
#define HW_TIM2_STM32F1_H

#include "HWTimer_STM32F1.h"
#include "HWTimer2.h"

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

void HWTimer2_STM32_ComputePeriodUs(HWTimerInitType_STM32 *params, uint32_t desiredPeriodUs, 
    uint32_t clkInHz, uint16_t *retDiffInTicks);

void HWTimer2_STM32_Init(HWTimerInitType_STM32 *params);

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Simple Interface Functions ******************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/* Implement the functions listed in HWTimer2.h under "Simple Interface 
Functions" in your own .c file exactly as written. For the file name, I usually 
name my .c file with an underscore prefix, such as HWTimer2_STM32G0.c or 
HWTimer2_PIC32MX.c. Don't forget to include IHardwareTimer.h. */

#endif  /* HW_TIM2_STM32F1_H */