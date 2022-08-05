/***************************************************************************//**
 * @brief Comparator Library
 * 
 * @author Matthew Spinks
 * 
 * @date 5/6/22     Original creation
 * 
 * @file Comp.c
 * 
 * @details
 *      A comparator with multiple output levels. I decided to define the
 * outputs based on the number of deadzones. This way the comparator can have
 * hysteresis. The number of outputs should be 1 greater than the number of 
 * deadzones. For example an analog button with three states (off, low, high)
 * will have two deadzone areas. The minimum number of deadzones is 1. This 
 * would be a standard comparator with two outputs states: on and off.
 * 
 * In order to create a comparator with multiple output levels, you will need
 * to give it an array of deadzones. The deadzones are defined as an area with 
 * an upper and lower bound. They cannot overlap, otherwise your output may not 
 * change when in a particular state.
 * 
 * The input will be an analog value from 0-65535. The output will be a number 
 * that represents the output level. The output will go from low to high 
 * starting at 0. This makes a convenient way to interface with if-statements
 * if you choose. If you comparator is just a basic, two output comparator with
 * a single deadzone defined, the output is either 0 or 1. If it has two
 * deadzones, the output is 0, 1, 2 and so on.
 * 
 * This is really designed for hardware that has pre-defined positions or areas
 * such as a slide switch or button. The outputs may not always be equal 
 * distance. This is not useful for something that has a large number of output 
 * levels that are equal distance apart from each other. Something like that 
 * would be better suited for a map function.
 * 
 * The output changed event flag is not cleared automatically. You must clear
 * it upon checking for an event.
 * 
 ******************************************************************************/

#include "Comp.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************


// ***** Static Function Prototypes ********************************************


// *****************************************************************************

/***************************************************************************//**
 * @brief Creates a Comparator object
 * 
 * Goes through the array of deadzones that you created and attempts to fix any
 * issues with it. Ideally, your deadzones should be in order from lowest to
 * highest and not overlap with one another.
 * 
 * The number of output levels is one greater than the deadzones. i.e. a basic
 * comparator has two outputs, low/high, and one deadzone in the middle.
 * 
 * @param self  pointer to the Comparator that you are using
 * 
 * @param deadzones  pointer to a deadzone object or an array of deadzones
 * 
 * @param numDeadzones  the number of deadzone objects in the array
 */
void Comp_Create(Comp *self, CompDeadzone *deadzones, uint8_t numDeadzones)
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
        if(deadzones[i+1].lower > deadzones[i].upper)
        {
            /* The higher deadzone is overlapping. Push it upwards. */
            deadzones[i+1].lower = deadzones[i].upper;

            if(deadzones[i+1].upper < deadzones[i+1].lower)
            {   
                /* What did you do? You should go double check your array... */
                deadzones[i+1].upper = deadzones[i+1].lower;
            }
        }
    }
    /* I tried to fix your deadzone array. Don't blame me if it's not perfect.*/
}

/***************************************************************************//**
 * @brief Update the Comparator object with its current value
 * 
 * This function should be called periodically
 * 
 * @param self  pointer to the Comparator that you are using
 * 
 * @param analogInput  0 to 65535
 */
void Comp_Process(Comp *self, uint16_t analogInput)
{
    uint8_t output = self->outputLevel;

    if(analogInput <= self->deadzones[output].lower)
    {
        if(output >= 1)
            output--;
    }
    else if(analogInput >= self->deadzones[output].upper)
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

/***************************************************************************//**
 * @brief Check if there has been an output changed event
 * 
 * @param self  pointer to the Comparator that you are using
 * 
 * @return 
 */
bool Comp_GetOutputChangedEvent(Comp *self)
{
    return self->outputChangedEvent;
}

/***************************************************************************//**
 * @brief Clear the output changed event flag
 * 
 * @param self  pointer to the Comparator that you are using
 */
void Comp_ClearOutputChangedFlag(Comp *self)
{
    self->outputChangedEvent = false;
}

/***************************************************************************//**
 * @brief Return the current output level of the Comparator object
 * 
 * The number of different levels is one geater than the amount of deadzones
 * that you define for your Comparator object
 * 
 * @param self  pointer to the Comparator that you are using
 * 
 * @return uint8_t  output level
 */
uint8_t Comp_GetOutputLevel(Comp *self)
{
    return self->outputLevel;
}

/*
 End of File
 */