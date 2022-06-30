/***************************************************************************//**
 * @brief Button Library Header File
 * 
 * @author  Matthew Spinks
 * 
 * @date 12/6/19    Original creation
 *       10/1/21    Updated documention
 *       2/21/22    Added doxygen
 *       4/12/22    Refactored to create analog and digital button classes
 * 
 * @file Button.h
 * 
 * @details
 *      A library that handles basic button features like short press and long
 * press. It can also debounce both the button press and release for you if
 * you want. This library can also be used for things like inserting connectors 
 * or switches. Anything that requires debouncing.
 * 
 ******************************************************************************/

#ifndef BUTTON_H
#define	BUTTON_H

// ***** Includes **************************************************************

#include <stdint.h>
#include <stdbool.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

typedef enum ButtonStateTag
{
    BUTTON_UP = 0,
    BUTTON_DEBOUNCE_PRESS,
    BUTTON_DOWN,
    BUTTON_DEBOUNCE_RELEASE,
} ButtonState;

typedef enum ButtonTypeTag
{
    BUTTON_DIGITAL,
    BUTTON_ANALOG,
} ButtonType;

typedef enum ButtonLengthTag
{
    BUTTON_SHORT_PRESS,
    BUTTON_LONG_PRESS,
} ButtonLength;

/* function pointers */
//typedef void (*TimerCallbackFunc)(Timer *timerContext); // TODO

/* Button object with counters and flags (with bit field) */
typedef struct ButtonTag
{
    void *instance;
    ButtonState state;
    ButtonType type;
    ButtonLength length;
    uint16_t longPressPeriod;
    uint16_t longPressCounter;
    uint16_t tickMs;
    
    // bit field for button events
    union {
        struct {
            unsigned buttonDownEvent    :1;
            unsigned shortPress         :1;
            unsigned longPress          :1;
            unsigned buttonUpEvent      :1;
            unsigned                    :0; // fill to nearest byte
        };
        uint8_t all;
    } flags;
} Button;

typedef struct AnalogButtonTag
{
    Button *super;
    uint16_t lowThreshold;
    uint16_t highThreshold;
} AnalogButton;

typedef struct DigitalButtonTag
{
    Button *super;
    uint16_t pressDebouncePeriod;
    uint16_t releaseDebouncePeriod;
    uint16_t debounceCounter;
} DigitalButton;

/** 
 * Description of struct members:
 * 
 * pressDebouncePeriod  the number of ticks to debounce the button press
 * 
 * releaseDebouncePeriod  the number of ticks to debounce the button release
 * 
 * longPressPeriod  the number of ticks for the long press feature
 * 
 * longPressCounter  counts the number of ticks for the long press feature
 * 
 * debounceCounter  counts the number of ticks for debouncing
 * 
 * buttonState  the state machine that handles debouncing
 * 
 * buttonType  either analog or digital
 * 
 * buttonLength either short press type or long press type
 * 
 * buttonDownEvent  the button has been pushed down
 * 
 * shortPress  the button has been pushed, debounced, and released
 * 
 * longPress  the button has been held down for a certain period of time
 * 
 * buttonUpEvent  the button has been released
 * 
 */

// ***** Function Prototypes ***************************************************

void Button_Create(Button *self, void *instanceOfSubClass);

void Button_Analog_Create(AnalogButton *self, Button *base, uint16_t lowThreshold, uint16_t highThreshold, uint16_t tickMs);

void Button_Digital_Create(DigitalButton *self, Button *base, uint16_t pressDebounceMs, uint16_t releaseDebounceMs, uint16_t tickMs);

// -----------------------------------------------------------------------------

void Button_SetLongPressMs(Button *self, uint16_t longPressMs);

void Button_Tick(Button *self, uint16_t value);

bool Button_GetShortPress(Button *self);

bool Button_GetLongPress(Button *self);

void Button_ClearShortPressFlag(Button *self);

void Button_ClearLongPressFlag(Button *self);

bool Button_GetButtonDownEvent(Button *self);

bool Button_GetButtonUpEvent(Button *self);

void Button_ClearButtonDownFlag(Button *self);

void Button_ClearButtonUpFlag(Button *self);

ButtonState Button_GetState(Button *self);

ButtonType Button_GetType(Button *self);

ButtonLength Button_GetLength(Button *self);

//void Timer_SetFinishedCallback(TimerCallbackFunc);

#endif	/* BUTTON_H */