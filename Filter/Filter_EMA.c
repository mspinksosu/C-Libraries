/***************************************************************************//**
 * @brief Filter Library Implementation (EMA Filter)
 * 
 * @author Matthew Spinks
 * 
 * @date 12/18/22  Original creation
 * 
 * @file Filter_EMA.c
 * 
 * @details
 *      TODO
 * 
 ******************************************************************************/

#include "Filter_EMA.h"

// ***** Defines ***************************************************************

#define ALPHA_16U(x) ((uint16_t)(x * 65535))

// ***** Global Variables ******************************************************

/*  Declare an interface struct and initialize its members the our local 
    functions. */
FilterInterface FilterFunctionTable = {
    .Filter_ComputeU16 = (uint16_t (*)(void *, uint16_t))Filter_EMA_ComputeU16,
};

// ***** Static Function Prototypes ********************************************


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void Filter_EMA_Create(Filter_EMA *self, Filter *base, float alpha)
{
    self->super = base;
    self->alphaU16 = ALPHA_16U(alpha);
    self->prevOutput = 0;
    /*  Call the base class constructor */
    Foo_Create(base, self, &FilterFunctionTable);
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

uint16_t Filter_EMA_ComputeU16(Filter_EMA *self, uint16_t input)
{
    uint32_t tmp;
    /* This filter is called an exponential moving average filter, which is 
    supposed to mimic a RC filter and uses less memory than an SMA filter. 
    There is value called alpha that goes from 0 to 1.0. Values closer to 0 
    make the filter more aggressive. 
    
    y[i] = x[i] * alpha + y[i - 1] * (1 - alpha)

    I converted the alpha value to a 16-bit number. Then I shift the output 
    from a 32-bit number to a 16-bit number on the last step. */
    tmp = input * self->alphaU16 + self->prevOutput * (65536 - self->alphaU16);
    
    /* This will round the 32-bit number before converting to 16-bit by adding 
    "one half" of a 16-bit number (0x8000). The principle is the same for
    decimal numbers i.e. adding 0.5. */
    return ((tmp + 0x8000) >> 16);
}

/*
 End of File
 */