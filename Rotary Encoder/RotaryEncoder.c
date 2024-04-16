/***************************************************************************//**
 * @brief Rotary Encoder Library Header
 * 
 * @file RotaryEncoder.c
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 10/2/16   Original Creation
 * @date 2/21/22   Added Doxygen
 * @date 7/2/22    Redesigned to add different encoders and debouncing
 * 
 * @details
 *      A library that handles the basic quadrature rotary encoder. A flag is
 * set every time the knob moves one "click" or detent either clockwise or 
 * counterclockwise. You check for these events by calling the get clockwise 
 * or counterclockwise functions. The flags for these functions are cleared 
 * automatically whenever they are called.
 * 
 * This library uses a table to determine what state the rotary encoder is in. 
 * This is based on the value given by the update phases function.
 * 
 * @section license License
 * SPDX-FileCopyrightText: © 2016 Matthew Spinks
 * SPDX-License-Identifier: Zlib
 * 
 * This software is released under the Zlib license. You are free alter and
 * redistribute it, but you must not misrepresent the origin of the software.
 * This notice may not be removed. <http://www.zlib.net/zlib_license.html>
 * 
 * ****************************************************************************/

#include "RotaryEncoder.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/* The previous value of the phases is shifted left and combined with the
current state. Using this, we can determine which direction we are going. The
value of the output will look like this: 0000baba. The index of the table 
corresponds to these values. A positive number indicates a clockwise transition
and a negative number indicates a counter clockwise transition. A zero means
there was no transition, or that it was invalid. */
static int8_t rotaryLookupTable[] = { 0, 1, -1, 0, -1, 0, 0, 1,
                                      1, 0, 0, -1, 0, -1, 1, 0 };

// ***** Static Function Prototypes ********************************************


// *****************************************************************************

void RE_Init(RotaryEncoder *self, uint16_t debounceMs, uint16_t tickMs)
{
    RE_InitWithType(self, RE_HALF_CYCLE_PER_DETENT, debounceMs, tickMs);
}

// *****************************************************************************

void RE_InitWithType(RotaryEncoder *self, RotaryEncoderType type, uint16_t debounceMs, uint16_t tickMs)
{
    uint16_t debouncePeriod;
    
    /* The rotary encoder type mask will tell us how often we need to change
    the output depending on the type of rotary encoder. There will be a state
    change every 1/4 cycle. But the detent will only be at either every 1/4,
    1/2, or full cycle */
    switch(type)
    {
        case RE_HALF_CYCLE_PER_DETENT:
            self->typeMask = 0x01; // multiples of two
            break;
        case RE_FULL_CYCLE_PER_DETENT:
            self->typeMask = 0x03; // multiples of four
            break;
        default:
            self->typeMask = 0; // every state change (1/4 cycle per detent)
            break;
    }

    if(tickMs != 0)
    {
        debouncePeriod = debounceMs / tickMs;
    }

    /* If you are using an RC filter to debounce, the debounce period should be
    zero. However, this type of debouncing algorithm requires a non-zero number 
    for its minimum. Using 1 will simply toggle the output every time the input 
    changes. For the maximum, I limit it to 8-bits. If you need more than 
    255 ms to debounce your rotary encoder, you've got some serious issues. */
    if(debouncePeriod == 0)
        self->debouncePeriod = 1;
    else if(debouncePeriod > 255)
        debouncePeriod = 255;
    
    self->debouncePeriod = (uint8_t)debouncePeriod;
    self->phaseAIntegrator = 0;
    self->phaseBIntegrator = 0;
    self->state = 0;
    self->output = 0;
    self->flags.all = 0;
}

// *****************************************************************************

