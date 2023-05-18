/***************************************************************************//**
 * @brief Timer Library
 * 
 * @file Timer.h
 * 
 * @author  Matthew Spinks
 * 
 * @date 12/2/14    Original creation
 * @date 1/17/19    Modified to use OOP
 * @date 10/1/21    Updated documention
 * @date 2/21/22    Added doxygen
 * 
 * @details
 *      A simple, free-running timer that is great for blinking LED's, buttons,
 * sleep timers etc. The period of the timer is based on how fast or slow you 
 * call the tick function. The function expects you to give the tick rate in 
 * milliseconds.
 * 
 * @section license License
 * SPDX-FileCopyrightText: © 2014 Matthew Spinks
 * SPDX-License-Identifier: Zlib
 * 
 * This software is released under the Zlib license. You are free alter and
 * redistribute it, but you must not misrepresent the origin of the software.
 * This notice may not be removed. <http://www.zlib.net/zlib_license.html>
 * 
 ******************************************************************************/

#include "Timer.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************


// ***** Static Function Prototypes ********************************************


// *****************************************************************************

void Timer_InitMs(Timer *self, uint16_t periodMs, uint16_t tickMs)
{
    if(tickMs != 0)
        self->period = periodMs / tickMs;

    self->flags.all = 0;
}

// *****************************************************************************

void Timer_Start(Timer *self)
{
    if(self->period != 0)
        self->flags.start = 1;
}

// *****************************************************************************

void Timer_Stop(Timer *self)
{
    self->flags.start = 0;
    self->flags.active = 0;
}

// *****************************************************************************

void Timer_Tick(Timer *self)
{
    // Check to see if timer is active and ready to start
    if(self->flags.start && self->period != 0)
    {
        self->flags.start = 0;
        self->count = self->period;
        self->flags.active = 1;
    }
    
    // Update active timers
    if(self->flags.active)
    {
        self->count--;
        
        if(self->count == 0)
        {
            self->flags.active = 0;
            self->flags.expired = 1;
            
            if(self->timerCallbackFunc)
            {
                self->timerCallbackFunc(self);
            }
        }
    }
}

// *****************************************************************************

uint16_t Timer_GetCount(Timer *self)
{
    return (self->period - self->count);
}

// *****************************************************************************

uint16_t Timer_GetPeriod(Timer *self)
{
    return self->period;
}

// *****************************************************************************

bool Timer_IsRunning(Timer *self)
{
    if(self->flags.active)
        return true;
    else
        return false;
}

// *****************************************************************************

bool Timer_IsFinished(Timer *self)
{
    if(self->flags.expired)
        return true;
    else
        return false;
}

// *****************************************************************************

void Timer_ClearFlag(Timer *self)
{
    self->flags.expired = 0;
}

// *****************************************************************************

void Timer_SetFinishedCallback(Timer *self, TimerCallbackFunc Function)
{
    self->timerCallbackFunc = Function;
}

/*
 End of File
 */
