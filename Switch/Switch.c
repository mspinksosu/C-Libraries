/***************************************************************************//**
 * @brief Switch Library
 * 
 * @author Matthew Spinks
 * 
 * @date January 5, 2022  Original creation
 * 
 * @file Switch.c
 * 
 * @details
 *      A library that handles basic features of a "dual throw" switch. There
 * are two inputs and one output. The output A and B cannot be both true. 
 * Optional "center off" that allows output to be off. The default is to not 
 * allow the output to be off.
 * 
 * The inputs will be debounced for you. When you initialize the Switch object,
 * you give it a time in milliseconds. This is the time that you plan to 
 * periodically call the tick function.
 * 
 * If you would like to use this for a single pole switch, simply set one of
 * the inputs to always be false when you call the tick function.
 * 
 * Every function will use Switch as its first argument. This is the instance
 * of the Switch you created. You should not need to manipulate any data inside
 * the Switch object. I have provided functions to do this for you.
 * 
 * There are flags for different events such as "A on", "B on", as well as any 
 * output change. I am providing multiple different getter functions for states 
 * and events. How you determine the output is entirely up to you.
 * 
 ******************************************************************************/

#include "Switch.h"

// ***** Defines ***************************************************************

#define OUTPUT_A_BIT    0
#define OUTPUT_B_BIT    1

// ***** Function Prototypes ***************************************************

static void DecodeOutput(Switch *self, SwitchState newState);

// ***** Global Variables ******************************************************

/***************************************************************************//**
 * @brief Initializes a normal "SPDT" Switch object
 * 
 * @param self  pointer to the Switch that you are using
 * 
 * @param debounceMs  time to debounce the switch in milliseconds
 * 
 * @param tickMs  how often you plan to call the Switch Tick function
 * 
 * @param inputAInit  the initial state of input A (true = On)
 * 
 * @param inputBInit  the initial state of input B (true = On)
 */
void Switch_Init(Switch *self, uint16_t debounceMs, uint16_t tickMs, bool inputAInit, bool inputBInit)
{
    Switch_InitWithCenterOff(self, debounceMs, tickMs, inputAInit, inputBInit, false);
}

/***************************************************************************//**
 * @brief Initializes a Switch object with "center off" feature
 * 
 * @param self  pointer to the Switch that you are using
 * 
 * @param debounceMs  time to debounce the switch in milliseconds
 * 
 * @param tickMs  how often you plan to call the Switch Tick function
 * 
 * @param inputAInit  the initial state of input A (true = On)
 * 
 * @param inputBInit  the initial state of input B (true = On)
 * 
 * @param offAllowed  true = both inputs off is a valid state
 */
void Switch_InitWithCenterOff(Switch *self, uint16_t debounceMs, uint16_t tickMs, bool inputAInit, bool inputBInit, bool offAllowed)
{
    if(offAllowed)
    {
        self->type = SW_CENTER_OFF;
    }
    else
    {
        self->type = SW_NORMAL;
    }

    if(tickMs != 0)
    {
        self->debouncePeriod = debounceMs / tickMs;
    }

    // Check for invalid initial state
    SwitchState initState = (SwitchState)((inputAInit << OUTPUT_A_BIT) | (inputBInit << OUTPUT_B_BIT));

    if(initState == SW_OUTPUT_OFF && offAllowed == false)
    {
        initState = SW_OUTPUT_INVALID;
    }
    self->outputState = initState;
    self->debounceState = SW_READY;
}

/***************************************************************************//**
 * @brief Update the Switch with its current status
 * 
 * @param self  pointer to the Switch that you are using
 * 
 * @param inputAIsOn  true if input A is on
 * 
 * @param inputBIsOn  true if input B is on
 */
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
                    // If the debounce period is zero, we assume that
                    // debouncing is being done via hardware

                    // Output changed decode the output
                    DecodeOutput(self, currentState);
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
                // We have finished debouncing. Decode the output
                DecodeOutput(self, currentState);
                self->debounceState = SW_READY;
                self->outputState = currentState;
            }
            break;
        default:
                self->outputState = SW_OUTPUT_INVALID;
            break;
    }
    
}
/***************************************************************************//**
 * @brief Check if there was an output B on event
 * 
 * @param self  pointer to the Switch that you are using
 * 
 * @return true true if output A changed from off to on
 */
