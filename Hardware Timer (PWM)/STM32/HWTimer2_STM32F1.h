/***************************************************************************//**
 * @brief Hardware Timer 2 Implementation Header (STM32F1)
 * 
 * @file HWTimer1_STM32F1.h
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

/* Include processor specific header files here if needed */


// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************


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

void HWTimer2_STM32_Init(HWTimerInitType_STM32 *params);

#endif  /* HW_TIM2_STM32F1_H */