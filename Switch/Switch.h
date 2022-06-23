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
 * are two inputs and one output. The output A and B cannot be both true. 
 * Optional "center off" that allows output to be off. The default is to not 
 * allow the output to be off.
 * 
 ******************************************************************************/

#ifndef SWITCH_H
#define SWITCH_H

#include <stdint.h>
#include <stdbool.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

// A is LSB, B is bit 1
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
    SwitchState outputState;
    SwitchType type;
    SwitchDebounceState debounceState;

    uint16_t debouncePeriod;
    uint16_t debounceCounter;

    // bitfield for events
    union {
        struct {
            unsigned switchChangedEvent     :1;
            unsigned outputAOnEvent         :1;
            unsigned outputBOnEvent         :1;
            unsigned outputOffEvent         :1;
            unsigned                        :0;
        };
    } flags;
} Switch;

// ***** Function Prototypes ***************************************************

void Switch_Init(Switch *self, uint16_t debounceMs, uint16_t tickMs, bool inputAInit, bool inputBInit);

void Switch_InitWithCenterOff(Switch *self, uint16_t debounceMs, uint16_t tickMs, bool inputAInit, bool inputBInit, bool offAllowed);

void Switch_Tick(Switch *self, bool inputAIsOn, bool inputBIsOn);

bool Switch_GetOutputAOnEvent(Switch *self);

void Switch_ClearOutputAOnEventFlag(Switch *self);

bool Switch_GetOutputBOnEvent(Switch *self);

void Switch_ClearOutputBOnEventFlag(Switch *self);

bool Switch_GetOffEvent(Switch *self);

void Switch_ClearOffEventFlag(Switch *self);

bool Switch_OutputChangeEvent(Switch *self);

void Switch_ClearOutputChangeEventFlag(Switch *self);

bool Switch_IsOff(Switch *self);

bool Switch_OutputA(Switch *self);

bool Switch_OutputB(Switch *self);

SwitchState Switch_GetState(Switch *self);

#endif	/* TRIGGER_H */