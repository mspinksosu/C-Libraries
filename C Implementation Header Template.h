/***************************************************************************//**
 * @brief Foo Library Implementation Header (MCU1)
 * 
 * @file Foo_MCU1.h
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 12/2/14   Original creation
 * @date 2/4/22    Modified
 * 
 * @details
 *      TODO
 * 
 * @section license License
 * SPDX-FileCopyrightText: © 2019 Matthew Spinks
 * SPDX-License-Identifier: Zlib
 * 
 * This software is released under the Zlib license. You are free alter and
 * redistribute it, but you must not misrepresent the origin of the software.
 * This notice may not be removed. <http://www.zlib.net/zlib_license.html>
 * 
 ******************************************************************************/

#ifndef FOO_MCU1_H
#define FOO_MCU1_H

#include "IFoo.h"

/* Include processor specific header files here if needed */
#include "mcu1_gpio.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/* Declare and define this variable in your implementation's .c file */
extern FooInterface MCU1_FunctionTable;

/* If you need to extend the base class, then declare your processor specific
class here. Your processor specific functions should all use this type in place 
of the base class type. */
typedef struct Foo_MCU1Tag
{
    Foo *super; // include the base class first
    
    /* Add any processor specific variables you need here */
    
} Foo_MCU1;

typedef struct FooInitType_MCU1Tag
{
    FooInitType *super; // include the base class first

    /* Add any processor specific variables you need here */
} FooInitType_MCU1;

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
 * @param base 
 */
void Foo_MCU1_CreateInitType(FooInitType_MCU1 *self, FooInitType *base);

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
 * @param params 
 */
void Foo_MCU1_Init(Foo_MCU1 *self, FooInitType_MCU1 *params);

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

#endif /* FOO_MCU1_H */
