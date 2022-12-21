/***************************************************************************//**
 * @brief Button Group Library Header File
 * 
 * @author  Matthew Spinks
 * 
 * @date 6/25/22     Original creation
 * 
 * @file ButtonGroup.h
 * 
 * @details
 *      A stripped down version of my original button library with less 
 * features. In order to try and save a little bit of memory, buttons are 
 * grouped in eight. There are is no long press feature, no callback functions
 * and the debounce length is used for both the press and release. This library 
 * can also be used for things like inserting connectors or switches. Anything 
 * that requires debouncing. This library is most useful if you have a large 
 * number of inputs that need debouncing.
 * 
 *      To create a group of buttons, initialize the object with the debounce
 * time and the sample rate in milliseconds. This is how often you will call
 * the Tick function.
 * 
 *      When you check for a press or release event, the flag for that event 
 * is not automatically cleared. This is for you to decide how to handle it. 
 * Normally, I clear the flag right after checking for an event, but there may 
 * be times when you don't want to do that.
 * 
 *      If you have a large list of inputs you can use some preprocessor macros 
 * to help manage your inputs and their indexes. You can use an enum with the 
 * last value being "TOTAL". By doing this, you can make an array of Button 
 * Group objects like so:
 * 
 * enum { BUTTON1,
 *        BUTTON2,
 *        TOTAL_BUTTONS};
 * 
 * #define NUM_BUTTON_GROUPS (TOTAL_BUTTONS / 8 + 1)
 * 
 * Here are some more useful macros:
 * 
 * #define InputGroup(x) (x >> 3) // divide by 8
 * #define InputBit(x) (x & 0x07) // mod 8
 * 
 * Example Usage:
 *      ButtonGroup oneButtonGroup;
 *      ButtonGroup arrayOfButtons[NUM_BUTTON_GROUPS];
 *      #define ButtonArray(x) &arrayOfButtons[InputGroup(x)], InputBit(x)
 *      
 *      BG_Init(&oneButtonGroup, DEBOUNCE_MS, TICK_MS);
 * 
 *      for(i=0; i<NUM_BUTTON_GROUPS; i++) {
 *          BG_Init(&arrayOfButtons[i], DEBOUNCE_MS, TICKS_MS); }
 * 
 *      BG_UpdateValue(&oneButtonGroup, BUTTON1, true); // pressed
 * 
 *      BG_UpdateValue(&arrayOfButtons[InputGroup(BUTTON20)], 
 *          InputBit(BUTTON20), true);
 * 
 *      BG_Tick(&oneButtonGroup);
 * 
 *      for(i=0; i<NUM_BUTTON_GROUPS; i++) {
 *          BG_Tick(&arrayOfButtons[i]); } // update entire array
 *      
 *      if(BG_GetPress(ButtonArray(BUTTON20)) // shorten with macro
 *      { .... }
 * 
 ******************************************************************************/

#ifndef BUTTON_GROUP_H
#define	BUTTON_GROUP_H

// ***** Includes **************************************************************

#include <stdint.h>
#include <stdbool.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************


typedef struct ButtonGroupTag
{
    uint16_t debouncePeriod;
    uint16_t integrator[8];
    uint16_t analogThreshold;
    uint8_t isAnalog;
    uint8_t input;
    uint8_t output;
    uint8_t previousOutput;
    uint8_t pressed;
    uint8_t released;
} ButtonGroup;

/** 
 * Description of struct members. You shouldn't really mess with any of these
 * variables directly. That is why I made functions for you to use.
 * 
 * debouncePeriod  The number of ticks to debounce the button press and release
 * 
 * integrator  A counter which changes the output variable when it saturates.
 *             Basically, it smooths the inputs.
 * 
 * analogThreshold  The threshold for all analog buttons in the group
 * 
 * isAnalog  Set to 1 if button is analog
 * 
 * input  The combined input of all 8 buttons
 * 
 * output  The combined output of all 8 buttons
 * 
 * previousOutput  The previous output is used for detecting events
 * 
 * pressed  The combined pressed event flags for all 8 buttons
 * 
 * released  The combined released event flags for all 8 buttons
 */

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Function Prototypes *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Initialize a Button Group object
 * 
 * @param self  pointer to the Button Group that you are using
 * 
 * @param debounceMs  debounce time for button press and release (milliseconds)
 * 
 * @param tickMs  how often you plan to call the Tick function
 */
