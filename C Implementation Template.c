/***************************************************************************//**
 * @brief Foo Library Implementation (MCU1)
 * 
 * @author Matthew Spinks
 * 
 * @date 12/2/14  Original creation
 * @date 2/4/22   Modified
 * 
 * @file Foo_MCU1.c
 * 
 * @details
 *      TODO
 * 
 ******************************************************************************/

#include "Foo_MCU1.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/*  The sub class must implement the functions provided in the interface. In 
    this case we are declaring an interface struct and initializing its members 
    (which are function pointers) the our local functions. Typecasting is 
    necessary. When a new sub class object is created, we will set its interface
    member equal to this table. */
static Foo_Interface FooFunctionTable = {
    .Foo_Func = (void (*)(void *))Foo_MCU1_Func,
    .Foo_GetValue = (uint16_t (*)(void *))Foo_MCU1_GetValue,
    .Foo_SetValue = (void (*)(void *, uint16_t))Foo_MCU1_SetValue,
    // Add the rest of the functions listed in the interface
};

// ***** Static Function Prototypes ********************************************

/* Put static function prototypes here */


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void Foo_MCU1_Init(Foo_MCU1 *self, Foo *base, uint16_t data)
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

void Foo_MCU1_ProcessorSpecificFunc(void)
{

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