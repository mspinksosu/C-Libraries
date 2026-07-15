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
    self->initTimerExpiredEvent = false;
    self->initTimerCleared = false;
    self->initSuccess = false;
    self->initWaitEnabled = params->initWaitEnabled;
    self->initRetryEnabled = params->initRetryEnabled;
    self->watchdogTimerEnabled = params->watchdogTimerEnabled;
    self->initRetryLimit = params->initRetryLimit;
    self->state = DEVICE_MONITOR_STATE_POR;
}

// *****************************************************************************

void DeviceMonitor_Tick(DeviceMonitor *self)
{
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
                self->state = DEVICE_MONITOR_STATE_RUN;
                if(self->watchdogTimerEnabled)
                {
                    self->timer.period = self->watchdogPeriod;
                    self->timer.flags.expired = 0;
                    self->timer.flags.start = 1;
                }
            }
            break;
        case DEVICE_MONITOR_STATE_INIT_WAIT:
            if(self->timer.flags.expired)
            {
                self->timer.flags.expired = 0;
                self->initTimerExpiredEvent = true;
                self->initTimerCleared = false;

                /* Notify the user that the init timer is finished and the 
                retry limit has been reached. Or just notify the user that the 
                init timer is done. The two function pointers can point to the 
                same function, or different functions, depending on what the 
                user wants. - MS  */
                if(self->initRetryEnabled && self->initRetryLimit > 0)
                {
                    self->initRetryCount++;
                    if(self->initRetryCount <= self->initRetryLimit)
                    {
                        if(self->InitTimerFinishedCallback != NULL)
                            self->InitTimerFinishedCallback(self);
                    }
                    else
                    {
                        if(self->InitRetryLimitReachedCallback != NULL)
                            self->InitRetryLimitReachedCallback(self);
                    }
                }
                else
                {
                    if(self->InitTimerFinishedCallback != NULL)
                        self->InitTimerFinishedCallback(self);
                }
            }
            /* Wait for the user to clear the init timer finished flag before 
            deciding to go to the next state or not. */
            if(self->initTimerCleared)
            {
                self->initTimerCleared = false;
                if(self->initSuccess)
                {
                    /* Init finished. Go to next state. The timer will be used 
                    as the watchdog timer from now on. */
                    self->state = DEVICE_MONITOR_STATE_RUN;
                    if(self->watchdogTimerEnabled)
                    {
                        self->timer.period = self->watchdogPeriod;
                        self->timer.flags.start = 1;
                    }
                }
                else
                {
                    /* Else stay here and retry. */
                    self->timer.flags.start = 1;
                }
            }
            break;
        case DEVICE_MONITOR_STATE_RUN:
            if(self->timer.flags.expired)
            {
                /* The watchdog timer will restart automatically. Notify the 
                user anytime there is a timeout. */
                self->timer.flags.expired = 0;
                if(self->watchdogTimerEnabled)
                {
                    if(self->WatchdogTimeoutCallback != NULL)
                        self->WatchdogTimeoutCallback(self);
                    self->timer.flags.start = 1;
                }
            }
            break;
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
    self->timer.flags.all = 0;
    self->initRetryCount = 0;
    self->initTimerExpiredEvent = false;
    self->initTimerCleared = false;
    self->initSuccess = false;
    self->state = DEVICE_MONITOR_STATE_POR;
}

// *****************************************************************************

bool DeviceMonitor_IsInitTimerFinished(DeviceMonitor *self)
{
    if(self->state == DEVICE_MONITOR_STATE_INIT_WAIT && self->initTimerExpiredEvent)
        return true;
    else
        return false;
}

// *****************************************************************************

void DeviceMonitor_InitTimerFinishedEvent(DeviceMonitor *self, bool initSuccess)
{
    self->initTimerExpiredEvent = false;
    self->initTimerCleared = true;
    self->initSuccess = initSuccess;
}

// *****************************************************************************

bool DeviceMonitor_IsInitRetryLimitReached(DeviceMonitor *self)
{
    return (self->initRetryEnabled && self->initRetryLimit > 0 && 
            self->initRetryCount >= self->initRetryLimit);
}

// *****************************************************************************

bool DeviceMonitor_IsWatchdogTimerFinished(DeviceMonitor *self)
{
    if(self->timer.flags.expired && self->state == DEVICE_MONITOR_STATE_RUN)
        return true;
    else
        return false;
}

// *****************************************************************************

void DeviceMonitor_ResetWatchdogTimer(DeviceMonitor *self)
{
    /* Check if we've made it out of the init state yet. */
    if(self->state == DEVICE_MONITOR_STATE_RUN)
    {
        self->timer.flags.active = 0;
        self->timer.flags.expired = 0;
        self->timer.flags.start = 1;
    }
}

// *****************************************************************************

DeviceMonitorState DeviceMonitor_GetState(DeviceMonitor *self)
{
    return self->state;
}

// *****************************************************************************

void DeviceMonitor_SetInitTimerFinishedCallback(DeviceMonitor *self, void(*Function)(void *context))
{
    self->InitTimerFinishedCallback = Function;
}

// *****************************************************************************

void DeviceMonitor_SetInitRetryLimitReachedCallback(DeviceMonitor *self, void(*Function)(void *context))
{
    self->InitRetryLimitReachedCallback = Function;
}

// *****************************************************************************

void DeviceMonitor_SetWatchdogTimeoutCallback(DeviceMonitor *self, void(*Function)(void *context))
{
    self->WatchdogTimeoutCallback = Function;
}

/*
 End of File
 */
