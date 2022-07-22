/***************************************************************************//**
 * @brief Foo Library Implementation Header (MCU1)
 * 
 * @author Matthew Spinks
 * 
 * @date 12/2/14  Original creation
 * @date 2/4/22   Modified
 * 
 * @file Foo_MCU1.h
 * 
 * @details
 *      TODO
 * 
 ******************************************************************************/

#ifndef FOO_H
#define FOO_H

#include "IFoo.h"

/* Include processor specific header files here if needed */
#include "mcu1_gpio.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/* Create your processor specific class. */
typedef struct Foo_MCU1Tag
{
    Foo *super; // include the base class first
    
    /* Add any processor specific variables you need here */
    
} Foo_MCU1;

// ***** Function Prototypes ***************************************************

void Foo_MCU1_Init(Foo *self, uint16_t data);

void Foo_MCU1_ProcessorSpecificFunc(void);

// ----- Interface Functions ---------------------------------------------------

void Foo_MCU1_Func(Foo_MCU1 *self);

uint16_t Foo_MCU1_GetValue(Foo_MCU1 *self);

void Foo_MCU1_SetValue(Foo_MCU1 *self, uint16_t data);

#endif  /* FOO_H */