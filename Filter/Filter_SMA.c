/***************************************************************************//**
 * @brief Filter Library Implementation (SMA Filter)
 * 
 * @file Filter_SMA.c
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

#include "Filter_SMA.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/*  Declare an interface struct and initialize its members the our local 
    functions. */
FilterInterface FilterFunctionTable = {
    .Filter_ComputeU16 = (uint16_t (*)(void *, uint16_t))Filter_SMA_ComputeU16,
};

// ***** Static Function Prototypes ********************************************


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void Filter_SMA_Create(Filter_SMA *self, Filter *base, uint16_t *buffer, uint8_t bufferLength)
{
    self->super = base;
    self->buffer = buffer;
    self->bufferLength = bufferLength;
    self->index = 0;
    /*  Call the base class constructor */
    Filter_Create(base, self, &FilterFunctionTable);
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

uint16_t Filter_SMA_ComputeU16(Filter_SMA *self, uint16_t input)
{
    if(self->buffer == NULL || self->bufferLength == 0)
        return;
    
    /* This is a type of filter called a simple moving average filter.
    It makes a buffer of samples, and averages the samples. There is one clever
    trick. There are only two values in the buffer that change; the newest 
    value and the oldest value. So, rather than summing the entire buffer each 
    loop, we subtract from the sum the current value in the buffer we are at, 
    then add the new input to the sum. */
    uint16_t output;

    self->sum -= self->buffer[self->index];
    self->sum += input;
    self->buffer[self->index] = input;
    output = self->sum / self->bufferLength;

    self->index++;
    if(self->index == self->bufferLength)
        self->index = 0;

    return output;
}

/*
 End of File
 */