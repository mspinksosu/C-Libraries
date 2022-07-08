/***************************************************************************//**
 * @brief Button Library
 * 
 * @author  Matthew Spinks
 * 
 * @date 12/6/19    Original creation
 *       10/1/21    Updated documention
 *       2/21/22    Added doxygen
 *       4/12/22    Refactored to create analog and digital button classes
 * 
 * @file Button.c
 * 
 * @details
 *      A library that handles basic button features like short press and long
 * press. It can also debounce both the button press and release for you if
 * you want. This library can also be used for things like inserting connectors 
 * or switches. Anything that requires debouncing.
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
 *      To create a long press button, call the init function with the long 
 * press length in milliseconds. Set it to zero if not needed. To change the
 * long press length of a button later, use the set long press function. It 
 * would be best to make sure your button is in a known state before changing 
 * the long press time though.
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
 * during the time the button is being held down and the long press event.
 * 
 * Example Code:
 *      Button PushButton;
 *      DigitalButton digitalPushButton;
 *      Button_Digital_Create(&digitalPushButton, &PushButton, 20, 20, 10);
 *      Button_InitMs(&PushButton, 3000);
 *      uint16_t buttonIsPressed = getGPIOValue();
 *      Button_Tick(&PushButton, buttonIsPressed);
 *
 ******************************************************************************/

#include <stddef.h>
#include "Button.h"

// ***** Defines ***************************************************************


// ***** Function Prototypes ***************************************************

static void AnalogButton_Init(AnalogButton *self, uint16_t longPressMs);
static void DigitalButton_Init(DigitalButton *self, uint16_t longPressMs);
static void AnalogButton_Tick(AnalogButton *self, uint16_t value);
static void DigitalButton_Tick(DigitalButton *self, bool isPressed);

// ***** Global Variables ******************************************************


/***************************************************************************//**
 * @brief Creates a Button object
 * 
 * Links the instance pointer in the base class (Button) to the sub class.
 * For this library, I decided to put both sub classes, Analog and Digital into
 * the same c file. Using this method, I do not need to assign a pointer to an
 * interface in the base class. When the functions in the interface are called
 * from the base class, I will check which sub class instance is being used
 * and call the appropriate function.
 * 
 * @param self  pointer to the Button that you are using
 * 
 * @param instanceOfSubClass  pointer to the sub class
 */
void Button_Create(Button *self, void *instanceOfSubClass)
{
    self->instance = instanceOfSubClass;
}

/***************************************************************************//**
 * @brief Creates an Analog Button object and then calls the base class
 *        create function
 * 
 * The output is high (pressed) when the pot value crosses the lower and upper
 * thresholds, and low (released) when the pot value is below the upper and 
 * lower thresholds. This is similar in operation to a Schmitt trigger. The
 * tickMs is necessary if you are going to use the long press feature
 * 
 * @param self  pointer to the Button that you are using
 * 
 * @param base  pointer to the base class object used for function calls
 * 
 * @param lowThreshold 
 * 
 * @param highThreshold 
 * 
 * @param tickMs 
 */
void Button_Analog_Create(AnalogButton *self, Button *base, uint16_t lowThreshold, uint16_t highThreshold, uint16_t tickMs)
{
    self->super = base;
    self->super->type = BUTTON_ANALOG;
    self->super->tickMs = tickMs;
    self->super->length = BUTTON_SHORT_PRESS;
    self->super->flags.all = 0;

    if(lowThreshold > highThreshold)
    {
        // swap the inputs
        self->highThreshold = lowThreshold;
        self->lowThreshold = highThreshold;
    }
    else
    {
        self->highThreshold = highThreshold;
        self->lowThreshold = lowThreshold;
    }

    /* Call the base class constructor. When you do this you connect the base
    class's (Button) instance to this button that you just created. This 
    lets you use the base class in function calls */
    Button_Create(base, self);
}

/***************************************************************************//**
 * @brief Creates a Digital Button object and then calls the base class
 *        create function
 * 
 * @param self  pointer to the Button that you are using
 * 
 * @param base  pointer to the base class object used for function calls
 * 
 * @param pressDebounceMs 
 * 
 * @param releaseDebounceMs 
 * 
 * @param tickMs 
 */
