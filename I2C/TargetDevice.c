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
 *      @todo add more details. Generic I2C target object. - MS
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
#include <stddef.h> // needed for NULL

// ***** Defines ***************************************************************

#define CircularIncrement(i, size) (i == (size - 1) ? 0 : i + 1)

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
    DataTransfer *transferReport, uint16_t numBytesTransferred)
{
    self->private.transferFinishedStatus = *finishedMessage;
    self->finishedTransfer = *transferReport;
    self->numBytesTransferred = numBytesTransferred;
    self->private.state = TARGETDEVICE_STATE_IDLE;
}

// *****************************************************************************

void TargetDevice_IsDataTransferFinished(TargetDevice *self, TargetDeviceTransferFinishedStatus *retFinishedMessage)
{
    *retFinishedMessage = self->private.transferFinishedStatus;
}

// *****************************************************************************

uint16_t TargetDevice_GetFinishedDataTransfer(TargetDevice *self, DataTransfer *retTransferReport)
{
    *retTransferReport = self->finishedTransfer;

    /* Clear the finished status flag */
    self->private.transferFinishedStatus = TARGETDEVICE_TRANSFER_NOT_FINISHED;
    /* @todo Should I also clear the transfer started flag here as well? 
    Or let the user handle it with the separate function? */
    self->private.transferStartedFlag = false;

    return self->numBytesTransferred;
}

// *****************************************************************************

void TargetDevice_WriteToDataTransferBuffer(TargetDevice *self, DataTransfer *dataTransferObj)
{
    if(self->private.count < self->private.size)
    {
        // There is space in the buffer
        uint8_t tempHead = CircularIncrement(self->private.head, self->private.size);
        self->private.buffer[self->private.head] = *dataTransferObj;
        self->private.head = tempHead;
        self->private.count++;
    }
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

void TargetDevice_ReadFromDataTransferBuffer(TargetDevice *self, DataTransfer *retDataTransferObj)
{
    DataTransfer retDataTransfer = {.length = 0, .ptrDataArray = NULL};

    if(self->private.count > 0)
    {
        /* The buffer is not empty. Get the data from the buffer to be 
        processed */
        *retDataTransferObj = self->private.buffer[self->private.tail];
        self->private.tail = CircularIncrement(self->private.tail, self->private.size);
        self->private.count--;
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
