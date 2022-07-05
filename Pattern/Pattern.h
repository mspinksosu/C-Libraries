/***************************************************************************//**
 * @brief Pattern Library Header File
 * 
 * @author Matthew Spinks
 * 
 * @date 7/9/22   Original creation
 * 
 * @file Pattern.h
 * 
 * @details
 *      TODO
 * 
 ******************************************************************************/

#ifndef PATTERN_H
#define PATTERN_H

#include <stdint.h>
#include <stdbool.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

typedef struct PatternStateTag
{
    uint8_t output;
    uint16_t timeInMs;
} PatternState;

typedef struct Pattern Pattern;

typedef void (*PatterCallbackFunc)(Pattern *patternContext);

struct Pattern
{
    PatternState *patternArray;
    uint8_t sizeOfArray;
    PatternState *nextPatternArray; // TODO test atomic load 
    uint8_t sizeOfNextArray;

    uint16_t tickMs;
    uint16_t count;
    uint16_t period;
    uint8_t index;
    uint8_t output;

    PatterCallbackFunc patternCallbackFunc;

    union {
        struct {
            unsigned start              :1;
            unsigned active             :1;
            unsigned finished           :1;
            unsigned stopWhenFinished   :1;
            unsigned loadAtomic         :1;
            unsigned                    :0; // fill to nearest byte
        };
        uint8_t all;
    } flags;
};

/** 
 * Description of struct
 * 
 * member1      description of variable member1
 * 
 */

// ***** Function Prototypes ***************************************************

void Pattern_InitMs(Pattern *self, PatternState *arrayOfStates, uint8_t sizeOfArray, uint16_t tickMs);

void Pattern_Load(Pattern *self, PatternState *arrayOfStates, uint8_t sizeOfArray);

void Pattern_LoadAtomic(Pattern *self, PatternState *arrayOfStates, uint8_t sizeOfArray);

void Pattern_Start(Pattern *self);

void Pattern_StopAtomic(Pattern *self);

void Pattern_Stop(Pattern *self);

void Pattern_Tick(Pattern *self);

bool Pattern_IsRunning(Pattern *self);

bool Pattern_IsFinished(Pattern *self);

void Pattern_ClearFlag(Pattern *self);

void Pattern_SetFinishedCallback(Pattern *self)


#endif  /* PATTERN_H */