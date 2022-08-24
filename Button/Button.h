/***************************************************************************//**
 * @brief Button Library Header File
 * 
 * @author  Matthew Spinks
 * 
 * @date 12/6/19    Original creation
 * @date 10/1/21    Updated documention
 * @date 2/21/22    Added doxygen
 * @date 4/12/22    Refactored to create analog and digital button classes
 * 
 * @file Button.h
 * 
 * @details
 *      A library that handles basic button features like short press and long
 * press. It can debounce the button press or release when using a digital
 * input, or it can use a analog voltage level. This library can also be used 
 * for things like inserting connectors or switches.
 * 
 *      There are two classes of buttons: analog and digital. To make a button, 
 * you will need both the base class Button object and either an analog or 
 * digital button. You will also need the debounce time in milliseconds if you
 * are using a digital button, or the high and low thresholds if you are using
 * an analog button. In addition to that, you need the expected update rate in
 * milliseconds (how often you call the tick function). This is used to
 * calculate the long press time as well as debounce the digital button
 * 
 *      If you are using a digital button and you are debouncing your buttons 
 * with hardware or some other method, initialize the debounce times as zero.
 * 
 *      There are two types of buttons available: short press and long press.
 * A button with a long press feature has a little bit different behavior than 
 * a regular button. When you press and hold it for the specified period of 
 * time, the long press event fires. If you let go of it before the long press 
 * period expires, it performs the normal short press action. 
 * 
 *      To create a long press button, simply give the long press length in 
 * milliseconds to the init function. Set it to zero if not needed. If you need
 * to change the long press length of a button later, use the set long press 
 * function. It would be best to make sure your button is in a known state 
 * before changing the long press time though.
 * 
 *      There are multiple types of events available to you. When you check for 
 * an event, the flag for that event is not automatically cleared. This is for 
 * you to decide how to handle it. Normally, I clear the flag right after 
 * checking for an event, but there may be times when you don't want to do that.
 * Functions have been provided for you to check for events and clear flags.
 * 
 *      Normally, you would only ever use the short press and long press events.
 * However, I've also included up and down events. The difference is that the 
 * short-press event behavior changes dependent on the button type. The up and 
 * down events always occur after debouncing a press or release and do not 
 * change. These can be useful depending on the situation. Like for example, 
 * you need to register a button down event in order to wake up a display, but 
 * need to ignore the normal short press action. Or if you need to do something
 * during the time between the button being held down and the long press event.
 * 
 * // TODO callback details
 * 
 * Example Code:
 *      Button PushButton;
 *      DigitalButton digitalPushButton;
 *      Button_Digital_Create(&digitalPushButton, &PushButton, 20, 20, 10);
 *      Button_InitMs(&PushButton, 3000); // long press 3 seconds
 *      uint16_t buttonIsPressed = GPIO_ReadPin(&pin1);
 *      Button_Tick(&PushButton, buttonIsPressed);
 *      if(Button_GetLongPress(&PushButton))
 *      { .... do something .... }
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

/* A forward declaration which will allow the compiler to "know" what a Button
is before I use it in the callback function declaration below me */
//typedef struct ButtonTag Button;

/* callback function pointer. The context pointer will point to the Button that 
initiated the callback. This is so that you can service multiple Button  
callbacks with the same function if you desire. */
typedef void (*ButtonCallbackFunc)(void *buttonContext);

