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

void GPIO_Create(GPIO *self, void *instanceOfSubClass)
{
    self->instance = instanceOfSubClass;
}

// *****************************************************************************

void GPIO_CreateInitType(GPIOInitType *params, void *instanceOfSubClass)
{
    params->instance = instanceOfSubClass;
}

// *****************************************************************************

void GPIO_SetPointerToAnalogValue(GPIO *self, uint16_t *ptr)
{
    self->ptrToAnalogValue = ptr;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void GPIO_InitPin(GPIO *self, GPIOInitType *params)
{
    if(interface->GPIO_InitPin != NULL && self->instance != NULL 
        && params->instance != NULL)
    {
        (interface->GPIO_InitPin)(self->instance, params->instance);
    }
}

// *****************************************************************************

void GPIO_SetPin(GPIO *self)
{
    if(interface->GPIO_SetPin != NULL && self->instance != NULL)
    {
        (interface->GPIO_SetPin)(self->instance);
    }
}

// *****************************************************************************

void GPIO_ClearPin(GPIO *self)
{
    if(interface->GPIO_ClearPin != NULL && self->instance != NULL)
    {
        (interface->GPIO_ClearPin)(self->instance);
    }
}

// *****************************************************************************

void GPIO_InvertPin(GPIO *self)
{
    if(interface->GPIO_InvertPin != NULL && self->instance != NULL)
    {
        (interface->GPIO_InvertPin)(self->instance);
    }
}

// *****************************************************************************

void GPIO_WritePin(GPIO *self, bool setPinHigh)
{
    if(interface->GPIO_WritePin != NULL && self->instance != NULL)
    {
        (interface->GPIO_WritePin)(self->instance, setPinHigh);
    }
}

// *****************************************************************************

uint16_t GPIO_ReadPin(GPIO *self)
{
    if(interface->GPIO_ReadPin != NULL && self->instance != NULL)
    {
        return (interface->GPIO_ReadPin)(self->instance);
    }
    else
    {
        return 0;
    }
}

// *****************************************************************************

bool GPIO_ReadBool(GPIO *self)
{
    if(interface->GPIO_ReadBool != NULL && self->instance != NULL)
    {
        return (interface->GPIO_ReadBool)(self->instance);
    }
    else
    {
        return false;
    }
}

// *****************************************************************************

void GPIO_SetType(GPIO *self, GPIOType type)
{
    if(interface->GPIO_SetType != NULL && self->instance != NULL)
    {
        (interface->GPIO_SetType)(self->instance, type);
    }
}

// *****************************************************************************

GPIOType GPIO_GetType(GPIO *self)
{
    if(interface->GPIO_GetType != NULL && self->instance != NULL)
    {
        return (interface->GPIO_GetType)(self->instance);
    }
    else
    {
        return 0;
    }
}

// *****************************************************************************

void GPIO_SetPull(GPIO *self, GPIOPull pullType)
{
    if(interface->GPIO_SetPull != NULL && self->instance != NULL)
    {
        (interface->GPIO_SetPull)(self->instance, pullType);
    }
}

// *****************************************************************************

GPIOPull GPIO_GetPull(GPIO *self)
{
    if(interface->GPIO_GetPull != NULL && self->instance != NULL)
    {
        return (interface->GPIO_GetPull)(self->instance);
    }
    else
    {
        return 0;
    }
}

/*
 End of File
*/