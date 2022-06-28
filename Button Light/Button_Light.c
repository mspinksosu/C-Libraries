/***************************************************************************//**
 * @brief Button Library (Light)
 * 
 * @author  Matthew Spinks
 * 
 * @date 6/28/22     Original creation
 * 
 * @file Button_Light.c
 * 
 * @details
 *      A stripped down version of my original button library with less
 * features. In order to try and save a little bit of memory, buttons are 
 * grouped in eight. There are no analog button options, no long press feature,
 * and the debounce length is used for both the press and release. This library 
 * can also be used for things like inserting connectors or switches. Anything 
 * that requires debouncing. 
 * 
 *      To create a group of buttons, initialize the object with the debounce
 * time and the sample rate in milliseconds.
 * 
 *      When you check for a press or release event, the flag for that event 
 * is not automatically cleared. This is for you to decide how to handle it. 
 * Normally, I clear the flag right after checking for an event, but there may 
 * be times when you don't want to do that.
 * 
 * Example Code:
 *
 ******************************************************************************/

#include "Button_Light.h"

// ***** Defines ***************************************************************


// ***** Function Prototypes ***************************************************


// ***** Global Variables ******************************************************


void ButtonGroup_Init(ButtonGroup *self, uint16_t debounceMs, uint16_t tickMs)
{
    if(tickMs != 0)
    {
        self->debouncePeriod = debounceMs / tickMs;
    }
}

/***************************************************************************//**
 * @brief Update the Button Group based on the input signals
 * 
 * You must call this function at a periodic rate.
 * 
 * @param self  pointer to the Button Group that you are using
 * 
 */
void BG_Tick(ButtonGroup *self)
{
    /* Update inputs */
    for(uint8_t i = 0; i <= 7; i++)
    {
        if(self->input & (1 << i))
        {
            if(self->integrator[i] < self->debouncePeriod)
                self->integrator[i]++;
            else
                self->integrator[i] = self->debouncePeriod; // maximum
        }
        else if(self->integrator[i] > 0)
        {
            self->integrator[i]--;
        }
    }

    /* Update outputs */
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

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 * 
 * @param index  index 0 - 7
 * 
 * @param isPressed 
 */
void BG_SetButtonValue(ButtonGroup *self, uint8_t index, bool isPressed)
{
    if(index > 7)
        return;
    
    if(isPressed)
        self->input |= (1 << index);
    else
        self->input &= ~(1 << index);
}

bool BG_GetPress(ButtonGroup *self, uint8_t index)
{
    if(index > 7)
        return false;

    if(self->pressed & (1 << index))
        return true;
    else
        return false;
}

void BG_ClearPressFlag(ButtonGroup *self, uint8_t index)
{

}

/*
 End of File
 */
