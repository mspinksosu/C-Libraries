/***************************************************************************//**
 * @brief I2C Target Device Implementation
 * 
 * @file TargetDevice_I2C.c
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

#include "I2CTarget_DT.h"
#include <stddef.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/*  The sub class must implement the functions provided in the interface. In 
    this case we are declaring an interface struct and initializing its members 
    (which are function pointers) the our local functions. Typecasting is 
    necessary. When a new sub class object is created, we will set its interface
    member equal to this table. */
TargetDeviceInterface TargetDeviceFunctionTable = {
    .TargetDevice_Init = (void (*)(void *, void *))TargetDevice_I2C_Init,
    .TargetDevice_DataTransferFinishedEvent = (void (*)(void *, TargetDeviceTransferFinishedStatus *, TargetDeviceTransferStatus *))TargetDevice_I2C_DataTransferFinishedEvent,
    .TargetDevice_IsDataTransferFinished = (void (*)(void *, TargetDeviceTransferFinishedStatus *))TargetDevice_I2C_IsDataTransferFinished,
    .TargetDevice_GetFinishedDataTransfer = (void (*)(void *, TargetDeviceTransferStatus *))TargetDevice_I2C_GetFinishedDataTransfer,
    .TargetDevice_WriteToDataTransferBuffer = (void (*)(void *, bool, uint8_t *, uint16_t))TargetDevice_I2C_WriteToDataTransferBuffer,
    .TargetDevice_DataTransferStartedEvent = (void (*)(void *))TargetDevice_I2C_DataTransferStartedEvent,
    .TargetDevice_IsDataTransferStarted = (bool (*)(void *))TargetDevice_I2C_IsDataTransferStarted,
    .TargetDevice_ClearDataTransferStartedFlag = (void (*)(void *))TargetDevice_I2C_ClearDataTransferStartedFlag,
    .TargetDevice_ReadFromDataTransferBuffer = (void (*)(void *, bool *, uint8_t **, uint16_t *))TargetDevice_I2C_ReadFromDataTransferBuffer,
    .TargetDevice_GetDataTransferBufferCount = (uint8_t (*)(void *))TargetDevice_I2C_GetDataTransferBufferCount,
    .TargetDevice_IsDataTransferBufferFull = (bool (*)(void *))TargetDevice_I2C_IsDataTransferBufferFull,
    .TargetDevice_IsDataTransferBufferNotEmpty = (bool (*)(void *))TargetDevice_I2C_IsDataTransferBufferNotEmpty,
    .TargetDevice_GetDataTransferBufferSize = (uint8_t (*)(void *))TargetDevice_I2C_GetDataTransferBufferSize,
    .TargetDevice_ClearDataTransferBuffer = (void (*)(void *))TargetDevice_I2C_ClearDataTransferBuffer,
    .TargetDevice_GetState = (TargetDeviceState (*)(void *))TargetDevice_I2C_GetState,
};

// ***** Static Function Prototypes ********************************************

/* Put static function prototypes here */


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void TargetDevice_I2C_Create(TargetDevice_I2C *self, TargetDevice *base)
{
    self->super = base;

    /* Do the rest of the initialization for struct members etc. */

    /*  Call the base class constructor. What you are doing is connecting the 
    base class's instance member to the instance of the sub class that you just 
    created, along with the list of functions that will be used. From now on,
    you'll be able to use the base class for function calls */
    TargetDevice_Create(base, self, &TargetDeviceFunctionTable);
}

// *****************************************************************************

