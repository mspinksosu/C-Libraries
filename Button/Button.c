/***************************************************************************//**
 * @brief Button Library
 * 
 * @author  Matthew Spinks
 * 
 * @date 12/6/19    Original creation
 * @date 10/1/21    Updated documention
 * @date 2/21/22    Added doxygen
 * @date 4/12/22    Refactored to create analog and digital button classes
 * @date 8/6/22     Updated doxygen
 * 
 * @file Button.c
 * 
 * @details
 *      A library that handles basic button features like short press and long
 * press. It can debounce the button press or release, to use with a digital
 * input, or it can use a analog voltage level.
 * 
 *      If you need a simpler Button library that takes up less memory. Try my
 * ButtonGroup library. It only handles digital inputs, and only does button
 * press and release. No long press or analog features. The output of each
 * button is combined with up to 7 other buttons.
 *
 ******************************************************************************/

#include <stddef.h>
#include "Button.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************


// ***** Static Function Prototypes ********************************************

static void AnalogButton_Init(AnalogButton *self, uint16_t longPressMs);
static void DigitalButton_Init(DigitalButton *self, uint16_t longPressMs);
static void AnalogButton_Tick(AnalogButton *self, uint16_t value);
static void DigitalButton_Tick(DigitalButton *self, bool isPressed);

// *****************************************************************************

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

    /* Connect the base class's (Button) instance to this button that you just 
    created. This lets you use the base class in function calls */
    base->instance = self;
}

// *****************************************************************************

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

    /* Connect the base class's (Button) instance to this button that you just 
    created. This lets you use the base class in function calls */
    base->instance = self;
}

// *****************************************************************************

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

// *****************************************************************************

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

// *****************************************************************************

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

// *****************************************************************************

bool Button_GetShortPress(Button *self)
{
    if(self->flags.shortPress)
        return true;
    else
        return false;
}

// *****************************************************************************

bool Button_GetLongPress(Button *self)
{
    if(self->flags.longPress)
        return true;
    else
        return false;
}

// *****************************************************************************

void Button_ClearShortPressFlag(Button *self)
{
    self->flags.shortPress = 0;
}

// *****************************************************************************

void Button_ClearLongPressFlag(Button *self)
{
    self->flags.longPress = 0;
}

// *****************************************************************************

bool Button_GetButtonDownEvent(Button *self)
{
    if(self->flags.buttonDownEvent)
        return true;
    else
        return false;
}

// *****************************************************************************

bool Button_GetButtonUpEvent(Button *self)
{
    if(self->flags.buttonUpEvent)
        return true;
    else
        return false;
}

// *****************************************************************************

void Button_ClearButtonDownFlag(Button *self)
{
    self->flags.buttonDownEvent = 0;
}

// *****************************************************************************

void Button_ClearButtonUpFlag(Button *self)
{
    self->flags.buttonUpEvent = 0;
}

// *****************************************************************************

ButtonState Button_GetState(Button *self)
{
    return self->state;
}

// *****************************************************************************

ButtonType Button_GetType(Button *self)
{
    return self->type;
}

// *****************************************************************************

ButtonLength Button_GetLength(Button *self)
{
    return self->length;
}

// *****************************************************************************

void Button_SetShortPressCallback(Button *self, ButtonCallbackFunc Function)
{
    self->shortPressCallback = Function;
}

// *****************************************************************************

void Button_SetLongPressCallback(Button *self, ButtonCallbackFunc Function)
{
    self->longPressCallback = Function;
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
                    {
                        self->super->flags.longPress = 1;
                        // callback function
                        if(self->super->longPressCallback)
                            self->super->longPressCallback(self->super);
                    }
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
                        // callback function
                        if(self->super->shortPressCallback)
                            self->super->shortPressCallback(self->super);
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
                        // callback function
                        if(self->super->shortPressCallback)
                            self->super->shortPressCallback(self->super);
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
                        // callback function
                        if(self->super->shortPressCallback)
                            self->super->shortPressCallback(self->super);
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
                    {
                        self->super->flags.longPress = 1;
                        // callback function
                        if(self->super->longPressCallback)
                            self->super->longPressCallback(self->super);
                    }
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
                            // callback function
                            if(self->super->shortPressCallback)
                                self->super->shortPressCallback(self->super);
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
                            // callback function
                            if(self->super->shortPressCallback)
                                self->super->shortPressCallback(self->super);
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
