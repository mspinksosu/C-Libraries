/***************************************************************************//**
 * @brief Button Group Library
 * 
 * @author  Matthew Spinks
 * 
 * @date 6/25/22     Original creation
 * 
 * @file ButtonGroup.c
 * 
 * @details
 *      This version of the debounce code is based on an alogrithm that has
 * been around for many years. It uses a very simple counter to act as an 
 * integrator. When the integrator reaches either 0 or the maximum value, the 
 * output state is changed.
 * 
 *      I took this simple approach and added a few extra steps. At the end I
 * check the previous state of the output to determine rising and falling 
 * transitions (pressed or released). I also added my usual object oriented
 * approach that I use for all of my C libraries. Every function uses a pointer
 * called self as its first argument. This is the pointer to the Button Group 
 * object you are currently using.
 * 
 * Button are combined into groups of eight. Check the header file for some
 * useful preprocessor macros you can use to decode the buttons more easily.
 * 
 ******************************************************************************/

#include "ButtonGroup.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************


// ***** Static Function Prototypes ********************************************


// *****************************************************************************

void BG_Init(ButtonGroup *self, uint16_t debounceMs, uint16_t tickMs)
{
    if(tickMs != 0)
    {
        self->debouncePeriod = debounceMs / tickMs;
    }

    if(self->debouncePeriod == 0)
    {
        self->debouncePeriod = 1;
    }

    self->output = 0;
    self->previousOutput = 0;
    self->pressed = 0;
    self->released = 0;
}

// *****************************************************************************

void BG_Tick(ButtonGroup *self)
{
    /* Update the inputs */
    for(uint8_t i = 0; i <= 7; i++)
    {
        if(self->input & (1 << i))
        {
            if(self->integrator[i] < self->debouncePeriod)
            {
                self->integrator[i]++;
            }
            else
            {
                /* Prevent from going over maximum value */
                self->integrator[i] = self->debouncePeriod;
            }
        }
        else 
        {
            if(self->integrator[i] > 0)
                self->integrator[i]--;
        }
    }

    /* Update the outputs. The output only changes if the integrator hits the 
    minimum or maximum value */
    for(uint8_t i = 0; i <= 7; i++)
    {
        if(self->integrator[i] == 0)
        {
            self->output &= ~(1 << i);
        }
        else if(self->integrator[i] >= self->debouncePeriod)
        {
            self->output |= (1 << i);
        }
    }

    /* Check the outputs for button presses and releases */
    uint8_t change = (self->previousOutput ^ self->output);
    self->pressed |= (self->output & change);
    self->released |= (~(self->output) & change);
    self->previousOutput = self->output;
}

// *****************************************************************************

void BG_UpdateButtonValue(ButtonGroup *self, uint8_t index, bool isPressed)
{
    if(index > 7)
        return;
    
    if(isPressed)
        self->input |= (1 << index);
    else
        self->input &= ~(1 << index);
}

// *****************************************************************************

bool BG_GetPress(ButtonGroup *self, uint8_t index)
{
    if(index > 7)
        return false;

    if(self->pressed & (1 << index))
        return true;
    else
        return false;
}

// *****************************************************************************

void BG_ClearPressFlag(ButtonGroup *self, uint8_t index)
{
    if(index > 7)
        return;

    self->pressed &= ~(1 << index);
}

// *****************************************************************************

bool BG_GetRelease(ButtonGroup *self, uint8_t index)
{
    if(index > 7)
        return false;

    if(self->released & (1 << index))
        return true;
    else
        return false;
}

// *****************************************************************************

void BG_ClearReleaseFlag(ButtonGroup *self, uint8_t index)
{
    if(index > 7)
        return;

    self->released &= ~(1 << index);
}

// *****************************************************************************

uint8_t BG_GetButtonOutput(ButtonGroup *self, uint8_t index)
{
    if(index > 7)
        return 0;
        
    if(self->output & (1 << index))
        return 1;
    else
        return 0;
}

// *****************************************************************************

uint8_t BG_GetButtonGroupOutput(ButtonGroup *self)
{
    return self->output;
}

/*
 End of File
 */
