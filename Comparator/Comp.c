/***************************************************************************//**
 * @brief Comparator Library
 * 
 * @author Matthew Spinks
 * 
 * @date 5/7/22    Original creation
 * 
 * @file Comp.c
 * 
 * @details
 *      In order to use this library you must define where the "deadzones" are.
 * Imagine a normal analog comparator. It has two output states. The middle 
 * area of the input is a deadzone. The deadzone has an upper and lower bound. 
 * This library can have more than one deadzone and more than two outputs 
 * states. It should work well for hardware that has pre-defined positions or 
 * areas. Imagine something like a slider with detents where the different 
 * regions may not necessarily be equal distance from each other. This is not 
 * as useful for something that has a very large number of output levels that 
 * are equal distance apart from each other. Something like that would be 
 * better suited for a map function.
 * 
 ******************************************************************************/

#include "Comp.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************


// ***** Static Function Prototypes ********************************************


// *****************************************************************************

void Comp_Init(Comp *self, CompDeadzone *deadzones, uint8_t numDeadzones)
{
    if(numDeadzones == 0 )
        return;

    self->deadzones = deadzones;
    self->numDeadzones = numDeadzones;
    self->outputLevel = 0;

    /* Swap thresholds if they are incorrect */
    uint16_t temp;
    for(uint8_t i = 0; i < numDeadzones; i++)
    {
        if(deadzones[i].lower > deadzones[i].upper)
        {
            temp = deadzones[i].lower;
            deadzones[i].lower = deadzones[i].upper;
            deadzones[i].upper = temp;
        }
    }

    /* Sort the array (simple insertion sort) */
    int16_t j;
    CompDeadzone element;
    for(uint8_t i = 1; i < numDeadzones; i++)
    {
        element = deadzones[i];
        j = i - 1;

         /* Going backwards, swap each element in the array with the next one 
         until we find where array[i] is supposed to go. Then, insert element.
         Note that j must be able to be negative. */
        while(j >= 0 && deadzones[j].lower > element.lower)
        {
            deadzones[j+1] = deadzones[j];
            j--;
        }
        deadzones[j+1] = element;
    }

    /* Go through the array and fix any overlapping bounds. If there is an 
    overlap of two deadzones, precedence is given to the lower one. I hope you 
    didn't make your deadzones overlap or the output will be something 
    unexpected. */
    for(uint8_t i = 0; i < numDeadzones - 1; i++)
    {
        if(deadzones[i+1].lower < deadzones[i].upper)
        {
            /* The higher deadzone is overlapping. Push it upwards. */
            deadzones[i+1].lower = deadzones[i].upper;

            if(deadzones[i+1].upper < deadzones[i+1].lower)
            {
                /* Push it up again. What did you do? You should probably
                go double check your array... */
                deadzones[i+1].upper = deadzones[i+1].lower;
            }
        }
    }
    /* I tried to fix your deadzone array. Don't blame me if it's not perfect.*/
}

// *****************************************************************************

void Comp_Process(Comp *self, uint16_t analogInput)
{
    uint8_t output = self->outputLevel;
    uint8_t i = output;
    
    if(output > 0)
        i = output - 1;
    
    if(analogInput <= self->deadzones[i].lower)
    {
        if(output >= 1)
            output--;
    }
    else if(analogInput >= self->deadzones[i].upper)
    {
        if(output < self->numDeadzones)
            output++;
    }

    if(self->outputLevel != output)
    {
        self->outputChangedEvent = true;
        self->outputLevel = output;
    }
}

// *****************************************************************************

bool Comp_GetOutputChangedEvent(Comp *self)
{
    return self->outputChangedEvent;
}

// *****************************************************************************

void Comp_ClearOutputChangedFlag(Comp *self)
{
    self->outputChangedEvent = false;
}

// *****************************************************************************

uint8_t Comp_GetOutputLevel(Comp *self)
{
    return self->outputLevel;
}

/*
 End of File
 */