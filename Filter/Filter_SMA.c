/***************************************************************************//**
 * @brief Foo Library Implementation (MCU1)
 * 
 * @author Matthew Spinks
 * 
 * @date 12/2/14   Original creation
 * @date 2/4/22    Modified
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
FooInterface FooFunctionTable = {
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

// TODO
uint16_t Filter_SMA_ComputeU16(Filter *self, uint16_t input)
{
    /* @note This is a type of filter called a simple moving average filter.
    It makes a buffer of samples, and averages the samples. There is one clever
    trick. There are only two values in the sum that change. Instead of summing
    the buffer each loop, we subtract from the sum, the current value in the 
    buffer we are looking to replace. Then add the new input into the buffer
    and the sum. - MS */
    static uint8_t smaIndex = 0;
    static uint32_t sum = 0;
    static uint32_t buffer[AVERAGE_LENGTH];
    uint16_t output;

    sum -= buffer[smaIndex];
    sum += input;
    buffer[smaIndex] = input;
    output = sum / AVERAGE_LENGTH;

    smaIndex++;
    if(smaIndex == AVERAGE_LENGTH)
        smaIndex = 0;
}

// TODO
#define ALPHA_16U(x) ((uint16_t)(x * 65535))
uint16_t Filter_EMA_ComputeU16(Filter *self, uint16_t input)
{
    static uint16_t prevOutput;
    uint16_t output;
    /* @note Another experiment with different type of filter called an
    exponential moving average filter, which is supposed to mimic a RC filter
    and uses less memory. There is value called alpha that goes from 0 to 1.0. 
    Values closer to 0 make the filter more aggressive. I converted the alpha 
    value to a 16-bit number. I also used 16-bit inputs instead of 32. Then 
    I shift the output back to a 16-bit number on the last step. */
    static uint16_t alpha = ALPHA_16U(0.1);
    uint32_t tmp = input * alpha + prevOutput * (65536 - alpha);
    /* round the 32-bit number before converting to 16-bit */
    output = (tmp + 0x8000) >> 16;
}
/*
 End of File
 */