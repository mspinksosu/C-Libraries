/***************************************************************************//**
 * @brief I2C Target Implementation Header (Non-Processor Specific)
 * 
 * @file I2CTarget.h
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 5/1/25    Original creation
 * 
 * @details
 *      @todo add more details. Generic I2C target object. Will use my data 
 * transfer buffer. - MS
 * 
 * @section license License
 * SPDX-FileCopyrightText: © 2025 Matthew Spinks
 * SPDX-License-Identifier: Zlib
 * 
 * This software is released under the Zlib license. You are free alter and
 * redistribute it, but you must not misrepresent the origin of the software.
 * This notice may not be removed. <http://www.zlib.net/zlib_license.html>
 * 
 ******************************************************************************/

#ifndef I2C_TARGET_H
#define I2C_TARGET_H

#include "II2CTarget.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/* If you need to extend the base class, then declare your processor specific
class here. Your processor specific functions should all use this type in place 
of the base class type. */
typedef struct Foo_MCU1Tag
{
    Foo *super; // include the base class first
    
    /* Add any processor specific variables you need here */
    
} Foo_MCU1;

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
 * @brief Connects the sub class to the base class
 * 
 * @param self
 * @param base
 */
void Foo_MCU1_Create(Foo_MCU1 *self, Foo *base);

/***************************************************************************//**
 * @brief
 * 
 * @param self 
 * @param data 
 */
void Foo_MCU1_ProcessorSpecificNonInterfaceFunc(Foo_MCU1 *self, uint16_t data);

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 */
void Foo_MCU1_Func(Foo_MCU1 *self);

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 * @return uint16_t 
 */
uint16_t Foo_MCU1_GetValue(Foo_MCU1 *self);

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 * @param data 
 */
void Foo_MCU1_SetValue(Foo_MCU1 *self, uint16_t data);

#endif /* I2C_TARGET_H */
