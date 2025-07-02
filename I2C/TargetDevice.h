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

typedef enum DataTransferTypeTag
{
    DATATRANSFER_TYPE_WRITE = 0,
    DATATRANSFER_TYPE_READ
} DataTransferType;

typedef struct DataTransferTag
{
    DataTransferType transferType;
    uint8_t *ptrDataArray;
    uint16_t length;
} DataTransfer;

typedef struct TargetDeviceTag
{
    DataTransfer finishedTransfer;
    uint16_t numBytesTransferred;
    struct
    {
        TargetDeviceState state;
        bool transferStartedFlag;
        TargetDeviceTransferFinishedStatus transferFinishedStatus;

        // DataTransfer buffer variables
        DataTransfer *buffer;
        uint8_t count;
        uint8_t head;
        uint8_t tail;
        uint8_t size;
    } private;
} TargetDevice;

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

/* @follow-up Should I change the write and read buffer functions to use the 
DataTransfer type? Or keep it like it is currently? - MS */

void TargetDevice_Init(TargetDevice *self, DataTransfer *arrayIn, uint8_t arrayInSize);

void TargetDevice_DataTransferFinishedEvent(TargetDevice *self, TargetDeviceTransferFinishedStatus *finishedMessage, 
    DataTransfer *transferReport, uint16_t numBytesTransferred);

void TargetDevice_IsDataTransferFinished(TargetDevice *self, TargetDeviceTransferFinishedStatus *retFinishedMessage);

/* Returns number of bytes transferred. @note As of right now, I've decided that getting the received data using 
this function should clear the transfer finished flag. - MS */
uint16_t TargetDevice_GetFinishedDataTransfer(TargetDevice *self, DataTransfer *retTransferReport);

void TargetDevice_WriteToDataTransferBuffer(TargetDevice *self, DataTransferType transferType, 
    uint8_t *dataArray, uint16_t length);

void TargetDevice_DataTransferStartedEvent(TargetDevice *self);

bool TargetDevice_IsDataTransferStarted(TargetDevice *self);

/* @note Adding for completeness. I wasn't sure if I wanted read from 
buffer to clear the transfer started flag or not, so I added this for now. - MS */
void TargetDevice_ClearDataTransferStartedFlag(TargetDevice *self);

// @note set return length to 0 if no data available
void TargetDevice_ReadFromDataTransferBuffer(TargetDevice *self, DataTransfer *retDataTransferObj);

uint8_t TargetDevice_GetDataTransferBufferCount(TargetDevice *self);

bool TargetDevice_IsDataTransferBufferFull(TargetDevice *self);

bool TargetDevice_IsDataTransferBufferNotEmpty(TargetDevice *self);

uint8_t TargetDevice_GetDataTransferBufferSize(TargetDevice *self);

void TargetDevice_ClearDataTransferBuffer(TargetDevice *self);


TargetDeviceState TargetDevice_GetState(TargetDevice *self);

#endif /* TARGETDEVICE_H */
