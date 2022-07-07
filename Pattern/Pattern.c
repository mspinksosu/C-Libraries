/***************************************************************************//**
 * @brief Pattern Library
 * 
 * @author Matthew Spinks
 * 
 * @date 7/9/22   Original creation
 * 
 * @file Pattern.c
 * 
 * @details
 *      TODO
 * 
 ******************************************************************************/

#include "Pattern.h"

// ***** Defines ***************************************************************


// ***** Function Prototypes ***************************************************


// ***** Global Variables ******************************************************

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
void Pattern_InitMs(Pattern *self, PatternState *arrayOfStates, uint8_t numOfStates, uint16_t tickMs)
{
    if(numOfStates == 0)
        return;

    /* The count for a given state is calculated by dividing the period in 
    milliseconds by the tick in milliseconds. If your tick rate is larger
    than the period, it will be set to 1. I will also set the tick value to
    1 if you accidentally set it to zero. */
    self->patternArray = arrayOfStates;
    self->numOfStates = numOfStates;
    self->count = 0;
    self->index = 0;
    self->flags.all = 0;
    self->tickMs = 1;

    if(tickMs != 0)
        self->tickMs = tickMs;
}

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
void Pattern_Load(Pattern *self, PatternState *arrayOfStates, uint8_t numOfStates)
{
    if(numOfStates == 0)
        return;

    /* Force pattern restart */
    self->flags.start = 0;
    self->flags.active = 0;
    self->flags.finished = 0;
    self->flags.loadAtomic = 0;
    self->patternArray = arrayOfStates;
    self->numOfStates = numOfStates;
    self->count = 0;
    self->index = 0;

    /* TODO Should I load the output immediately? */
    self->output = self->patternArray[self->index].output;
    self->flags.start = 1;
}

/***************************************************************************//**
 * @brief  Load a new pattern after the current one finishes
 * 
 * Ready the next pattern and set a flag. When the current pattern finishes,
 * the new pattern is loaded. Normally, the pattern is running when this
 * function is called.
 * 
 * @param self  pointer to the Pattern you are using
 * 
 * @param arrayOfStates  pointer to a single PatternState or an array
 * 
 * @param numOfStates  the number of states (not the size of the array)
 */
void Pattern_LoadAtomic(Pattern *self, PatternState *arrayOfStates, uint8_t numOfStates)
{
    if(numOfStates == 0)
        return;

    self->nextPatternArray = arrayOfStates;
    self->nextNumOfStates = numOfStates;
    self->flags.loadAtomic = 1;
}

/***************************************************************************//**
 * @brief Start the pattern
 * 
 * @param self  pointer to the Pattern you are using
 */
void Pattern_Start(Pattern *self)
{
    self->flags.start = 1;
}

/***************************************************************************//**
 * @brief Stop the pattern after it reaches the end of the pattern array
 * 
 * @param self  pointer to the Pattern you are using
 */
void Pattern_StopAtomic(Pattern *self)
{
    self->flags.stopWhenFinished = 1;
}

/***************************************************************************//**
 * @brief Stop the pattern immediately
 * 
 * @param self  pointer to the Pattern you are using
 */
void Pattern_Stop(Pattern *self)
{
    self->flags.start = 0;
    self->flags.active = 0;
}

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
void Pattern_Tick(Pattern *self)
{
    /* Check if timer is ready to start */
    if(self->flags.start)
    {
        /* Load the count and set the output */
        self->flags.start = 0;
        self->period =  self->patternArray[self->index].timeInMs / self->tickMs;

        /* TODO If the period is less than 1, should I set it to 1 for the user? */
        if(self->period == 0)
            self->period = 1;

        self->count = self->period;
        self->output = self->patternArray[self->index].output;
        self->flags.active = 1;
    }

    /* Update the timer */
    if(self->flags.active)
    {
        self->count--;

        if(self->count == 0)
        {
            /* Check if we are done */
            if(self->index + 1 == self->numOfStates)
            {
                /* We finished the pattern */
                self->flags.finished = 1;
                self->index = 0;

                // TODO callback

                if(self->flags.loadAtomic)
                {
                    /* Load the next pattern */
                    self->flags.loadAtomic = 0;
                    self->patternArray = self->nextPatternArray;
                    self->numOfStates = self->nextNumOfStates;
                }

                if(self->flags.stopWhenFinished)
                    self->flags.active = 0;
                else
                    self->flags.start = 1;
            }
            else
            {
                self->index++;
                self->flags.start = 1;
            }
        }
    }
}

/***************************************************************************//**
 * @brief Get the current output of the pattern
 * 
 * The output can be used for up to 8 GPIO or anything else you want
 * 
 * @param self  pointer to the Pattern you are using
 * 
 * @return uint8_t  the output
 */
uint8_t Pattern_GetOutput(Pattern *self)
{
    return self->output;
}

/***************************************************************************//**
 * @brief Check if the pattern is running
 * 
 * @param self  pointer to the Pattern that you are using
 *
 * @return  true if the pattern is running
 */
bool Pattern_IsRunning(Pattern *self)
{
    if(self->flags.active)
        return true;
    else
        return false;
}

/***************************************************************************//**
 * @brief Check if the pattern is finished
 * 
 * This flag is not cleared automatically.
 * 
 * @param self  pointer to the Pattern that you are using
 *
 * @return  true if pattern is finished
 */
bool Pattern_IsFinished(Pattern *self)
{
    if(self->flags.finished)
        return true;
    else
        return false;
}

/***************************************************************************//**
 * @brief Clear the pattern finished flag
 * 
 * @param self  pointer to the Pattern that you are using
 */
void Pattern_ClearFlag(Pattern *self)
{
    self->flags.finished = 0;
}

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
void Pattern_SetFinishedCallback(Pattern *self,PatterCallbackFunc Function)
{
    self->patternCallbackFunc = Function;
}

/*
 End of File
 */