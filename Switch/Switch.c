/***************************************************************************//**
 * @brief Switch Library
 * 
 * @file Switch.c
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 1/5/22    Original creation
 * 
 * @details
 *      A library that handles basic features of a "dual throw" switch. There
 * are two inputs and one output. The output A and B cannot be both true. 
 * Optional "center off" that allows output to be off. The default is to not 
 * allow the output to be off.
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

#include "Switch.h"

// ***** Defines ***************************************************************

#define OUTPUT_A_BIT    0
#define OUTPUT_B_BIT    1

// ***** Global Variables ******************************************************


// ***** Static Function Prototypes ********************************************

static void DecodeOutput(Switch *self, SwitchState newState);

// *****************************************************************************

void Switch_Init(Switch *self, uint16_t debounceMs, uint16_t tickMs, bool inputAInit, bool inputBInit, bool centerOffAllowed)
{
    if(centerOffAllowed)
        self->type = SW_CENTER_OFF;
    else
        self->type = SW_NORMAL;

    if(tickMs != 0)
        self->debouncePeriod = debounceMs / tickMs;

    // Check for invalid initial state
    SwitchState initState = (SwitchState)((inputAInit << OUTPUT_A_BIT) | (inputBInit << OUTPUT_B_BIT));

    if(initState == SW_OUTPUT_OFF && centerOffAllowed == false)
    {
        initState = SW_OUTPUT_INVALID;
    }
    self->outputState = initState;
    self->debounceState = SW_READY;
    self->flags.all = 0;
}

// *****************************************************************************

void Switch_SetCenterOff(Switch *self, bool centerOffAllowed)
{
    if(centerOffAllowed)
        self->type = SW_CENTER_OFF;
    else
        self->type = SW_NORMAL;
}

// *****************************************************************************

void Switch_Tick(Switch *self, bool inputAIsOn, bool inputBIsOn)
{
    SwitchState currentState = (SwitchState)((inputAIsOn << OUTPUT_A_BIT) | (inputBIsOn << OUTPUT_B_BIT));

    switch(self->debounceState)
    {
        case SW_READY:
            if(self->outputState != currentState)
            {
                if(self->debouncePeriod == 0)
                {
                    /* If the debounce period is zero, we assume that the
                    debouncing is being done via hardware */

                    /* Output changed. Decode the output, set any flags, and
                    update the current output. */
                    DecodeOutput(self, currentState);
                    self->outputState = currentState;
                    
                    /* callback function */
                    if(self->flags.switchChangedEvent && self->outputChangeCallback)
                    {
                        (self->outputChangeCallback)(Switch_GetOuput(self), self);
                    }
                }
                else
                {
                    // Else, the input has changed. We need to debounce
                    self->debounceState = SW_DEBOUNCING;
                    self->debounceCounter = 0;
                }
            }
            break;
        case SW_DEBOUNCING:
            self->debounceCounter++;
            if(self->debounceCounter == self->debouncePeriod)
            {
                /* We have finished debouncing. Decode the output, set any 
                flags, update the current output. */
                DecodeOutput(self, currentState);
                self->outputState = currentState;
                self->debounceState = SW_READY;

                /* callback function */
                if(self->flags.switchChangedEvent && self->outputChangeCallback)
                {
                    (self->outputChangeCallback)(Switch_GetOuput(self), self);
                }
            }
            break;
        default:
                self->outputState = SW_OUTPUT_INVALID;
            break;
    }
}

// *****************************************************************************

bool Switch_GetOutputAOnEvent(Switch *self)
{
    if(self->flags.outputAOnEvent)
        return true;
    else
        return false;
}

// *****************************************************************************

void Switch_ClearOutputAOnEventFlag(Switch *self)
{
    self->flags.outputAOnEvent = 0;
}

// *****************************************************************************

bool Switch_GetOutputBOnEvent(Switch *self)
{
    if(self->flags.outputBOnEvent)
        return true;
    else
        return false;
}

// *****************************************************************************

void Switch_ClearOutputBOnEventFlag(Switch *self)
{
    self->flags.outputBOnEvent = 0;
}

// *****************************************************************************

bool Switch_GetOffEvent(Switch *self)
{
    if(self->flags.outputOffEvent)
        return true;
    else
        return false;
}

// *****************************************************************************

void Switch_ClearOffEventFlag(Switch *self)
{
    self->flags.outputOffEvent = 0;
}

// *****************************************************************************

bool Switch_OutputChangeEvent(Switch *self)
{
    if(self->flags.switchChangedEvent)
        return true;
    else
        return false;
}

// *****************************************************************************

void Switch_ClearOutputChangeEventFlag(Switch *self)
{
    self->flags.switchChangedEvent = 0;
}

// *****************************************************************************

bool Switch_IsOff(Switch *self)
{
    if(self->type == SW_CENTER_OFF && self->outputState == SW_OUTPUT_OFF)
        return true;
    else
        return false;
}

// *****************************************************************************

bool Switch_OutputA(Switch *self)
{
    if(self->outputState == SW_OUTPUT_A)
        return true;
    else
        return false;
}

// *****************************************************************************

bool Switch_OutputB(Switch *self)
{
    if(self->outputState == SW_OUTPUT_B)
        return true;
    else
        return false;
}

// *****************************************************************************

SwitchState Switch_GetOutput(Switch *self)
{
    SwitchState retVal = self->outputState;

    if(self->type == SW_NORMAL && retVal == SW_OUTPUT_OFF)
        retVal = SW_OUTPUT_INVALID;
    
    return retVal;
}

// *****************************************************************************

void Switch_SetOutputChangeCallback(Switch *self, SwitchCallbackFunc Function)
{
    self->outputChangeCallback = Function;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //  
// ***** Local Functions *****************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Set the event flags
 * 
 * @param self  pointer to the Switch that you are using
 * 
 * @param newState  an enum that holds the switch state
 */
static void DecodeOutput(Switch *self, SwitchState newState)
{
    if(self->outputState != newState)
    {
        self->flags.switchChangedEvent = 1;
    }
    uint8_t stateChange = (uint8_t)((self->outputState) ^ newState);

    /* TODO Should the event flags be triggered if moving from the invalid 
        state to a valid state? */

    // If output A has changed to high
    if((stateChange & (1 << OUTPUT_A_BIT)) && (newState & (1 << OUTPUT_A_BIT)))
    {
        self->flags.outputAOnEvent = 1;
    }

    // If output B has changed to high
    if((stateChange & (1 << OUTPUT_B_BIT)) && (newState & (1 << OUTPUT_B_BIT)))
    {
        self->flags.outputBOnEvent = 1;
    }

    // If the output has become zero
    if(stateChange != 0 && newState == SW_OUTPUT_OFF)
    {
        if(self->type == SW_CENTER_OFF)
        {
            self->flags.outputOffEvent = 1;
        }
    }
}

/*
 End of File
 */