void TargetDevice_I2C_CreateInitType(TargetDeviceInitType_I2C *params, TargetDeviceInitType *base)
{
    params->super = base;
    TargetDevice_CreateInitType(base, params);
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void TargetDevice_I2C_SetInitTypeParams(TargetDeviceInitType_I2C *params, uint8_t targetAddress7Bit, 
    DataTransfer *dtArray, uint8_t dtArraySize)
{
    params->targetAddress7Bit = targetAddress7Bit;
    params->ptrToDTArray = dtArray;
    params->dtArraySize = dtArraySize;
}

// *****************************************************************************

void TargetDevice_I2C_Init(TargetDevice_I2C *self, TargetDeviceInitType_I2C *params)
{
    // Call the base initialization
    TargetDevice_BaseInit(self->super, params->targetAddress7Bit);
    // Now finish setting up the buffer
    self->private.state = I2CTARGET_STATE_IDLE;
    self->private.transferFinishedStatus = I2CTARGET_TRANSFER_NOT_FINISHED;
    self->private.transferStartedFlag = false;
    DTBuffer_Init(&(self->private.dtBuffer), params->ptrToDTArray, params->dtArraySize);
}

// *****************************************************************************

void TargetDevice_I2C_DataTransferFinishedEvent(TargetDevice_I2C *self, TargetDeviceTransferFinishedStatus *finishedMessage, 
    TargetDeviceTransferStatus *transferReport)
{
    self->private.transferFinishedStatus = *finishedMessage;
    self->finishedTransfer = *transferReport;
    self->private.state = I2CTARGET_STATE_IDLE;
}

// *****************************************************************************

void TargetDevice_I2C_IsDataTransferFinished(TargetDevice_I2C *self, TargetDeviceTransferFinishedStatus *retFinishedMessage)
{
    *retFinishedMessage = self->private.transferFinishedStatus;
}

// *****************************************************************************

void TargetDevice_I2C_GetFinishedDataTransfer(TargetDevice_I2C *self, TargetDeviceTransferStatus *retTransferReport)
{
    *retTransferReport = self->finishedTransfer;

    /* Clear the finished status flag */
    self->private.transferFinishedStatus = I2CTARGET_TRANSFER_NOT_FINISHED;
    /* @todo Should I also clear the transfer started flag here as well? 
    Or let the user handle it with the separate function? */
    self->private.transferStartedFlag = false;
}

// *****************************************************************************

void TargetDevice_I2C_WriteToDataTransferBuffer(TargetDevice_I2C *self, bool readTypeTransfer, 
    uint8_t *dataArray, uint16_t length)
{
    DataTransferType type = DATA_TRANSFER_TYPE_WRITE;
    if(readTypeTransfer)
        type = DATA_TRANSFER_TYPE_READ;

    DTBuffer_WriteDataTransfer(&(self->private.dtBuffer), type, dataArray, length);
    self->private.state = I2CTARGET_STATE_IDLE;
    self->private.transferFinishedStatus = I2CTARGET_TRANSFER_NOT_FINISHED;
}

// *****************************************************************************

void TargetDevice_I2C_DataTransferStartedEvent(TargetDevice_I2C *self)
{
    self->private.transferStartedFlag = true;
    self->private.state = I2CTARGET_STATE_TRANSFER_IN_PROGRESS;
}

// *****************************************************************************

bool TargetDevice_I2C_IsDataTransferStarted(TargetDevice_I2C *self)
{
    if(self->private.transferStartedFlag)
        return true;
    else
        return false;
}

// *****************************************************************************

void TargetDevice_I2C_ClearDataTransferStartedFlag(TargetDevice_I2C *self)
{
    self->private.transferStartedFlag = false;
}

// *****************************************************************************

void TargetDevice_I2C_ReadFromDataTransferBuffer(TargetDevice_I2C *self, bool *retIsReadType, 
    uint8_t **retPtrArray, uint16_t *retLength)
{
    DataTransfer retDataTransfer;
    DTBuffer_ReadDataTransfer(&(self->private.dtBuffer), &retDataTransfer);
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

uint8_t TargetDevice_I2C_GetDataTransferBufferCount(TargetDevice_I2C *self)
{
    return DTBuffer_GetCount(&(self->private.dtBuffer));
}

// *****************************************************************************

bool TargetDevice_I2C_IsDataTransferBufferFull(TargetDevice_I2C *self)
{
    return DTBuffer_IsFull(&(self->private.dtBuffer));
}

// *****************************************************************************

bool TargetDevice_I2C_IsDataTransferBufferNotEmpty(TargetDevice_I2C *self)
{
    return DTBuffer_IsNotEmpty(&(self->private.dtBuffer));
}

// *****************************************************************************

uint8_t TargetDevice_I2C_GetDataTransferBufferSize(TargetDevice_I2C *self)
{
    return DTBuffer_GetSize(&(self->private.dtBuffer));
}

// *****************************************************************************

void TargetDevice_I2C_ClearDataTransferBuffer(TargetDevice_I2C *self)
{
    DTBuffer_Flush(&(self->private.dtBuffer));
}

// *****************************************************************************

TargetDeviceState TargetDevice_I2C_GetState(TargetDevice_I2C *self)
{
    return self->private.state;
}

/*
 End of File
 */