void Button_Digital_Create(DigitalButton *self, Button *base, uint16_t pressDebounceMs, uint16_t releaseDebounceMs, uint16_t tickMs)
{
    self->super = base;
    self->super->type = BUTTON_DIGITAL;
    self->super->tickMs = tickMs;
    self->super->length = BUTTON_SHORT_PRESS;
    self->super->flags.all = 0;

    if(tickMs != 0)
    {
        self->pressDebouncePeriod = pressDebounceMs / tickMs;
        self->releaseDebouncePeriod = releaseDebounceMs / tickMs;
    }

    /* Call the base class constructor. When you do this you connect the base
    class's (Button) instance to this button that you just created. This 
    lets you use the base class in function calls */
    Button_Create(base, self);
}

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
void Button_InitMs(Button *self, uint16_t longPressMs)
{
    if(self->instance)
    {
        if(self->type == BUTTON_ANALOG)
        {
            AnalogButton_Init(self->instance, longPressMs);
        }
        else if(self->type == BUTTON_DIGITAL)
        {
            DigitalButton_Init(self->instance, longPressMs);
        }
    }

    /* Reset the button state */
    self->state = BUTTON_UP;
}

/***************************************************************************//**
 * @brief Sets the long press feature of a button
 * 
 * Does not alter the state of the button. This is useful when you have a long
 * press button and you need to alter the long press time for example turning 
 * on vs turning off. You should make sure your button is in a known state when
 * calling this function
 * 
 * @param self  pointer to the Button that you are using
 * 
 * @param longPressMs  the period that you must hold the button down (in ms)
 * 
 */
void Button_SetLongPressMs(Button *self, uint16_t longPressMs)
{
    if(self->instance)
    {
        if(self->type == BUTTON_ANALOG)
        {
            AnalogButton_Init(self->instance, longPressMs);
        }
        else if(self->type == BUTTON_DIGITAL)
        {
            DigitalButton_Init(self->instance, longPressMs);
        }
    }
}

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
 * 
 */
void Button_Tick(Button *self, uint16_t value)
{   
    if(self->instance != NULL)
    {
        if(self->type == BUTTON_ANALOG)
        {
            AnalogButton_Tick(self->instance, value);
        }
        else if(self->type == BUTTON_DIGITAL)
        {
            DigitalButton_Tick(self->instance, value);
        }
    }
}

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
bool Button_GetShortPress(Button *self)
{
    if(self->flags.shortPress)
        return true;
    else
        return false;
}

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
bool Button_GetLongPress(Button *self)
{
    if(self->flags.longPress)
        return true;
    else
        return false;
}

/***************************************************************************//**
 * @brief Clear the short press event flag.
 * 
 * @param self  pointer to the Button that you are using
 */
void Button_ClearShortPressFlag(Button *self)
{
    self->flags.shortPress = 0;
}

/***************************************************************************//**
 * @brief Clear the long press event flag.
 * 
 * @param self  pointer to the Button that you are using
 */
void Button_ClearLongPressFlag(Button *self)
{
    self->flags.longPress = 0;
}

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
bool Button_GetButtonDownEvent(Button *self)
{
    if(self->flags.buttonDownEvent)
        return true;
    else
        return false;
}

/***************************************************************************//**
 * @brief Check if there was a button up event.
 * 
 * The up event always occurs anytime a button is released and debounced.
 * 
 * @param self  pointer to the Button that you are using
 * 
 * @return true if there was a button up event
 */
bool Button_GetButtonUpEvent(Button *self)
{
    if(self->flags.buttonUpEvent)
        return true;
    else
        return false;
}

/***************************************************************************//**
 * @brief Clear the button down event flag.
 * 
 * @param self  pointer to the Button that you are using
 */
void Button_ClearButtonDownFlag(Button *self)
{
    self->flags.buttonDownEvent = 0;
}

/***************************************************************************//**
 * @brief Clear the button up event flag.
 * 
 * @param self  pointer to the Button that you are using
 */
void Button_ClearButtonUpFlag(Button *self)
{
    self->flags.buttonUpEvent = 0;
}

/***************************************************************************//**
 * @brief Get the current state of the button
 * 
 * @param self  pointer to the Button that you are using
 * 
 * @return ButtonState  Up, Down, Debounce Press, or Debounce Release
 */
