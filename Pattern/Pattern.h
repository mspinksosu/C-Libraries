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

typedef void (*PatternCallbackFunc)(Pattern *patternContext);

struct Pattern
{
    PatternState *patternArray;
    uint8_t numOfStates;
    PatternState *nextPatternArray; // TODO test atomic load 
    uint8_t nextNumOfStates;

    uint16_t tickMs;
    uint16_t count;
    uint16_t period;
    uint8_t index;
    uint8_t output;

    PatternCallbackFunc patternCallbackFunc;

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
 * Description of struct members. You shouldn't really mess with any of these
 * variables directly. That is why I made functions for you to use.
 * 
 * patternArray  Pointer to an array of PatternState variables
 * 
 * numOfStates  The number PatternState variables in the array
 * 
 * nextPatternArray  Pointer to the next pattern to load (atomic load)
 * 
 * nextNumOfStates  The number of PatternState variables in the next array
 * 
 * tickMs  How often you call the tick function (in milliseconds)
 * 
 * count  Counts the number of ticks in each pattern state
 * 
 * period  The number of ticks to remain in a state
 * 
 * index  The position in the PatternState array
 * 
 * output  The output of the pattern at any time (can be up to 8-bits)
 * 
 * start  When this flag is set, the pattern begins
 * 
 * active  This bit is 1 whenever the pattern is running. Clear this bit to 
 *         stop the pattern.
 * 
 * finished  This flag is set when the pattern reaches the last pattern in the
 *           array. It is not cleared automatically
 * 
 * stopWhenFinished  Tells the pattern to stop at the end of the array, or to
 *                   continue starting over at index 0.
 * 
 * loadAtomic  If this bit is set, when the pattern finishes the pattern
 *             pointed to by nextPatternArray will be loaded.
 */

// ***** Function Prototypes ***************************************************

/***************************************************************************//**
 * @brief Initialize a Pattern Object
 * 
 * Uses an array of states that define the output state and the time for that
 * state in milliseconds. The pattern will always start at index zero of the 
 * array. Be sure to pass the number of states and not the size of the array 
 * itself.
 * 
 * @param self  pointer to the Pattern you are using
 * 
 * @param arrayOfStates  pointer to a single PatternState or an array
 * 
 * @param numOfStates  the number of states (not the size of the array)
 * 
 * @param tickMs  how often you plan to call the Pattern Tick function
 */
void Pattern_InitMs(Pattern *self, PatternState *arrayOfStates, uint8_t numOfStates, uint16_t tickMs);

/***************************************************************************//**
 * @brief Load a new pattern immediately
 * 
 * Stops the current pattern and loads the new pattern, beginning at index 0
 * 
 * @param self  pointer to the Pattern you are using
 * 
 * @param arrayOfStates  pointer to a single PatternState or an array
 * 
 * @param numOfStates  the number of states (not the size of the array)
 */
void Pattern_Load(Pattern *self, PatternState *arrayOfStates, uint8_t numOfStates);

/***************************************************************************//**
 * @brief  Load a new pattern after the current one finishes
 * 
 * Ready the next pattern and set a flag. When the current pattern finishes,
 * the new pattern is loaded. Normally the pattern is running when this
 * function is called.
 * 
 * @param self  pointer to the Pattern you are using
 * 
 * @param arrayOfStates  pointer to a single PatternState or an array
 * 
 * @param numOfStates  the number of states (not the size of the array)
 */
void Pattern_LoadAtomic(Pattern *self, PatternState *arrayOfStates, uint8_t numOfStates);

/***************************************************************************//**
 * @brief Start the pattern
 * 
 * The start function always takes precedence over stop atomic. If you want to
 * display a pattern once, call start and then stop atomic. The start function
 * will not restart the pattern for you either. This can useful when switching
 * back and forth between patterns. If you need to restart from index zero
 * use Pattern_Load.
 * 
 * @param self  pointer to the Pattern you are using
 */
void Pattern_Start(Pattern *self);

/***************************************************************************//**
 * @brief Stop the pattern after it reaches the end of the pattern array
 * 
 * @param self  pointer to the Pattern you are using
 */
void Pattern_StopAtomic(Pattern *self);

/***************************************************************************//**
 * @brief Stop the pattern immediately
 * 
 * @param self  pointer to the Pattern you are using
 */
void Pattern_Stop(Pattern *self);

/***************************************************************************//**
 * @brief Update the pattern
 * 
 * Once the pattern reaches the size that you specify it will go back to index
 * zero. If you have it set to stop when finished, it will stop. Otherwise, it
 * will loop around again.
 * 
 * After every loop through the pattern, there will be a callback function call
 * if it is enabled.
 * 
 * If you called load atomic during the middle of the pattern, it will load the 
 * next pattern when it finishes. Load atomic does not alter the start and stop
 * bits.
 * 
 * @param self 
 */
void Pattern_Tick(Pattern *self);

/***************************************************************************//**
 * @brief Get the current output of the pattern
 * 
 * The output can be used for up to 8 GPIO or anything else you want
 * 
 * @param self  pointer to the Pattern you are using
 * 
 * @return uint8_t  the output
 */
uint8_t Pattern_GetOutput(Pattern *self);

/***************************************************************************//**
 * @brief Check if the pattern is running
 * 
 * @param self  pointer to the Pattern that you are using
 *
 * @return  true if the pattern is running
 */
bool Pattern_IsRunning(Pattern *self);

/***************************************************************************//**
 * @brief Check if the pattern is finished
 * 
 * This flag is not cleared automatically.
 * 
 * @param self  pointer to the Pattern that you are using
 *
 * @return  true if pattern is finished
 */
bool Pattern_IsFinished(Pattern *self);

/***************************************************************************//**
 * @brief Clear the pattern finished flag
 * 
 * @param self  pointer to the Pattern that you are using
 */
void Pattern_ClearFlag(Pattern *self);

/***************************************************************************//**
 * @brief Set a function to be called when the pattern finishes
 * 
 * The function prototype must have a pointer to a Pattern as its argument. 
 * The context is so that multiple callbacks can be serviced by the same 
 * function if desired.
 * 
 * @param self  pointer to the Pattern that you are using
 * 
 * @param Function  format: void SomeFunction(Pattern *context)
 */
void Pattern_SetFinishedCallback(Pattern *self, PatternCallbackFunc Function);


#endif  /* PATTERN_H */