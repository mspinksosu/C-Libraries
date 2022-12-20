/***************************************************************************//**
 * @brief Filter Library Implementation (EMA Filter)
 * 
 * @author Matthew Spinks
 * 
 * @date 12/2/14   Original creation
 * @date 2/4/22    Modified
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
    functions. Typecasting is necessary. When a new sub class object is 
    created, we will set its interface member equal to this table. */
FilterInterface FilterFunctionTable = {
    .Filter_ComputeU16 = (uint16_t (*)(void *, uint16_t))Filter_EMA_ComputeU16,
};

// ***** Static Function Prototypes ********************************************

/* Put static function prototypes here */


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void Filter_EMA_Create(Filter_EMA *self, Filter *base)
{
    self->super = base;

    /*  Call the base class constructor */
    Foo_Create(base, self, &FilterFunctionTable);
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

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