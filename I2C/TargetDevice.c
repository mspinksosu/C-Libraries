/***************************************************************************//**
 * @brief Target Device Library
 * 
 * @file TargetDevice.c
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 5/1/25    Original creation
 * 
 * @details
 *      @todo add more details. Generic I2C target object. Will use my data 
 * transfer buffer. - MS
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

#include "TargetDevice.h"

// ***** Defines ***************************************************************

#define CircularIncrement(i, size) i == (size - 1) ? 0 : i + 1

// ***** Global Variables ******************************************************


// ***** Static Function Prototypes ********************************************


// *****************************************************************************

void TargetDevice_Init(TargetDevice *self, DataTransfer *arrayIn, uint8_t arrayInSize)
{
    self->private.buffer = arrayIn;
    self->private.size = arrayInSize;
    self->private.count = 0;
    self->private.state = TARGETDEVICE_STATE_IDLE;
    self->private.transferFinishedStatus = TARGETDEVICE_TRANSFER_NOT_FINISHED;
    self->private.transferStartedFlag = false;
}

// *****************************************************************************

void TargetDevice_DataTransferFinishedEvent(TargetDevice *self, TargetDeviceTransferFinishedStatus *finishedMessage, 
    TargetDeviceTransferStatus *transferReport)
{
    self->private.transferFinishedStatus = *finishedMessage;
    self->finishedTransfer = *transferReport;
    self->private.state = TARGETDEVICE_STATE_IDLE;
}

// *****************************************************************************

void TargetDevice_IsDataTransferFinished(TargetDevice *self, TargetDeviceTransferFinishedStatus *retFinishedMessage)
{
    *retFinishedMessage = self->private.transferFinishedStatus;
}

// *****************************************************************************

void TargetDevice_GetFinishedDataTransfer(TargetDevice *self, TargetDeviceTransferStatus *retTransferReport)
{
    *retTransferReport = self->finishedTransfer;

    /* Clear the finished status flag */
    self->private.transferFinishedStatus = TARGETDEVICE_TRANSFER_NOT_FINISHED;
    /* @todo Should I also clear the transfer started flag here as well? 
    Or let the user handle it with the separate function? */
    self->private.transferStartedFlag = false;
}

// *****************************************************************************

void TargetDevice_WriteToDataTransferBuffer(TargetDevice *self, bool readTypeTransfer, 
    uint8_t *dataArray, uint16_t length)
{
    DataTransferType type = DATA_TRANSFER_TYPE_WRITE;
    if(readTypeTransfer)
        type = DATA_TRANSFER_TYPE_READ;

    // @todo replace DTBuffer function
    // DTBuffer_WriteDataTransfer(&(self->private.dtBuffer), type, dataArray, length);

    self->private.state = TARGETDEVICE_STATE_IDLE;
    self->private.transferFinishedStatus = TARGETDEVICE_TRANSFER_NOT_FINISHED;
}

// *****************************************************************************

void TargetDevice_DataTransferStartedEvent(TargetDevice *self)
{
    self->private.transferStartedFlag = true;
    self->private.state = TARGETDEVICE_STATE_TRANSFER_IN_PROGRESS;
}

// *****************************************************************************

bool TargetDevice_IsDataTransferStarted(TargetDevice *self)
{
    if(self->private.transferStartedFlag)
        return true;
    else
        return false;
}

// *****************************************************************************

void TargetDevice_ClearDataTransferStartedFlag(TargetDevice *self)
{
    self->private.transferStartedFlag = false;
}

// *****************************************************************************

void TargetDevice_ReadFromDataTransferBuffer(TargetDevice *self, bool *retIsReadType, 
    uint8_t **retPtrArray, uint16_t *retLength)
{
    DataTransfer retDataTransfer;
    // @todo replace DTBuffer function
    // DTBuffer_ReadDataTransfer(&(self->private.dtBuffer), &retDataTransfer);
    if(retDataTransfer.length > 0)
    {
        if(retDataTransfer.transferType == DATA_TRANSFER_TYPE_READ)
            *retIsReadType = true;
        else
            *retIsReadType = false;
        /* Change the value of the pointer to point to the new address */
        (*retPtrArray) = retDataTransfer.ptrDataArray;
        *retLength = retDataTransfer.length;
    }
}

// *****************************************************************************

uint8_t TargetDevice_GetDataTransferBufferCount(TargetDevice *self)
{
    return self->private.count;
}

// *****************************************************************************

bool TargetDevice_IsDataTransferBufferFull(TargetDevice *self)
{
    if(self->private.count >= self->private.size)
        return true;
    else
        return false;
}

// *****************************************************************************

bool TargetDevice_IsDataTransferBufferNotEmpty(TargetDevice *self)
{
    if(self->private.count != 0)
        return true;
    else
        return false;
}

// *****************************************************************************

uint8_t TargetDevice_GetDataTransferBufferSize(TargetDevice *self)
{
    return self->private.size;
}

// *****************************************************************************

void TargetDevice_ClearDataTransferBuffer(TargetDevice *self)
{
    self->private.tail = self->private.head;
    self->private.count = 0;
}

// *****************************************************************************

TargetDeviceState TargetDevice_GetState(TargetDevice *self)
{
    return self->private.state;
}

/*
 End of File
 */