void RE_Tick(RotaryEncoder *self, bool AisHigh, bool BisHigh)
{
    /* First update the inputs */
    if(AisHigh)
    {
        if(self->phaseAIntegrator < self->debouncePeriod)
        {
            self->phaseAIntegrator++;
        }
        else
        {
            /* Prevent from going over maximum value */
            self->phaseAIntegrator = self->debouncePeriod;
        }
    }
    else if(self->phaseAIntegrator > 0)
    {
        /* Phase A low */
        self->phaseAIntegrator--;
    }

    if(BisHigh)
    {
        if(self->phaseBIntegrator < self->debouncePeriod)
        {
            self->phaseBIntegrator++;
        }
        else
        {
            /* Prevent from going over maximum value */
            self->phaseBIntegrator = self->debouncePeriod;
        }
    }
    else if(self->phaseBIntegrator > 0)
    {
        /* Phase B low */
        self->phaseBIntegrator--;
    }

    /* Update the integrator outputs. Shifting the previous state left will 
    allow us to see the output from the previous state and the current state 
    together. Phase A will be encoded as bit 0, and and phase B as bit 1. 
    So the byte will look like this: 0000baba */
    self->state = (self->state << 2) & 0x0C;

    /* Update the outputs */
    if(self->phaseAIntegrator == 0)
    {
        self->state &= ~0x01; // clear phase A (bit 0)
    }
    else if(self->phaseAIntegrator >= self->debouncePeriod)
    {
        self->state |= 0x01; // set phase A (bit 0)
    }
    
    if(self->phaseBIntegrator == 0)
    {
        self->state &= ~0x02; // clear phase B (bit 1)
    }
    else if(self->phaseBIntegrator >= self->debouncePeriod)
    {
        self->state |= 0x02; // set phase B (bit 1)
    }
    
    /* Decode the output using the state table. A state transition will occur
    every quarter cycle. A positive number indicates a clockwise transition
    and a negative number indicates a counter clockwise transition. A zero 
    means there was no transition, or that it was invalid. */
    int8_t newOutput = rotaryLookupTable[self->state];

    /* Detect direction reversal. A negative to positive or a positive to 
    negative changes the sign and resets the count */
    if((newOutput == 1 && self->output < 0) || (newOutput == -1 && self->output >= 0))
    {
        self->output = newOutput;
    }
    else
    {
        self->output += newOutput;

        /* Prevent overflow and underflow. If the output was at 127 and going 
        in the postive direction, it will be at -128. Roll it to 0. If the 
        output was at -128 and going in the negative direction, it will be at 
        127. Roll it to -1. */
        if(newOutput == 1)
            self->output &= 0x7F;
        
        if(newOutput == -1)
            self->output |= 0x80;
    }
    
    /* The typemask will cause an event to occur every quarter, half, or full
    cycle depending on the type of rotary encoder. It is based on modulo
    division. */
    if((self->output & self->typeMask) == 0)
    {
        if(newOutput == 1)
        {
            self->flags.clockwise = 1;
            if(self->clockwiseEventCallback)
                self->clockwiseEventCallback(self);
        }
        else if(newOutput == -1)
        {
            self->flags.counterClockwise = 1;
            if(self->counterClockwiseEventCallback)
                self->counterClockwiseEventCallback(self);
        }
    }
}

// *****************************************************************************

bool RE_GetClockwise(RotaryEncoder *self)
{
    bool retVal = false;

    if(self->flags.clockwise)
        retVal = true;
    
    self->flags.clockwise = 0;
    return retVal;
}

// *****************************************************************************

bool RE_GetCounterClockwise(RotaryEncoder *self)
{
    bool retVal = false;

    if(self->flags.counterClockwise)
        retVal = true;
    
    self->flags.counterClockwise = 0;
    return retVal;
}

// *****************************************************************************

void RE_SetClockwiseEventCallback(RotaryEncoder *self, RECallbackFunc Function)
{
    self->clockwiseEventCallback = Function;
}

// *****************************************************************************

void RE_SetCounterClockwiseEventCallback(RotaryEncoder *self, RECallbackFunc Function)
{
    self->counterClockwiseEventCallback = Function;
}

/*
 End of File
 */