/***************************************************************************//**
 * @brief Target Device Interface Header
 * 
 * @file ITargetDevice.h
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 5/1/25    Original creation
 * 
 * @details
 *      @todo add more details. I decided to take my I2C slave code that was 
 * originally for the I2C manager and break it out into a more generic object. 
 * While I'm at it, I'll update the terminology to use "controller" and 
 * "target" since that is what is preferred now.
 * 
 * @todo should I just go ahead and change this file to generic TargetType 
 * while I'm at it?
 * 
 * 
 * @section license License
 * SPDX-FileCopyrightText: © 2025 Matthew Spinks
 * SPDX-License-Identifier: Zlib
 * 
 * This software is released under the Zlib license. You are free alter and
 * redistribute it, but you must not misrepresent the origin of the software.
 * This notice may not be removed. <http://www.zlib.net/zlib_license.html>
 * 
 ******************************************************************************/

#ifndef ITARGET_DEVICE_H
#define ITARGET_DEVICE_H

#include <stdint.h>
#include <stdbool.h>

/* @note @todo For now I'm going to go ahead and make the I2C target dependent 
on the data transfer type, while I think of different ways to implement the 
library. It is sort of universal type object anyway, so I might end up keeping 
it like this. The only function that really needs it right now is the receive 
function. In a way it behaves sort of like the normal receive function, 
except that instead of returning a byte, it is returning an object. - MS */
// #include "DataTransfer.h"

// ***** Defines ***************************************************************

/* @note The size of the data transfer buffer should be one more than the 
amount that you would like the target to be able to hold at once. I would 
suggest a minimum size of 3. That is enough to hold one write data request, 
followed by one read data request. - MS */
#define I2CTARGET_DT_BUFFER_SIZE 3 // @remove later

// ***** Global Variables ******************************************************

typedef enum TargetDeviceStateTag
{
    I2C_TARGET_STATE_IDLE = 0,
    I2C_TARGET_STATE_TRANSFER_IN_PROGRESS,
    I2C_TARGET_STATE_ERROR,
} TargetDeviceState;

typedef struct TargetDeviceInterfaceTag
{
    /* @note @todo Haven't decided if I want to implement this where a single 
    DT is processed at a time (similar to UART). Or use the buffer functions. 
    If I use the buffer functions and load multiple DT's at a time it can be 
    useful for generating repeated starts if needed. But it does force the 
    person using the library to implement those functions, even if it is just 
    a buffer of size one. - MS */
    void (*TargetDevice_Init)(void *instance, void *params);

    void (*TargetDevice_DataTransferFinishedEvent)(void *instance);
    bool (*TargetDevice_IsDataTransferFinished)(void *instance);
    void (*TargetDevice_GetFinishedDataTransfer)(void *instance);

    void (*TargetDevice_RequestDataTransfer)(void *instance, bool, uint8_t *, uint16_t);
    void (*TargetDevice_DataTransferStartedEvent)(void *instance);
    bool (*TargetDevice_IsDataTransferPending)(void *instance);
    void (*TargetDevice_GetPendingDataTransfer)(void *instance, bool *, uint8_t *, uint16_t *);

    uint8_t (*TargetDevice_GetDataTransferBufferCount)(void *instance);
    bool (*TargetDevice_IsDataTransferBufferFull)(void *instance);
    uint8_t (*TargetDevice_GetDataTransferBufferSize)(void *instance);
    void (*TargetDevice_ClearDataTransferBuffer)(void *instance);

    TargetDeviceState (*TargetDevice_GetState)(void *instance);
} TargetDeviceInterface;

// @todo decided if I want to keep the old callback function pointers
/* callback function pointer. The context is so that you can know which of
your I2C devices initiated the callback. */
// typedef void (*TargetDeviceCallbackFunc)(TargetDevice *i2cTargetContext);

/* @todo decide if I want to make this the base class or not */
typedef struct TargetDeviceTag
{
    TargetDeviceInterface *interface;
    void *instance;

    // struct
    // {
    //     DTBuffer *buffer;
    //     DataTransfer dtArray[I2CTARGET_DT_BUFFER_SIZE];
    // } private;
} TargetDevice;

typedef struct TargetDeviceInitTypeTage
{
    void *instance;
} TargetDeviceInitType;

/** 
 * Description of struct
 * 
 * member1      description of variable member1
 * 
 */

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void TargetDevice_Create(TargetDevice *self, void *instanceOfSubclass, TargetDeviceInterface *interface);

void TargetDevice_CreateInitType(TargetDeviceInitType *params, void *instanceOfSubClass);

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void TargetDevice_Init(TargetDevice *self, TargetDeviceInitType *params);

void TargetDevice_DataTransferFinishedEvent(TargetDevice *self);

bool TargetDevice_IsDataTransferFinished(TargetDevice *self);

void TargetDevice_GetFinishedDataTransfer(TargetDevice *self);

// place in buffer
void TargetDevice_RequestDataTransfer(TargetDevice *self, bool readTypeTransfer, 
    uint8_t *array, uint16_t length);

// not pending anymore
void TargetDevice_DataTransferStartedEvent(TargetDevice *self);

bool TargetDevice_IsDataTransferPending(TargetDevice *self);

// sets transferPending to false after calling
void TargetDevice_GetPendingDataTransfer(TargetDevice *self, bool *retIsReadType, 
    uint8_t *retPtrArray, uint16_t *retLength);

uint8_t TargetDevice_GetDataTransferBufferCount(TargetDevice *self);

bool TargetDevice_IsDataTransferBufferFull(TargetDevice *self);

uint8_t TargetDevice_GetDataTransferBufferSize(TargetDevice *self);

void TargetDevice_ClearDataTransferBuffer(TargetDevice *self);

TargetDeviceState TargetDevice_GetState(TargetDevice *self);

// @todo possible callback functions
// TargetDevice_SetTransferFinishedCallbackFunc

#endif /* ITARGET_DEVICE_H */
