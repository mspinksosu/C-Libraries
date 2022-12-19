/***************************************************************************//**
 * @brief Foo Interface
 * 
 * @author Matthew Spinks
 * 
 * @date 12/2/14   Original creation
 * @date 8/6/22    Modified
 * 
 * @file IFoo.c
 * 
 * @details
 *      TODO
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

void Foo_Create(Foo *self, void *instanceOfSubClass, Foo_Interface *interface)
{
    self->instance = instanceOfSubClass;
    self->interface = interface;
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