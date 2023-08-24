/***************************************************************************//**
 * @brief Hardware Timer Common Implementation Header (STM32F1)
 * 
 * @file HWTimer_STM32F1.h
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

#ifndef HW_TIM_STM32F1_H
#define HW_TIM_STM32F1_H

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

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 * @param base 
 */
void HWTimer_STM32_Create(HWTimer_STM32 *self, HWTimer *base, HWTimerInterface *interface);

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 * @param base 
 */
void HWTimer_STM32_CreateInitType(HWTimerInitType_STM32 *self, HWTimerInitType *base);


// TODO extra STM32 parameters
void HWTimer_STM32_SetInitTypeToDefaultParams(HWTimerInitType_STM32 *params);

void HWTimer_STM32_SetInitTypeParams(HWTimerInitType_STM32 *params, HWTimerPrescaleSelect prescaleSelect,
    uint16_t prescaleCounter, uint16_t period, bool useOverflowInterrupt, bool useCompareMatchInterrupts);

#endif  /* HW_TIM_STM32F1_H */