/***************************************************************************//**
 * @brief Foo Interface
 * 
 * @author Matthew Spinks
 * 
 * @date 12/2/14  Original creation
 * @date 2/4/22   Modified
 * 
 * @file IFoo.c
 * 
 * @details
 *      TODO
 * 
 ******************************************************************************/

#include "IFoo.h"

// ***** Defines ***************************************************************


// ***** Function Prototypes ***************************************************


// ***** Global Variables ******************************************************


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Combine the base class, sub class, and function table
 * 
 * This function shouldn't be called directly. It is preferred that it is 
 * called from within the sub class constructor. This makes the function more 
 * type safe with the use of the void pointer
 * 
 * @param self 
 * @param instanceOfSubClass 
 * @param interface 
 */
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

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 */
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

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 * @return uint16_t 
 */
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

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 * @param data 
 */
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