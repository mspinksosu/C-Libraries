/***************************************************************************//**
 * @brief Foo Interface
 * 
 * @file IFoo.c
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 12/2/14   Original creation
 * @date 8/6/22    Modified
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

#include "IFoo.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************


// ***** Static Function Prototypes ********************************************


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void Foo_Create(Foo *self, void *instanceOfSubclass, Foo_Interface *interface)
{
    self->instance = instanceOfSubclass;
    self->interface = interface;

    /* @note This is a method for setting the pointer to the super class called 
    type-punning. This only works if the super class pointer is the first 
    member of the sub class struct. Since we know the first member of the sub 
    class is going to be a pointer to a Foo, we can cast the instance pointer 
    as a pointer to a *Foo. */
    // Foo **ptrToSuper = (Foo**)instanceOfSubClass;
    // *ptrToSuper = self;

    /* I prefer to set the pointer within a subclass Create function instead 
    because it is much safer. If the first member is not a pointer to the base 
    class, that method will still work. @see Foo_MCU1_Create */
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void Foo_Func(Foo *self)
{
    /* Check the function table is set up properly before calling the processor
    specific function */
    if(self->interface->Foo_Func != NULL && self->instance != NULL)
    {
        /* Dispatch the function using indirection */
        (self->interface->Foo_Func)(self->instance);
    }
}

// *****************************************************************************

uint16_t Foo_GetValue(Foo *self)
{
    if(self->interface->Foo_GetValue != NULL && self->instance != NULL)
    {
        return (self->interface->Foo_GetValue)(self->instance);
    }
    else
    {
        return 0;
    }
}

// *****************************************************************************

void Foo_SetValue(Foo *self, uint16_t data)
{
    if(self->interface->Foo_SetValue != NULL && self->instance != NULL)
    {
        (self->interface->Foo_SetValue)(self->instance, data);
    }
}

/*
 End of File
 */