/***************************************************************************//**
 * @brief Filter Library Implementation (EMA Filter)
 * 
 * @file Filter_EMA.c
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 12/18/22  Original creation
 * 
 * @details
 *      TODO
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

#include "Filter_EMA.h"

// ***** Defines ***************************************************************

#define DEFAULT_ALPHA 0.2
#define ALPHA_U16(x) ((uint16_t)(x * 65535))

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

    if(alpha < 0)
        alpha = DEFAULT_ALPHA;

    self->alphaU16 = ALPHA_U16(alpha);
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
    make the filter roll off earlier. A value of 0 allows no output at all. A
    value of 1.0 applies no filtering.
    
    y[i] = x[i] * alpha + y[i - 1] * (1 - alpha)
    alpha = dt / (RC + dt)

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