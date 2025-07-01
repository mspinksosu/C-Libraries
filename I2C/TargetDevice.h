/***************************************************************************//**
 * @brief Target Device Header File
 * 
 * @file TargetDevice.h
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 5/1/25    Original creation
 * 
 * @details
 *      @todo add more details. Generic I2C target object. re-factoring I2C 
 * target code into more generic target device which can work with SPI 
 * also. - MS
 * 
 * @note The size of the data transfer buffer can be a minimum of 1. I would 
 * suggest a minimum size of 2. That is enough to hold one write data request, 
 * followed by one read data request. So that you could perform a repeated start 
 * if needed. - MS 
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

#ifndef TARGETDEVICE_H
#define TARGETDEVICE_H

#include <stdint.h>
#include <stdbool.h>

#include "DataTransfer.h" // @remove later

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

typedef enum TargetDeviceStateTag
{
    TARGETDEVICE_STATE_IDLE = 0,
    TARGETDEVICE_STATE_TRANSFER_IN_PROGRESS,
} TargetDeviceState;

typedef enum TargetDeviceTransferFinishedStatusTag
{
    TARGETDEVICE_TRANSFER_NOT_FINISHED = 0,
    TARGETDEVICE_TRANSFER_FINISHED_SUCCESS,
    TARGETDEVICE_TRANSFER_FINISHED_FAIL
} TargetDeviceTransferFinishedStatus;

typedef struct TargetDeviceTransferStatus
{
    bool isReadType;
    uint8_t *ptrArray;
    uint16_t sizeOfArray;
    uint16_t numBytesTransferred;
} TargetDeviceTransferStatus;

typedef struct TargetDeviceTag
{
    // TargetDevice *super; // include the base class first @remove later
    TargetDeviceTransferStatus finishedTransfer;
    struct
    {
        TargetDeviceState state;
        DTBuffer dtBuffer;
        bool transferStartedFlag;
        TargetDeviceTransferFinishedStatus transferFinishedStatus;

        // @todo add DTBuffer variables
    } private;
} TargetDevice;

// typedef struct TargetDeviceInitTypeTag
// {
//     // TargetDeviceInitType *super; // include the base class first @remove later
//     // uint8_t targetAddress7Bit; // 7-bit address, right justified
//     DataTransfer *ptrToDTArray;
//     uint8_t dtArraySize;
// } TargetDeviceInitType;

/** 
 * Description of struct
 * 
 * member1      description of variable member1
 * 
 */

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Function Prototypes *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


// void TargetDevice_SetInitTypeParams(TargetDeviceInitType *params, uint8_t targetAddress7Bit, 
//     DataTransfer *dtArray, uint8_t dtArraySize);

// @todo change to include variables from DataTransfer.h
// void TargetDevice_Init(TargetDevice *self, TargetDeviceInitType *params);

void TargetDevice_DataTransferFinishedEvent(TargetDevice *self, TargetDeviceTransferFinishedStatus *finishedMessage, 
    TargetDeviceTransferStatus *transferReport);

void TargetDevice_IsDataTransferFinished(TargetDevice *self, TargetDeviceTransferFinishedStatus *retFinishedMessage);

void TargetDevice_GetFinishedDataTransfer(TargetDevice *self, TargetDeviceTransferStatus *retTransferReport);

void TargetDevice_WriteToDataTransferBuffer(TargetDevice *self, bool readTypeTransfer, 
    uint8_t *dataArray, uint16_t length);

void TargetDevice_DataTransferStartedEvent(TargetDevice *self);

bool TargetDevice_IsDataTransferStarted(TargetDevice *self);

void TargetDevice_ClearDataTransferStartedFlag(TargetDevice *self);

void TargetDevice_ReadFromDataTransferBuffer(TargetDevice *self, bool *retIsReadType, 
    uint8_t **retPtrArray, uint16_t *retLength);

uint8_t TargetDevice_GetDataTransferBufferCount(TargetDevice *self);

bool TargetDevice_IsDataTransferBufferFull(TargetDevice *self);

bool TargetDevice_IsDataTransferBufferNotEmpty(TargetDevice *self);

uint8_t TargetDevice_GetDataTransferBufferSize(TargetDevice *self);

void TargetDevice_ClearDataTransferBuffer(TargetDevice *self);

TargetDeviceState TargetDevice_GetState(TargetDevice *self);

#endif /* TARGETDEVICE_H */
