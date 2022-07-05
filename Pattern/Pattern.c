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
 * @param self 
 * @param arrayOfStates 
 * @param sizeOfArray 
 * @param tickMs 
 */
void Pattern_InitMs(Pattern *self, PatternState *arrayOfStates, uint8_t sizeOfArray, uint16_t tickMs)
{
    if(sizeOfArray == 0)
        return;

    /* The count for a given state is calculated by dividing the period in 
    milliseconds by the tick in milliseconds. If your tick rate is larger
    than the period, it will be set to 1. I will also set the tick value to
    1 if you accidentally set it to zero. */
    self->patternArray = arrayOfStates;
    self->sizeOfArray = sizeOfArray;
    self->count = 0;
    self->index = 0;
    self->flags.all = 0;
    self->tickMs = 1;

    if(tickMs != 0)
        self->tickMs = tickMs;

    /* Preload the first count? */
    //self->period =  self->patternArray[self->index].timeInMs / self->tickMs;
}

void Pattern_Load(Pattern *self, PatternState *arrayOfStates, uint8_t sizeOfArray)
{
    if(sizeOfArray == 0)
        return;

    /* Force pattern restart */
    self->flags.start = 0;
    self->flags.active = 0;
    self->flags.finished = 0;
    self->flags.loadAtomic = 0;
    self->patternArray = arrayOfStates;
    self->sizeOfArray = sizeOfArray;
    self->count = 0;
    self->index = 0;

    /* TODO Should I load the output immediately? */
    self->output = self->patternArray[self->index].output;
    self->flags.start = 1;
}

void Pattern_LoadAtomic(Pattern *self, PatternState *arrayOfStates, uint8_t sizeOfArray)
{
    if(sizeOfArray == 0)
        return;

    self->flags.loadAtomic = 1;
    self->nextPatternArray = arrayOfStates;
    self->sizeOfNextArray = sizeOfArray;
}

void Pattern_Start(Pattern *self)
{
    self->flags.start = 1;
}

void Pattern_StopAtomic(Pattern *self)
{
    self->flags.stopWhenFinished = 1;
}

void Pattern_Stop(Pattern *self)
{
    self->flags.start = 0;
    self->flags.active = 0;
}

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
            if(self->index + 1 == self->sizeOfArray)
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
                    self->sizeOfArray = self->sizeOfNextArray;
                }

                if(self->flags.stopWhenFinished == 0)
                {
                    self->flags.start = 1;
                }
            }
            else
            {
                self->index++;
                self->flags.start = 1;
            }
        }
    }
}

/*
 End of File
 */