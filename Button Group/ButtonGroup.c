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
 *      A stripped down version of my original button library with less
 * features. In order to try and save a little bit of memory, buttons are 
 * grouped in eight. There are no analog button options, no long press feature,
 * and the debounce length is used for both the press and release. This library 
 * can also be used for things like inserting connectors or switches. Anything 
 * that requires debouncing. This library is most useful if you have a large 
 * number of inputs that need debouncing.
 * 
 *      This version of the debounce code is based on an alogrithm that has
 * around for many years. It uses a very simple counter to act as integrator.
 * When the integrator reaches either 0 or the maximum, the output state is
 * changed.
 * 
 *      I took this simple approach and added a few extra steps. At the end I
 * check the previous state of the output to determine rising and falling 
 * transitions (pressed or released). I also added my usual object oriented
 * approach that I use for all of my C libraries. Every function uses a pointer
 * called self as its first argument. This is the pointer to the Button Group 
 * object you are currently using. 
 * 
 *      To create a group of buttons, initialize the object with the debounce
 * time and the sample rate in milliseconds.
 * 
 *      When you check for a press or release event, the flag for that event 
 * is not automatically cleared. This is for you to decide how to handle it. 
 * Normally, I clear the flag right after checking for an event, but there may 
 * be times when you don't want to do that.
 * 
 * If you have a large list of inputs you can use some preprocessor macros to 
 * help manage your inputs and their indexes. You can use an enum with the last
 * value being "TOTAL". By doing this, you can make an array of Button Group
 * objects. Example:
 * 
 * #define NUM_BUTTON_GROUPS    (TOTAL_BUTTONS / 8 + 1)
 * 
 * Here are some more useful macros:
 * 
 * #define InputGroup(x) (x >> 3) // divide by 8
 * #define InputBit(x) (x & 0x07) // mod 8
 * #define ButtonArrayDecode(array, x) array[InputGroup(x)], InputBit(x)
 * 
 * Example Usage:
 *      ButtonGroup oneButtonGroup;
 *      ButtonGroup arrayOfButtons[NUM_BUTTON_GROUPS];
 *      
 *      BG_Init(&oneButtonGroup, DEBOUNCE_MS, TICK_MS);
 * 
 *      for(i=0; i<NUM_BUTTON_GROUPS; i++) {
 *          BG_Init(&arrayOfButtons[i], DEBOUNCE_MS, TICKS_MS); }
 * 
 *      BG_UpdateButtonValue(&oneButtonGroup, INPUT1, true); // pressed
 *      
 *      BG_UpdateButtonValue(&arrayOfButtons[InputGroup(INPUT20)], 
 *          InputBit(INPUT20), true);
 * 
 *      BG_Tick(&oneButtonGroup);
 * 
 *      for(i=0; i<NUM_BUTTON_GROUPS; i++) {
 *          BG_Tick(&arrayOfButtons[i]); } // update all of the button groups
 *      
 *      if(BG_GetPress(ButtonArrayDecode(&arrayOfButtons, INPUT20)))
 *      { .... }
 * 
 ******************************************************************************/

#include "ButtonGroup.h"

// ***** Defines ***************************************************************


// ***** Function Prototypes ***************************************************


// ***** Global Variables ******************************************************

/***************************************************************************//**
 * @brief Initialize a Button Group object
 * 
 * @param self 
 * 
 * @param debounceMs 
 * 
 * @param tickMs 
 */
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
        else if(self->integrator[i] > 0)
        {
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

/***************************************************************************//**
 * @brief Update the value of the buttons in the Button Group
 * 
 * You must update the button values at a rate equal to or faster than the 
 * tick rate that you specified. You can call this function in the same area as
 * the tick function if you desire. It does not matter if the button is high or
 * low. Only that it is pressed or not pressed.
 * 
 * @param self   pointer to the Button Group that you are using
 * 
 * @param index  button index 0 - 7
 * 
 * @param isPressed  true if button is pressed.
 */
void BG_UpdateButtonValue(ButtonGroup *self, uint8_t index, bool isPressed)
{
    if(index > 7)
        return;
    
    if(isPressed)
        self->input |= (1 << index);
    else
        self->input &= ~(1 << index);
}

/***************************************************************************//**
 * @brief Check if a button was pressed
 * 
 * @param self  pointer to the Button Group that you are using
 * 
 * @param index  button index 0 - 7
 * 
 * @return true  if pressed
 */
bool BG_GetPress(ButtonGroup *self, uint8_t index)
{
    if(index > 7)
        return false;

    if(self->pressed & (1 << index))
        return true;
    else
        return false;
}

/***************************************************************************//**
 * @brief Clear the button press flag
 * 
 * @param self  pointer to the Button Group that you are using
 * 
 * @param index  button index 0 - 7
 */
void BG_ClearPressFlag(ButtonGroup *self, uint8_t index)
{
    if(index > 7)
        return;

    self->pressed &= ~(1 << index);
}

/***************************************************************************//**
 * @brief Check if a button was released
 * 
 * @param self  pointer to the Button Group that you are using
 * 
 * @param index  button index 0 - 7
 * 
 * @return true  if released
 */
bool BG_GetRelease(ButtonGroup *self, uint8_t index)
{
    if(index > 7)
        return false;

    if(self->released & (1 << index))
        return true;
    else
        return false;
}

/***************************************************************************//**
 * @brief Clear the button release flag
 * 
 * @param self  pointer to the Button Group that you are using
 * 
 * @param index  button index 0 - 7
 */
void BG_ClearReleaseFlag(ButtonGroup *self, uint8_t index)
{
    if(index > 7)
        return;

    self->released &= ~(1 << index);
}

/***************************************************************************//**
 * @brief Get the current output value of a single button
 * 
 * @param self  pointer to the Button Group that you are using
 * 
 * @param index  button index 0 - 7
 * 
 * @return uint8_t  0 = not pressed, 1 = pressed
 */
uint8_t BG_GetButtonOutput(ButtonGroup *self, uint8_t index)
{
    if(index > 7)
        return 0;
        
    if(self->output & (1 << index))
        return 1;
    else
        return 0;
}

/***************************************************************************//**
 * @brief Get the current output of the entire group
 * 
 * @param self  pointer to the Button Group that you are using
 * 
 * @return uint8_t  the output of all 8 buttons
 */
uint8_t BG_GetButtonGroupOutput(ButtonGroup *self)
{
    return self->output;
}

/*
 End of File
 */