ButtonState Button_GetState(Button *self)
{
    return self->state;
}

/***************************************************************************//**
 * @brief Get the button type.
 * 
 * @param self  pointer to the Button that you are using
 * 
 * @return ButtonType  analog or digital
 */
ButtonType Button_GetType(Button *self)
{
    return self->type;
}

/***************************************************************************//**
 * @brief Get the button length
 * 
 * @param self  pointer to the Button that you are using
 * 
 * @return ButtonLength  short or long press
 */
ButtonLength Button_GetLength(Button *self)
{
    return self->length;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //  
// ***** Local Functions *****************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Initialize an Analog Button
 * 
 * If long press is zero or less than the debounce time, the button will be
 * considered a short press type
 * 
 * @param self  pointer to the Button that you are using
 * 
 * @param longPressMs  long press period (in ms). Zero if not needed
 */
static void AnalogButton_Init(AnalogButton *self, uint16_t longPressMs)
{
    if(self->super->tickMs != 0)
    {
        self->super->longPressPeriod = longPressMs / self->super->tickMs;
    }
    
    /* In case you accidentally initialize the long press period to zero or
     a value less than one tick */
    if(self->super->longPressPeriod == 0)
        self->super->length = BUTTON_SHORT_PRESS;
    else
        self->super->length = BUTTON_LONG_PRESS;
}

/***************************************************************************//**
 * @brief Initialize a Digital Button
 * 
 * If long press is zero or less than the debounce time, the button will be
 * considered a short press type
 * 
 * @param self  pointer to the Button that you are using
 * 
 * @param longPressMs  long press period (in ms). Zero if not needed
 */
static void DigitalButton_Init(DigitalButton *self, uint16_t longPressMs)
{
    if(self->super->tickMs != 0)
    {
        self->super->longPressPeriod = longPressMs / self->super->tickMs;
    }
    
    /* In case you accidentally initialize the long press period to less than
    the short press period */
    if(self->super->longPressPeriod <= (self->pressDebouncePeriod + 
                                        self->releaseDebouncePeriod))
    {
        self->super->length = BUTTON_SHORT_PRESS;
    }
    else
        self->super->length = BUTTON_LONG_PRESS;
}

/***************************************************************************//**
 * @brief Update the Analog Button with its current status.
 * 
 * This function should be called at a periodic rate specified during
 * initialization. After debouncing the switch, event flags will be set. These 
 * can be checked with getter functions.
 * 
 * This function will be called using the base class function. This is so
 * that the user does not need to worry about which implementation to call. 
 * 
 * @param self  pointer the Analog Button
 * 
 * @param value  adc reading of the button
 */
static void AnalogButton_Tick(AnalogButton *self, uint16_t value)
{
    switch(self->super->state)
    {
        case BUTTON_UP:
            if(value > self->lowThreshold)
            {
                self->super->state = BUTTON_DEBOUNCE_PRESS;
            }
            break;
        case BUTTON_DEBOUNCE_PRESS:
            if(value >= self->highThreshold)
            {
                self->super->flags.buttonDownEvent = 1;
                self->super->state = BUTTON_DOWN;
                self->super->longPressCounter = 0;
            }
            break;
        case BUTTON_DOWN:
            // Update the long press timer
            if(value >= self->highThreshold && self->super->length == BUTTON_LONG_PRESS)
            {
                if(self->super->longPressCounter < self->super->longPressPeriod)
                {
                    self->super->longPressCounter++;
                    
                    if(self->super->longPressCounter == self->super->longPressPeriod)
                        self->super->flags.longPress = 1;
                }
            }

            // Check for the button release
            if(value < self->highThreshold)
            {
                if(self->super->length == BUTTON_LONG_PRESS)
                {
                    // If the button is a long press type, the short press
                    // event should happen on the release if the button
                    // is released before the timer expires.
                    if(self->super->longPressCounter < self->super->longPressPeriod)
                    {
                        self->super->flags.shortPress = 1;
                    }
                }
                self->super->state = BUTTON_DEBOUNCE_RELEASE;
            }
            break;
        case BUTTON_DEBOUNCE_RELEASE:
            if(value <= self->lowThreshold)
            {
                self->super->flags.buttonUpEvent = 1;
                self->super->state = BUTTON_UP;
            }
            break;
    }
}

/***************************************************************************//**
 * @brief Update the Digital Button with its current status.
 * 
 * This function should be called at a periodic rate specified during
 * initialization. After debouncing the switch, event flags will be set. These 
 * can be checked with getter functions.
 * 
 * This function will be called using the base class function. This is so
 * that the user does not need to worry about which implementation to call.
 * 
 * @param self  pointer the Digital Button
 * 
 * @param isPressed  true = pressed
 */
static void DigitalButton_Tick(DigitalButton *self, bool isPressed)
{
    switch(self->super->state)
    {
        case BUTTON_UP:
            if(isPressed)
            {
                if(self->pressDebouncePeriod == 0)
                {
                    // If the debounce period is zero, we assume that 
                    // debouncing is being done via hardware
                    if(self->super->length == BUTTON_SHORT_PRESS)
                    {
                        self->super->flags.shortPress = 1;
                        // TODO callback function
                    }
                    self->super->flags.buttonDownEvent = 1;
                    self->super->state = BUTTON_DOWN;
                    self->super->longPressCounter = 0;
                }
                else
                {
                    // Else, we need to debounce our button press
                    self->super->state = BUTTON_DEBOUNCE_PRESS;
                    self->debounceCounter = 0;
                }
            }
            break;
        case BUTTON_DEBOUNCE_PRESS:
            self->debounceCounter++;
            if(self->debounceCounter == self->pressDebouncePeriod)
            {
                if(isPressed)
                {
                    // Button debounced successfully
                    if(self->super->length == BUTTON_SHORT_PRESS)
                    {
                        // We are finished.
                        self->super->flags.shortPress = 1;
                        // TODO callback function
                    }
                    self->super->flags.buttonDownEvent = 1;
                    self->super->state = BUTTON_DOWN;
                    self->super->longPressCounter = 0;
                }
                else
                {
                    // We finished debouncing, but the button isn't being
                    // pressed
                    self->super->state = BUTTON_UP;
                }
            }
            break;
        case BUTTON_DOWN:
            // Update the long press timer
            if(isPressed && self->super->length == BUTTON_LONG_PRESS)
            {
                if(self->super->longPressCounter < self->super->longPressPeriod)
                {
                    self->super->longPressCounter++;
                    
                    if(self->super->longPressCounter == self->super->longPressPeriod)
                        self->super->flags.longPress = 1;
                }
            }
            
            // Check for the button release
            if(!isPressed)
            {
                if(self->releaseDebouncePeriod == 0)
                {
                    // If the debounce period is zero, we assume that 
                    // debouncing is being done via hardware
                    if(self->super->length == BUTTON_LONG_PRESS)
                    {
                        // If the button is a long press type, the short press
                        // event should happen on the release if the button
                        // is released before the timer expires.
                        if(self->super->longPressCounter < self->super->longPressPeriod)
                        {
                            self->super->flags.shortPress = 1;
                        }
                    }
                    self->super->flags.buttonUpEvent = 1;
                    self->super->state = BUTTON_UP;
                }
                else
                {
                    // Else, we need to debounce our button releases
                    self->super->state = BUTTON_DEBOUNCE_RELEASE;
                    self->debounceCounter = 0;
                }
            }
            break;
        case BUTTON_DEBOUNCE_RELEASE:
            self->debounceCounter++;
            if(self->debounceCounter == self->releaseDebouncePeriod)
            {
                if(!isPressed)
                {
                    // Button debounced successfully
                    if(self->super->length == BUTTON_LONG_PRESS)
                    {
                        // If the button is a long press type, the short press
                        // event should happen on the release if the button
                        // is released before the timer expires.
                        if(self->super->longPressCounter < self->super->longPressPeriod)
                        {
                            self->super->flags.shortPress = 1;
                        }
                    }
                    self->super->flags.buttonUpEvent = 1;
                    self->super->state = BUTTON_UP;
                }
                else
                {
                    // We finished debouncing, but the button isn't being
                    // pressed
                    self->super->state = BUTTON_UP;
                }
            }
            break;
        default:
            
            break;
    }
}

/*
 End of File
 */
