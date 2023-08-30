/***************************************************************************//**
 * @brief Hardware Timer Common Implementation (STM32F1)
 * 
 * @file HWTimer_STM32F1.c
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

#include "HWTimer_STM32F1.h"

/* Include processor specific header files here */


// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************


// ***** Static Function Prototypes ********************************************

/* Put static function prototypes here */


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void HWTimer_STM32_Create(HWTimer_STM32 *self, HWTimer *base, HWTimerInterface *interface)
{
    self->super = base;
    HWTimer_Create(base, self, interface);
}

// *****************************************************************************

void HWTimer_STM32_CreateInitType(HWTimerInitType_STM32 *self, HWTimerInitType *base)
{
    self->super = base;
    HWTimer_CreateInitType(base, self);
}

// *****************************************************************************

void HWTimer_STM32_SetInitTypeToDefaultParams(HWTimerInitType_STM32 *params)
{
    /* Add any extra STM32 parameters here */

    /* Call the base class function */
    HWTimer_SetInitTypeToDefaultParams(params->super);
}

// *****************************************************************************

void HWTimer_STM32_SetInitTypeParams(HWTimerInitType_STM32 *params)
{
    /* Add any extra STM32 parameters to the function */
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


/*
 End of File
 */