void BG_Init(ButtonGroup *self, uint16_t debounceMs, uint16_t tickMs);

/***************************************************************************//**
 * @brief Update the Button Group based on the input signals
 * 
 * You must call this function at a periodic rate.
 * 
 * @param self  pointer to the Button Group that you are using
 */
void BG_Tick(ButtonGroup *self);

/***************************************************************************//**
 * @brief Set the analog input thresholds
 * 
 * All buttons will be set as digital by default. All buttons in a group will
 * share the same analog threshold.
 * 
 * @param self  pointer to the Button Group that you are using
 * 
 * @param threshold  not pressed: <= threshold, pressed: > threshold
 */
void BG_SetAnalogThreshold(ButtonGroup *self, uint16_t threshold);

/***************************************************************************//**
 * @brief Set the button type to analog
 * 
 * All buttons will be set as digital by default. All buttons in a group will
 * share the same analog threshold. Don't forget to set an analog threshold.
 * 
 * @param self  pointer to the Button Group that you are using
 * 
 * @param index  the button you are updating (0 - 7)
 */
void BG_SetButtonTypeAnalog(ButtonGroup *self, uint8_t index);

/***************************************************************************//**
 * @brief Set the button type to digital
 * 
 * All buttons will be set as digital by default. If for some reason, you need
 * to set a button back to digital, you can call this function. This is mostly
 * just for completeness.
 * 
 * @param self  pointer to the Button Group that you are using
 * 
 * @param index  the button you are updating (0 - 7)
 */
void BG_SetButtonTypeDigital(ButtonGroup *self, uint8_t index);

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
 * @param index  the button you are updating (0 - 7)
 * 
 * @param value  if digital, 0 = not pressed
 */
void BG_UpdateValue(ButtonGroup *self, uint8_t index, uint16_t value);

/***************************************************************************//**
 * @brief Check if a button was pressed
 * 
 * @param self  pointer to the Button Group that you are using
 * 
 * @param index  button index 0 - 7
 * 
 * @return true  if pressed
 */
bool BG_GetPress(ButtonGroup *self, uint8_t index);

/***************************************************************************//**
 * @brief Clear the button press flag
 * 
 * @param self  pointer to the Button Group that you are using
 * 
 * @param index  button index 0 - 7
 */
void BG_ClearPressFlag(ButtonGroup *self, uint8_t index);

/***************************************************************************//**
 * @brief Check if a button was released
 * 
 * @param self  pointer to the Button Group that you are using
 * 
 * @param index  button index 0 - 7
 * 
 * @return true  if released
 */
bool BG_GetRelease(ButtonGroup *self, uint8_t index);

/***************************************************************************//**
 * @brief Clear the button release flag
 * 
 * @param self  pointer to the Button Group that you are using
 * 
 * @param index  button index 0 - 7
 */
void BG_ClearReleaseFlag(ButtonGroup *self, uint8_t index);

/***************************************************************************//**
 * @brief Get the current output value of a single button
 * 
 * @param self  pointer to the Button Group that you are using
 * 
 * @param index  button index 0 - 7
 * 
 * @return uint8_t  0 = not pressed, 1 = pressed
 */
uint8_t BG_GetButtonOutput(ButtonGroup *self, uint8_t index);

/***************************************************************************//**
 * @brief Get the current output of the entire group
 * 
 * @param self  pointer to the Button Group that you are using
 * 
 * @return uint8_t  the output of all 8 buttons, 0 = not pressed, 1 = pressed
 */
uint8_t BG_GetButtonGroupOutput(ButtonGroup *self);

#endif	/* BUTTON_GROUP_H */