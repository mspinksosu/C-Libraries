/***************************************************************************//**
 * @brief GPIO Interface
 * 
 * @file IGPIO.c
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 2/6/22    Original creation
 * 
 * @details
 *      The counterpart for the GPIO interface library. Functions here are
 * called using the base class object. When you use the create functions, you
 * are linking a base class and a subclass together. The functions listed in
 * the interface section will use the interface object and your base class 
 * object to determine which function implementation to call and which subclass
 * object to give to it.
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

#include "IGPIO.h"
#include <stddef.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

static GPIOInterface *interface;

// ***** Static Function Prototypes ********************************************


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void GPIO_DriverSetInterface(GPIOInterface *thisInterface)
{
    interface = thisInterface;
}

// *****************************************************************************

void GPIO_CreateInitType(GPIOInitType *params, void *instanceOfSubClass)
{
    params->instance = instanceOfSubClass;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions with Inheritance  *******************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void GPIO_InitPin(GPIOPort port, uint8_t pin, GPIOInitType *params)
{
    if(interface->GPIO_InitPin != NULL && params->instance != NULL)
    {
        (interface->GPIO_InitPin)(port, pin, params->instance);
    }
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Simple Interface Functions ******************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/* Implement the functions listed in the IGPIO.h under "Simple Interface 
Functions" in your own .c file exactly as written. For the file name, I usually 
name my .c file with an underscore prefix, such as GPIO_STM32G0.c or 
GPIO_PIC32MX.c. Don't forget to include IMCU.h. */

/*
 End of File
*/