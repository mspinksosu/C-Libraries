/***************************************************************************//**
 * @brief Foo Interface
 * 
 * @author Matthew Spinks
 * 
 * @date 12/2/14  Original creation
 * @date 8/6/22   Modified
 * 
 * @file IFoo.c
 * 
 * @details
 *      TODO
 * 
 ******************************************************************************/

#include "IFoo.h"
#include <stdio.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

static uint8_t str[10];

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

void LCD_PutInt(LCD *self, int16_t num, uint8_t width)
{
    if(self->interface->LCD_PutString != NULL && self->instance != NULL)
    {
        int n = sprintf(str, "%*d", width, num);
        
        if(n > 0)
            (self->interface->LCD_PutString)(str);
    }
}

void LCD_PutFloat(LCD *self, float num, uint8_t precision)
{
    if(self->interface->LCD_PutString != NULL && self->instance != NULL)
    {
        float round = 0.5f;
        int n = 0;

        if(precision > 6)
            precision = 6;
        
        for(uint8_t i = 0; i < precision; i++) round = round / 10.0f;
        
        if(num < 0.0)
        {
            /* If < 0, add 3 places for minus sign, 0, and decimal point */
            num -= round;
            n = sprintf(str, "%0*.*f", precision + 3, precision, num);
        }
        else
        {
            num += round;
            n = sprintf(str, "%.*f", precision, num);
        }
        if(n > 0)
            (self->interface->LCD_PutString)(str);
    }
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