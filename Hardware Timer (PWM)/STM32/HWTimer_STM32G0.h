/***************************************************************************//**
 * @brief Hardware Timer Implementation Header (STM32G0)
 * 
 * @file HWTimer_STM32G0.h
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

#ifndef HW_TIM_STM32G0_H
#define HW_TIM_STM32G0_H

#include "IHardwareTimer.h"

/* Include processor specific header files here if needed */


// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/* If you need to extend the base class, then declare your processor specific
class here. Your processor specific functions should all use this type in place 
of the base class type. */
typedef struct HWTimer_STM32Tag
{
    HWTimer *super; // include the base class first
    // TODO add output pin enable and polarity
    // TODO add preload
} HWTimer_STM32;

typedef struct HWTimerInitType_STM32Tag
{
    HWTimerInitType *super;
    /* Add any processor specific variables you need here */
} HWTimerInitType_STM32;

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

void HWTimer_STM32_Create(HWTimer_STM32 *self, HWTimer *base);

void HWTimer_STM32_CreateInitType(HWTimerInitType_STM32 *self, HWTimerInitType *base);

#endif  /* HW_TIM_STM32G0_H */