/***************************************************************************//**
 * @brief Comparator Library
 * 
 * @file Comp.c
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 5/7/22    Original creation
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
 * @section license License
 * SPDX-FileCopyrightText: © 2022 Matthew Spinks
 * SPDX-License-Identifier: Zlib
 * 
 * This software is released under the Zlib license. You are free alter and
 * redistribute it, but you must not misrepresent the origin of the software.
 * This notice may not be removed. <http://www.zlib.net/zlib_license.html>
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
}

// *****************************************************************************

void Comp_UpdateValue(Comp *self, uint16_t analogInput)
{
    uint8_t output = self->outputLevel;
    uint16_t lowerThreshold = 0, upperThreshold = 0xFFFF;
    
    if(output > 0)
        lowerThreshold = self->deadzones[output - 1].lower;
    
    if(output < self->numDeadzones)
        upperThreshold = self->deadzones[output].upper;

    if(analogInput < lowerThreshold)
    {
        output--;
    }
    else if(analogInput > upperThreshold)
    {
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