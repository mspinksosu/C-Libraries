/***************************************************************************//**
 * @brief I2C Target Implementation (Non-Processor Specific)
 * 
 * @file I2CTarget.c
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

#include "TargetDevice_I2C.h" // @todo change name to something that includes data transfer buffer? I2CTarget_DT? - MS
#include <stddef.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/*  The sub class must implement the functions provided in the interface. In 
    this case we are declaring an interface struct and initializing its members 
    (which are function pointers) the our local functions. Typecasting is 
    necessary. When a new sub class object is created, we will set its interface
    member equal to this table. */
TargetDeviceInterface FooFunctionTable = {
    .TargetDevice_Init = (void (*)(void *))Foo_MCU1_Func,
};

// ***** Static Function Prototypes ********************************************

/* Put static function prototypes here */


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void Foo_MCU1_Create(Foo_MCU1 *self, Foo *base)
{
    self->super = base;

    /* Do the rest of the initialization for struct members etc. */

    /*  Call the base class constructor. What you are doing is connecting the 
    base class's instance member to the instance of the sub class that you just 
    created, along with the list of functions that will be used. From now on,
    you'll be able to use the base class for function calls */
    Foo_Create(base, self, &FooFunctionTable);
}

// *****************************************************************************

void Foo_MCU1_ProcessorSpecificNonInterfaceFunc(Foo_MCU1 *self, uint16_t data)
{
    /* Optional function to do some specific initialization or whatever else
    you want to do */
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/* Don't forget to add these functions to the function table */

void Foo_MCU1_Func(Foo *self)
{

}

// *****************************************************************************

uint16_t Foo_MCU1_GetValue(Foo *self)
{

}

// *****************************************************************************

void Foo_MCU1_SetValue(Foo *self, uint16_t data)
{

}

/*
 End of File
 */
