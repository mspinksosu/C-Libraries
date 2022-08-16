/***************************************************************************//**
 * @brief Switch Library Header File
 * 
 * @author Matthew Spinks
 * 
 * @date January 5, 2022  Original creation
 * 
 * @file Switch.h
 * 
 * @details
 *      A library that handles basic features of a "dual throw" switch. There
 * are two inputs and one output. The output can either be A or B, but not
 * both at the same time. There is an optional "center off" feature that allows 
 * output to be off. The default is no center-off.
 * 
 *      To create a Switch, you need a Switch object, the debounce time in 
 * milliseconds and the update rate in milliseconds. (This is how often you plan 
 * to periodically call the tick function.) If you are using hardware to 
 * debounce your switch inputs, you can set the debounce time to zero. You can 
 * also set the tick rate to zero, but you still must call the Tick function,
 * or else the output will never change. (Honestly though, if you are already
 * debouncing your inputs, why are you using this library at all? That's pretty
 * much the only feature here except maybe the center lockout.)
 * 
 *      Be sure to give the initialization function the initial value or else
 * you will get a false event on startup. Also, if you would like to use this 
 * for a single pole switch, simply set one of the arguments to the tick
 * function to always be false.
 * 
 *      There are flags for different events such as "A on", "B on", as well as 
 * any output change. I am providing multiple different getter functions for 
 * states and events. How you determine the output is entirely up to you. The
 * events flags are not cleared automatically. There may be times when you 
 * don't want to immediately clear a flag.
 * 
 ******************************************************************************/

#ifndef SWITCH_H
#define SWITCH_H

#include <stdint.h>
#include <stdbool.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

// A is bit 0, B is bit 1
typedef enum SwitchStateTag
{
    SW_OUTPUT_OFF = 0x00,
    SW_OUTPUT_A = 0x01,
    SW_OUTPUT_B = 0x02,
    SW_OUTPUT_INVALID = 0x03,
} SwitchState;

typedef enum SwitchTypeTag
{
    SW_NORMAL,
    SW_CENTER_OFF,
} SwitchType;

typedef enum DebounceTag
{
    SW_READY = 0,
    SW_DEBOUNCING,
} SwitchDebounceState;

typedef struct SwitchTag
{
    uint16_t debouncePeriod;
    uint16_t debounceCounter;

    SwitchState outputState;
    SwitchType type;
    SwitchDebounceState debounceState;

    union {
        struct {
            unsigned switchChangedEvent :1;
            unsigned outputAOnEvent     :1;
            unsigned outputBOnEvent     :1;
            unsigned outputOffEvent     :1;
            unsigned                    :4;
        };
        uint8_t all;
    } flags;
} Switch;

/** 
 * Description of struct members. You shouldn't really mess with any of these
 * variables directly. That is why I made functions for you to use.
 * 
 * outputState  The current output of the switch, expressed as an enum
 * 
 * type  The type of switch. Normal or center-off type
 * 
 * debounceState  Used for the state machine
 * 
 * debouncePeriod  The number of ticks to debounce the switch input
 * 
 * debounceCounter  Counts the number of ticks for debouncing
 * 
 * switchChangedEvent  The output of the switch has changed
 * 
 * outputAOnEvent  This flag is set when output A changes from off to on
 * 
 * outputBOnEvent  This flag is set when output B changes from off to on
 * 
 * outputOffEvent  This flag is set when output changes from either A or B to 
 *                 off. Only works if the center-off feature is turned on
 */

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Function Prototypes *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Initializes a "SPDT" Switch object
 * 
 * @param self  pointer to the Switch that you are using
 * @param debounceMs  time to debounce the switch in milliseconds
 * @param tickMs  how often you plan to call the Switch Tick function
 * @param inputAInit  the initial state of input A (true = On)
 * @param inputBInit  the initial state of input B (true = On)
 * @param centerOffAllowed  true = allow the off state to be valid
 */
void Switch_Init(Switch *self, uint16_t debounceMs, uint16_t tickMs, bool inputAInit, bool inputBInit, bool centerOffAllowed);

/***************************************************************************//**
 * @brief Enable or disable the "center-off" feature
 * 
 * This will allow you change the type of switch after initializing it, if you
 * want to do that.
 * 
 * @param self  pointer to the Switch that you are using
 * 
 * @param centerOffAllowed  true = allow the off state to be valid
 */
void Switch_SetCenterOff(Switch *self, bool centerOffAllowed);

/***************************************************************************//**
 * @brief Update the Switch with its current status
 * 
 * @param self  pointer to the Switch that you are using
 * 
 * @param inputAIsOn  true if input A is on
 * 
 * @param inputBIsOn  true if input B is on
 */
void Switch_Tick(Switch *self, bool inputAIsOn, bool inputBIsOn);

/***************************************************************************//**
 * @brief Check if there was an output B on event
 * 
 * @param self  pointer to the Switch that you are using
 * 
 * @return true true if output A changed from off to on
 */
bool Switch_GetOutputAOnEvent(Switch *self);

/***************************************************************************//**
 * @brief Clear the output A on event flag
 * 
 * @param self  pointer to the Switch that you are using
 */
void Switch_ClearOutputAOnEventFlag(Switch *self);

/***************************************************************************//**
 * @brief Check if there was an output B on event
 * 
 * @param self  pointer to the Switch that you are using
 * 
 * @return true if output B changed from off to on
 */
bool Switch_GetOutputBOnEvent(Switch *self);

/***************************************************************************//**
 * @brief Clear the output B on event flag
 * 
 * @param self  pointer to the Switch that you are using
 */
void Switch_ClearOutputBOnEventFlag(Switch *self);

/***************************************************************************//**
 * @brief Check if there was an output off event
 * 
 * @param self  pointer to the Switch that you are using
 * 
 * @return true if there was an output off event
 */
bool Switch_GetOffEvent(Switch *self);

/***************************************************************************//**
 * @brief Clear the output off event flag
 * 
 * @param self  pointer to the Switch that you are using
 */
void Switch_ClearOffEventFlag(Switch *self);

/***************************************************************************//**
 * @brief Check if there was an output change event
 * 
 * @param self  pointer to the Switch that you are using
 * 
 * @return true if there was an output change event
 */
bool Switch_OutputChangeEvent(Switch *self);

/***************************************************************************//**
 * @brief Clear the output change event flag
 * 
 * @param self  pointer to the Switch that you are using
 */
void Switch_ClearOutputChangeEventFlag(Switch *self);

/***************************************************************************//**
 * @brief Check if the switch is currently off
 * 
 * @param self  pointer to the Switch that you are using
 * 
 * @return true if switch is off and off is allowed as a valid state
 */
bool Switch_IsOff(Switch *self);

/***************************************************************************//**
 * @brief Get the current state of output A
 * 
 * @param self  pointer to the Switch that you are using
 * 
 * @return true if output A is currently on
 */
bool Switch_OutputA(Switch *self);

/***************************************************************************//**
 * @brief Get the current state of output B
 * 
 * @param self  pointer to the Switch that you are using
 * 
 * @return true if output B is currently on
 */
bool Switch_OutputB(Switch *self);

/***************************************************************************//**
 * @brief Get the raw value of the switch as an enum.
 * 
 * This can return the SW_OUTPUT_OFF regardless of whether or not the
 * center-off feature is enabled. Be careful. If you want to be safer, use the
 * OutputA, OutputB, and IsOff functions instead.
 * 
 * @param self  pointer to the Switch that you are using
 * 
 * @return SwitchState  an enum that holds the switch state
 */
SwitchState Switch_GetState(Switch *self);

#endif /* TRIGGER_H */