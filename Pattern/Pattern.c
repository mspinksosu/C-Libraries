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
 *      A library that makes simple patterns. This is basically a software 
 * timer with some special atomic functions added the make loading a new 
 * pattern very easy. It's not meant to be an extremely accurate timer, but it 
 * can work very well depending on the tick rate you choose and how well you 
 * stick to it. When the pattern gets ready to to go to the next state, it will 
 * calculate the period in ticks. The period in ticks is truncated based on the 
 * tick rate you initialized your pattern with. If your period ends up being 
 * smaller than your tick rate, the period will be set to one. If you 
 * accidentally set your tick rate to zero, I will set it to one as well. 
 * 
 ******************************************************************************/

#include "Pattern.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************


// ***** Static Function Prototypes ********************************************


// *****************************************************************************

void Pattern_InitMs(Pattern *self, uint16_t tickMs)
{
    /* The count for a given state is calculated by dividing the period in 
    milliseconds by the tick in milliseconds. If your tick rate is larger
    than the period, it will be set to 1. I will also set the tick value to
    1 if you accidentally set it to zero. */
    self->count = 0;
    self->index = 0;
    self->flags.all = 0;
    self->tickMs = 1;

    if(tickMs != 0)
        self->tickMs = tickMs;
}

// *****************************************************************************

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

    /* TODO Should I start the timer automatically? */
    self->output = self->patternArray[self->index].output;
    self->flags.start = 1;
}

// *****************************************************************************

void Pattern_LoadAtomic(Pattern *self, PatternState *arrayOfStates, uint8_t numOfStates)
{
    if(numOfStates == 0)
        return;

    self->nextPatternArray = arrayOfStates;
    self->nextNumOfStates = numOfStates;
    self->flags.loadAtomic = 1;
}

// *****************************************************************************

void Pattern_Start(Pattern *self)
{
    if(self->numOfStates == 0)
        return;

    self->flags.start = 1;
    self->flags.stopWhenFinished = 0;
}

// *****************************************************************************

void Pattern_StopAtomic(Pattern *self)
{
    self->flags.stopWhenFinished = 1;
}

// *****************************************************************************

void Pattern_Stop(Pattern *self)
{
    self->flags.start = 0;
    self->flags.active = 0;
}

// *****************************************************************************

void Pattern_Tick(Pattern *self)
{
    /* Check if timer is ready to start */
    if(self->flags.start)
    {
        self->flags.start = 0;
        self->period =  self->patternArray[self->index].timeInMs / self->tickMs;

        /* If the period is less than 1, make it to 1 for the user */
        if(self->period == 0)
            self->period = 1;

        /* Load the count and set the output */
        self->count = self->period;
        self->output = self->patternArray[self->index].output;
        self->flags.active = 1;

        if(self->outputChangedCallback)
            self->outputChangedCallback(self->output);
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

                /* Callback function with context */
                if(self->patternFinishedCallback)
                    self->patternFinishedCallback(self);

                if(self->flags.loadAtomic)
                {
                    /* Load the next pattern */
                    self->flags.loadAtomic = 0;
                    self->patternArray = self->nextPatternArray;
                    self->numOfStates = self->nextNumOfStates;
                }

                if(self->flags.stopWhenFinished)
                {
                    self->flags.stopWhenFinished = 0;
                    self->flags.active = 0;
                }
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

// *****************************************************************************

uint8_t Pattern_GetOutput(Pattern *self)
{
    return self->output;
}

// *****************************************************************************

bool Pattern_IsRunning(Pattern *self)
{
    if(self->flags.active)
        return true;
    else
        return false;
}

// *****************************************************************************

bool Pattern_IsFinished(Pattern *self)
{
    if(self->flags.finished)
        return true;
    else
        return false;
}

// *****************************************************************************

void Pattern_ClearFlag(Pattern *self)
{
    self->flags.finished = 0;
}

// *****************************************************************************

void Pattern_SetFinishedCallback(Pattern *self,PatternCallbackFunc Function)
{
    self->patternFinishedCallback = Function;
}

// *****************************************************************************

void Pattern_SetOutputChangedCallback(Pattern *self, void (*Function)(uint8_t))
{
    self->outputChangedCallback = Function;
}

/*
 End of File
 */