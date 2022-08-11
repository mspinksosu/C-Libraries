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
 *      A library that makes simple patterns. It's very useful for blinking 
 * LED's. LED's are good for diagnostics. It starts out simple enough. One or 
 * two LED's and some flags usually. But at some point we get asked to add more 
 * patterns and different conditions. Then we get asked if we can make it flash 
 * different error codes to the customer. Now it becomes a little bit more 
 * complicated and we have to spend some time making the blinken lights look 
 * just right. This library can help you do all of that.
 * 
 * To make a Pattern, you will need to make an array of PatternStates. The
 * PatternState defines what the output should be and for how long (in
 * milliseconds). When you initialize the Pattern, you pass a reference to your 
 * array of PatternStates and the number of states (be careful not to give the
 * size of the array in bytes). You also need the expected update rate in 
 * milliseconds. This is how often you plan to call the tick function. The time 
 * periods are truncated based on your tick rate.
 * 
 * When the pattern reaches the final state, a few things can happen depending
 * on what flags are set. Normally, the pattern loops around and starts again
 * at index zero. I've included some special functions called "atomic", which 
 * let the pattern run to completion. For example, calling the Stop function
 * pauses the pattern immediately. But calling StopAtomic stops the pattern
 * after if reaches the end. Likewise, the Load function loads a pattern and 
 * immediately starts regardless of what state it was in previously. But the
 * LoadAtomic function will wait until the pattern finishes and then load the
 * next pattern and begin. This is very useful for ensuring there are no little
 * "blips" when switching patterns.
 * 
 * The output is read by calling the GetOutput function. The output is just an
 * 8-bit value. It can be 1 or 0. Or it can have multiple outputs together. 
 * It can be used to flash patterns on an battery level gauge for example. Or 
 * to make a cool Knight Rider sequence. How you choose to decode the output is 
 * entirely up to you.
 * 
 * When the pattern finishes a flag is set. The flag is not cleared 
 * automatically. This is for you to decide when to clear it. There is also a 
 * pattern finished callback function. It is always called at the end of the 
 * pattern regardless if the pattern loops or not. The function you create must 
 * follow the prototype listed for PatternCallbackFunc. It must have a pointer 
 * to a Pattern object as an argument. This is so you can have multiple 
 * patterns pointing to the same callback function if you desire. Inside your 
 * callback function, you can look at the context pointer to see which Pattern 
 * called the function and decide what to do.
 * 
 * Example Code:
 *      Pattern ledBlink;
 *      PatternState pattern1Array[2] = {{.output = 1, .timeInMs = 32},
 *                                       {.output = 0, .timeInMs = 468}};
 *      PatternState pattern2Array[3] = {{.output = 0x01, .timeInMs = 250},
 *                                       {.output = 0x02, .timeInMs = 250},
 *                                       {.output = 0x00, .timeInMs = 5}};
 *      Pattern_InitMs(&ledBlink, &pattern1Array[0], 2, TICK_ONE_MS);
 *      Pattern_Tick(&ledBlink); // call every 1 ms
 *      ledOutput = Pattern_GetOutput(&ledBlink); // set GPIO
 *      if(stopBlinking == true)
 *          Pattern_StopAtomic(&ledBlink);
 *      if(changePattern == true)
 *          Pattern_LoadAtomic(&ledBlink, &pattern2Array[0], 3, TICK_ONE_MS);
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
    PatternState *nextPatternArray;
    uint8_t nextNumOfStates;

    uint16_t tickMs;
    uint16_t count;
    uint16_t period;
    uint8_t index;
    uint8_t output;

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

    PatternCallbackFunc patternFinishedCallback;
    void (*outputChangedCallback)(uint8_t outputState);
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

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Function Prototypes *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Initialize a Pattern Object
 * 
 * Sets the expected tick rate and all the initial values
 * 
 * @param self  pointer to the Pattern you are using
 * 
 * @param tickMs  how often you plan to call the Pattern Tick function
 */
void Pattern_InitMs(Pattern *self, uint16_t tickMs);

/***************************************************************************//**
 * @brief Load a new pattern immediately
 * 
 * Stops the current pattern and loads the new pattern, beginning at index 0.
 * Be sure to pass the number of states and not the size of the array in bytes.
 * 
 * @param self  pointer to the Pattern you are using
 * 
 * @param arrayOfStates  pointer to a single PatternState or an array
 * 
 * @param numOfStates  the number of states in the array
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
 * will not restart the pattern for you either. If you need to restart from 
 * index zero use Pattern_Load.
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
 * @param self  pointer to the Pattern you are using
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

/***************************************************************************//**
 * @brief Set a function to be called anytime the output changes
 * 
 * What's that? You said you wanted to have the function to set your GPIO to be
 * called automatically for you? Without it needing to include this header?
 * Don't worry fam. I got you.
 * 
 * @param self  pointer to the Pattern that you are using
 * 
 * @param Function  format: void SomeFunction(uint8_t outputState)
 */
void Pattern_SetOutputChangedCallback(Pattern *self, void (*Function)(uint8_t));

#endif  /* PATTERN_H */