typedef struct ButtonTag
{
    void *instance;
    ButtonCallbackFunc shortPressCallback;
    ButtonCallbackFunc longPressCallback;
    ButtonState state;
    ButtonType type;
    ButtonLength length;
    uint16_t longPressPeriod;
    uint16_t longPressCounter;
    uint16_t tickMs;
    
    union {
        struct {
            unsigned shortPress         :1;
            unsigned longPress          :1;
            unsigned buttonDownEvent    :1;
            unsigned buttonUpEvent      :1;
            unsigned                    :4;
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
 * Description of struct members. You shouldn't really mess with any of these
 * variables directly. That is why I made functions for you to use.
 * 
 * instance  Pointer to the sub class AnalogButton or DigitalButton
 * 
 * super  Pointer from the sub class to the base class Button
 * 
 * state  The state machine that handles debouncing
 * 
 * type  Either analog or digital
 * 
 * length  Either short press type or long press type
 * 
 * longPressPeriod  The number of ticks for the long press feature
 * 
 * longPressCounter  Counts the number of ticks for the long press feature
 * 
 * tickMs  How often you call the tick function (in milliseconds)
 * 
 * shortPress  The button has been pushed, debounced (and released). For a long 
 *             press type button, the short press occurs on release
 * 
 * longPress  The button has been held down for a certain period of time
 * 
 * buttonDownEvent  The button has been pushed and debounced
 * 
 * buttonUpEvent  The button has been released and debounced
 * 
 * lowThreshold  If the analog button voltage goes from above the highThreshold
 *               to below the lowThreshold the button was released
 * 
 * highThreshold  If the analog button voltage goes from below the lowThreshold
 *                to above the highThreshold the button was pushed
 * 
 * pressDebouncePeriod  The number of ticks to debounce the button press
 * 
 * releaseDebouncePeriod  The number of ticks to debounce the button release
 * 
 * debounceCounter  Counts the number of ticks for debouncing
 */

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Function Prototypes *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Creates an Analog Button object and links it to the base class
 * 
 * The output is high (pressed) when the value crosses the lower and upper
 * thresholds, and low (released) when the value is below the upper and lower 
 * thresholds. This is similar in operation to a Schmitt trigger. The tickMs 
 * is necessary if you are going to use the long press feature
 * 
 * @param self  pointer to the Button that you are using
 * 
 * @param base  pointer to the base class object used for function calls
 * 
 * @param lowThreshold  low voltage threshold (adc count)
 * 
 * @param highThreshold  low voltage threshold (adc count)
 * 
 * @param tickMs  how often you plan to call the Button Tick function
 */
void Button_Analog_Create(AnalogButton *self, Button *base, uint16_t lowThreshold, uint16_t highThreshold, uint16_t tickMs);

/***************************************************************************//**
 * @brief Creates a Digital Button object and links it to the base class
 * 
 * If you are using hardware to debounce (RC filter) then set your debounce
 * times to 0. The tickMs is still necessary if you plan to use the long press
 * feature.
 * 
 * @param self  pointer to the Button that you are using
 * 
 * @param base  pointer to the base class object used for function calls
 * 
 * @param pressDebounceMs  how long to debounce the button press
 * 
 * @param releaseDebounceMs  how long to debounce the button release
 * 
 * @param tickMs  how often you plan to call the Button Tick function
 */
void Button_Digital_Create(DigitalButton *self, Button *base, uint16_t pressDebounceMs, uint16_t releaseDebounceMs, uint16_t tickMs);

// -----------------------------------------------------------------------------

/***************************************************************************//**
 * @brief Initialize a button
 * 
 * If long press is zero or less than the debounce time, the button will be
 * considered a short press type
 * 
 * @param self  pointer to the Button that you are using
 * 
 * @param longPressMs  long press period (in ms). Zero if not needed
 */
void Button_InitMs(Button *self, uint16_t longPressMs);

/***************************************************************************//**
 * @brief Change the long press time of a button
 * 
 * Calling this function again after initialization will not alter the state of 
 * the button. This is useful when you have a long press button and you need to 
 * alter the long press time (for example press and hold time to turn on vs 
 * turn off) You should make sure your button is in a known state when calling 
 * this function.
 * 
 * @param self  pointer to the Button that you are using
 * 
 * @param longPressMs  the period that you must hold the button down (in ms)
 */
void Button_SetLongPressMs(Button *self, uint16_t longPressMs);

/***************************************************************************//**
 * @brief Update the Button object with its current status.
 * 
 * You must call this function at a periodic rate and give it the current
 * status of the button. It does not matter if your pin is high or low, only
 * that the button is pressed, or not pressed. This Works for both analog and 
 * digital buttons. If the button is analog, simply give the adc reading. If
 * it is digital, 0 = not pressed and anything else is pressed
 * 
 * @param self  pointer to the Button that you are using
 * 
 * @param value  if digital 0 = not pressed
 */
void Button_Tick(Button *self, uint16_t value);

/***************************************************************************//**
 * @brief Check if there has been a short press event.
 * 
 * The short press event changes depending on the button type. If the button is
 * a short-press type, the event occurs after debouncing the release. If the 
 * button is a long-press type, the short press occurs on the release if the 
 * button is released before the long press timer expires. 
 * 
 * @param self  pointer to the Button that you are using
 * 
 * @return true if there was a short press event
 */
bool Button_GetShortPress(Button *self);

/***************************************************************************//**
 * @brief Check if there has been a long press event.
 * 
 * The long press event occurs when the button has been pressed, debounced,
 * and held for a period of time. The release is ignored.
 * 
 * @param self  pointer to the Button that you are using
 * 
 * @return true if there was a long press event
 */
bool Button_GetLongPress(Button *self);

/***************************************************************************//**
 * @brief Clear the short press event flag.
 * 
 * @param self  pointer to the Button that you are using
 */
void Button_ClearShortPressFlag(Button *self);

/***************************************************************************//**
 * @brief Clear the long press event flag.
 * 
 * @param self  pointer to the Button that you are using
 */
void Button_ClearLongPressFlag(Button *self);

/***************************************************************************//**
 * @brief Check if there was a button down event.
 * 
 * Unlike the short-press event, the down event always occurs anytime a button 
 * is pressed and debounced.
 * 
 * @param self  pointer to the Button that you are using
 * 
 * @return true if there was a button down event
 */
bool Button_GetButtonDownEvent(Button *self);

/***************************************************************************//**
 * @brief Check if there was a button up event.
 * 
 * The up event always occurs anytime a button is released and debounced.
 * 
 * @param self  pointer to the Button that you are using
 * 
 * @return true if there was a button up event
 */
bool Button_GetButtonUpEvent(Button *self);

/***************************************************************************//**
 * @brief Clear the button down event flag.
 * 
 * @param self  pointer to the Button that you are using
 */
void Button_ClearButtonDownFlag(Button *self);

/***************************************************************************//**
 * @brief Clear the button up event flag.
 * 
 * @param self  pointer to the Button that you are using
 */
void Button_ClearButtonUpFlag(Button *self);

/***************************************************************************//**
 * @brief Get the current state of the button
 * 
 * @param self  pointer to the Button that you are using
 * 
 * @return ButtonState  Up, Down, Debounce Press, or Debounce Release
 */
ButtonState Button_GetState(Button *self);

/***************************************************************************//**
 * @brief Get the button type.
 * 
 * @param self  pointer to the Button that you are using
 * 
 * @return ButtonType  analog or digital
 */
ButtonType Button_GetType(Button *self);

/***************************************************************************//**
 * @brief Get the button length
 * 
 * @param self  pointer to the Button that you are using
 * 
 * @return ButtonLength  short or long press
 */
ButtonLength Button_GetLength(Button *self);

/***************************************************************************//**
 * @brief Set a function to be called when a short press event happens
 * 
 * The function prototype must have a pointer to a Button as its argument. 
 * This is so that multiple Buttons can be serviced by the same function if 
 * desired. This function will not clear any event flags.
 * 
 * @param self  pointer to the Button that you are using
 * 
 * @param Function  format: void SomeFunction(Button *context)
 */
void Button_SetShortPressCallback(Button *self, ButtonCallbackFunc Function);

/***************************************************************************//**
 * @brief Set a function to be called when a long press event happens
 * 
 * The function prototype must have a pointer to a Button as its argument. 
 * This is so that multiple Buttons can be serviced by the same function if 
 * desired. This function will not clear any event flags.
 * 
 * @param self  pointer to the Button that you are using
 * 
 * @param Function  format: void SomeFunction(Button *context)
 */
void Button_SetLongPressCallback(Button *self, ButtonCallbackFunc Function);

#endif /* BUTTON_H */