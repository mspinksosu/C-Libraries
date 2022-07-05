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
 * grouped in eight. There are no analog button options, no long press feature,
 * and the debounce length is used for both the press and release. This library 
 * can also be used for things like inserting connectors or switches. Anything 
 * that requires debouncing. This library is most useful if you have a large 
 * number of inputs that need debouncing.
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

    uint8_t input;
    uint8_t output;
    uint8_t previousInput; // TODO is this needed for button events
    uint8_t previousOutput;
    uint8_t pressed;
    uint8_t released;
    uint8_t buttonDown;
    uint8_t buttonUp;
} ButtonGroup;

/** 
 * Description of struct members: TODO
 * 
 */

// ***** Function Prototypes ***************************************************

void BG_Init(ButtonGroup *self, uint16_t debounceMs, uint16_t tickMs);

void BG_Tick(ButtonGroup *self);

void BG_UpdateButtonValue(ButtonGroup *self, uint8_t index, bool isPressed);

bool BG_GetPress(ButtonGroup *self, uint8_t index);

void BG_ClearPressFlag(ButtonGroup *self, uint8_t index);

bool BG_GetRelease(ButtonGroup *self, uint8_t index);

void BG_ClearReleaseFlag(ButtonGroup *self, uint8_t index);

bool BG_GetButtonDownEvent(ButtonGroup *self, uint8_t index);

bool BG_GetButtonUpEvent(ButtonGroup *self, uint8_t index);

void BG_ClearButtonDownFlag(ButtonGroup *self, uint8_t index);

void BG_ClearButtonUpFlag(ButtonGroup *self, uint8_t index);

uint8_t BG_GetButtonOutput(ButtonGroup *self, uint8_t index);

uint8_t BG_GetButtonGroupOutput(ButtonGroup *self);


#endif	/* BUTTON_GROUP_H */