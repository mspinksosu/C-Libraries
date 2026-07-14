/***************************************************************************//**
 * @brief Device Monitor Library
 * 
 * @file DeviceMonitor.c
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 7/14/26   Original creation
 * 
 * @details
 *      TODO
 * 
 * @section license License
 * SPDX-FileCopyrightText: © 2026 Matthew Spinks
 * SPDX-License-Identifier: Zlib
 * 
 * This software is released under the Zlib license. You are free alter and
 * redistribute it, but you must not misrepresent the origin of the software.
 * This notice may not be removed. <http://www.zlib.net/zlib_license.html>
 * 
 ******************************************************************************/

#include "DeviceMonitor.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************


// ***** Static Functions Prototypes *******************************************


// *****************************************************************************

void DeviceMonitor_Init(DeviceMonitor *self, DeviceMonitorInitType *params)
{
    if(params->timerTickRateMs == 0)
        params->timerTickRateMs = 1;

    self->initPeriod = params->initPeriodMs / params->timerTickRateMs;
    self->watchdogPeriod = params->watchdogPeriodMs / params->timerTickRateMs;

    if(self->initPeriod == 0)
        self->initPeriod = 1;
    if(self->watchdogPeriod == 0)
        self->watchdogPeriod = 1;

    self->timer.flags.all = 0;
    self->initRetryCount = 0;
    self->initWaitEnabled = params->initWaitEnabled;
    self->initRetryEnabled = params->initRetryEnabled;
    self->watchdogTimerEnabled = params->watchdogTimerEnabled;
    self->initRetryLimit = params->initRetryLimit;
    self->state = DEVICE_MONITOR_STATE_POR;
}

// *****************************************************************************

void DeviceMonitor_Tick(DeviceMonitor *self)
{
    bool enterRunState = false;

    switch(self->state)
    {
        case DEVICE_MONITOR_STATE_POR:
            if(self->initWaitEnabled)
            {
                self->timer.period = self->initPeriod;
                self->timer.flags.expired = 0;
                self->timer.flags.start = 1;
                self->state = DEVICE_MONITOR_STATE_INIT_WAIT;
            }
            else
            {
                enterRunState = true;
            }
            break;
        case DEVICE_MONITOR_STATE_INIT_WAIT:
            /* Restart the pressure sensor if it fails on startup. */
            if(self->timer.flags.expired)
            {
                self->timer.flags.expired = 0;

                // @todo timer finished callback
                // @todo if successful init, go to run state

                if(self->initRetryEnabled && self->initRetryLimit > 0)
                {
                    self->initRetryCount++;
                    if(self->initRetryCount <= self->initRetryLimit)
                    {
                        // @todo retry timer expired callback
                        // @todo if successful init, don't restart timer, go to run state
                        self->timer.flags.start = 1;
                    }
                    else
                    {
                        // @todo retry limit reached callback
                        enterRunState = true;
                    }
                }
                else
                {
                    enterRunState = true;
                }
            }
            break;
        case DEVICE_MONITOR_STATE_RUN:
            /* Reset the sensor if it times out */
            if(self->timer.flags.expired)
            {
                self->timer.flags.expired = 0;
                if(self->watchdogTimerEnabled)
                {
                    // @todo timeout callback
                    self->timer.flags.start = 1;
                }
            }
            break;
    }

    /* I put this down here rather than copying 
    and pasting it in multiple places - MS */
    if(enterRunState)
    {
        self->state = DEVICE_MONITOR_STATE_RUN;
        if(self->watchdogTimerEnabled)
        {
            self->timer.period = self->watchdogPeriod;
            self->timer.flags.expired = 0;
            self->timer.flags.start = 1;
        }
    }

    /* Putting the timer last allows the main code a chance to check the timer 
    expired flag and act on it before the next tick. Ideally, the user would 
    use the timer callback functions instead. - MS */
    if(self->timer.flags.start && self->timer.period != 0)
    {
        self->timer.flags.start = 0;
        self->timer.count = self->timer.period;
        self->timer.flags.active = 1;
    }

    if(self->timer.flags.active)
    {
        self->timer.count--;
        if(self->timer.count == 0)
        {
            self->timer.flags.active = 0;
            self->timer.flags.expired = 1;
        }
    }

}

// *****************************************************************************

void DeviceMonitor_Reset(DeviceMonitor *self)
{

}

// *****************************************************************************

bool DeviceMonitor_IsInitTimerFinished(DeviceMonitor *self)
{
    if(self->state == DEVICE_MONITOR_STATE_INIT_WAIT && self->timer.flags.expired)
        return true;
    else
        return false;
}

// *****************************************************************************

void DeviceMonitor_ClearInitTimer(DeviceMonitor *self, bool initSuccess)
{

}

// *****************************************************************************

bool DeviceMonitor_InitRetryLimitReached(DeviceMonitor *self)
{

}

// *****************************************************************************

bool DeviceMonitor_IsWatchdogTimerFinished(DeviceMonitor *self)
{

}

// *****************************************************************************

void DeviceMonitor_ResetWatchdogTimer(DeviceMonitor *self)
{

}

// *****************************************************************************

DeviceMonitorState DeviceMonitor_GetState(DeviceMonitor *self)
{
    return self->state;
}

/*
 End of File
 */
