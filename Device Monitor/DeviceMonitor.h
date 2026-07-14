/***************************************************************************//**
 * @brief Device Monitor Library Header File
 * 
 * @file DeviceMonitor.h
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

#ifndef DEVICEMONITOR_H
#define DEVICEMONITOR_H

#include <stdint.h>
#include <stdbool.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

typedef enum DeviceMonitorStateTag
{
    DEVICE_MONITOR_STATE_POR = 0,
    DEVICE_MONITOR_STATE_INIT_WAIT,
    DEVICE_MONITOR_STATE_RUN
} DeviceMonitorState;

typedef struct DeviceMonitorTag
{
    DeviceMonitorState state;
    struct
    {
        uint16_t period;
        uint16_t count;
        union {
            struct {
                unsigned start      :1;
                unsigned active     :1;
                unsigned expired    :1;
                unsigned            :5;
            };
            uint8_t all;
        } flags;
    } timer;
    uint16_t initPeriod;
    uint16_t watchdogPeriod;
    bool initWaitEnabled;
    bool initRetryEnabled;
    bool watchdogTimerEnabled;
    uint16_t initRetryCount;
    uint16_t initRetryLimit; // how many times to try before giving up

    // @todo add callback function pointers
} DeviceMonitor;

typedef struct DeviceMonitorInitTypeTag
{
    uint16_t timerTickRateMs;
    bool initWaitEnabled;
    uint16_t initPeriodMs;
    bool initRetryEnabled;
    uint16_t initRetryLimit;
    bool watchdogTimerEnabled;
    uint16_t watchdogPeriodMs;
} DeviceMonitorInitType;

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Function Prototypes *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

// @todo add doxygen headers, documentation

void DeviceMonitor_Init(DeviceMonitor *self, DeviceMonitorInitType *params);

void DeviceMonitor_Tick(DeviceMonitor *self);

void DeviceMonitor_Reset(DeviceMonitor *self);

bool DeviceMonitor_IsInitTimerFinished(DeviceMonitor *self);

void DeviceMonitor_ClearInitTimer(DeviceMonitor *self, bool initSuccess);

bool DeviceMonitor_InitRetryLimitReached(DeviceMonitor *self);

bool DeviceMonitor_IsWatchdogTimerFinished(DeviceMonitor *self);

void DeviceMonitor_ResetWatchdogTimer(DeviceMonitor *self);

DeviceMonitorState DeviceMonitor_GetState(DeviceMonitor *self);

// @todo more features:
// add enable, disable initTimer
// add enable, disable watchdog timer
// add setters for watchdog, retry limit etc. ?
// add error codes?

// init timer finished callback
// init failed callback
// init retry limit callback
// watchdog timeout callback

#endif /* DEVICEMONITOR_H */