bool Switch_GetOutputAOnEvent(Switch *self)
{
    if(self->flags.outputAOnEvent)
        return true;
    else
        return false;
}

/***************************************************************************//**
 * @brief Clear the output A on event flag
 * 
 * @param self  pointer to the Switch that you are using
 */
void Switch_ClearOutputAOnEventFlag(Switch *self)
{
    self->flags.outputAOnEvent = 0;
}

/***************************************************************************//**
 * @brief Check if there was an output B on event
 * 
 * @param self  pointer to the Switch that you are using
 * 
 * @return true if output B changed from off to on
 */
bool Switch_GetOutputBOnEvent(Switch *self)
{
    if(self->flags.outputBOnEvent)
        return true;
    else
        return false;
}

/***************************************************************************//**
 * @brief Clear the output B on event flag
 * 
 * @param self  pointer to the Switch that you are using
 */
void Switch_ClearOutputBOnEventFlag(Switch *self)
{
    self->flags.outputBOnEvent = 0;
}

/***************************************************************************//**
 * @brief Check if there was an output off event
 * 
 * @param self  pointer to the Switch that you are using
 * 
 * @return true if there was an output off event
 */
bool Switch_GetOffEvent(Switch *self)
{
    if(self->flags.outputOffEvent)
        return true;
    else
        return false;
}

/***************************************************************************//**
 * @brief Clear the output off event flag
 * 
 * @param self  pointer to the Switch that you are using
 */
void Switch_ClearOffEventFlag(Switch *self)
{
    self->flags.outputOffEvent = 0;
}

/***************************************************************************//**
 * @brief Check if there was an output change event
 * 
 * @param self  pointer to the Switch that you are using
 * 
 * @return true if there was an output change event
 */
bool Switch_OutputChangeEvent(Switch *self)
{
    if(self->flags.switchChangedEvent)
        return true;
    else
        return false;
}

/***************************************************************************//**
 * @brief Clear the output change event flag
 * 
 * @param self  pointer to the Switch that you are using
 */
void Switch_ClearOutputChangeEventFlag(Switch *self)
{
    self->flags.switchChangedEvent = 0;
}

/***************************************************************************//**
 * @brief Check if the switch is currently off
 * 
 * @param self  pointer to the Switch that you are using
 * 
 * @return true if switch is off and off is allowed as a valid state
 */
bool Switch_IsOff(Switch *self)
{
    if(self->type == SW_CENTER_OFF && self->outputState == SW_OUTPUT_OFF)
        return true;
    else
        return false;
}

/***************************************************************************//**
 * @brief Get the current state of output A
 * 
 * @param self  pointer to the Switch that you are using
 * 
 * @return true if output A is currently on
 */
bool Switch_OutputA(Switch *self)
{
    if(self->outputState == SW_OUTPUT_A)
        return true;
    else
        return false;
}

/***************************************************************************//**
 * @brief Get the current state of output B
 * 
 * @param self  pointer to the Switch that you are using
 * 
 * @return true if output B is currently on
 */
bool Switch_OutputB(Switch *self)
{
    if(self->outputState == SW_OUTPUT_B)
        return true;
    else
        return false;
}

/***************************************************************************//**
 * @brief Get the current output of the switch as an enum
 * 
 * @param self  pointer to the Switch that you are using
 * 
 * @return SwitchState  an enum that holds the switch state
 */
SwitchState Switch_GetState(Switch *self)
{
    return(self->outputState);
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
    if(stateChange != 0 && newState == 0)
    {
        if(self->type == SW_CENTER_OFF)
        {
            self->flags.outputOffEvent = 1;
        }
    }
}