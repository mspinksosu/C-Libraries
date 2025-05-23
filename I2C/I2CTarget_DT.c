/***************************************************************************//**
 * @brief I2C Target Implementation
 * 
 * @file I2CTarget_DT.c
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
I2CTargetInterface I2CTargetFunctionTable = {
    .I2CTarget_Init = (void (*)(void *, void *))I2CTarget_DT_Init,
    .I2CTarget_DataTransferFinishedEvent = (void (*)(void *, I2CTargetTransferFinishedStatus *, I2CTargetTransferStatus *))I2CTarget_DT_DataTransferFinishedEvent,
    .I2CTarget_IsDataTransferFinished = (void (*)(void *, I2CTargetTransferFinishedStatus *))I2CTarget_DT_IsDataTransferFinished,
    .I2CTarget_GetFinishedDataTransfer = (void (*)(void *, I2CTargetTransferStatus *))I2CTarget_DT_GetFinishedDataTransfer,
    .I2CTarget_WriteToDataTransferBuffer = (void (*)(void *, bool, uint8_t *, uint16_t))I2CTarget_DT_WriteToDataTransferBuffer,
    .I2CTarget_DataTransferStartedEvent = (void (*)(void *))I2CTarget_DT_DataTransferStartedEvent,
    .I2CTarget_IsDataTransferStarted = (bool (*)(void *))I2CTarget_DT_IsDataTransferStarted,
    .I2CTarget_ClearDataTransferStartedFlag = (void (*)(void *))I2CTarget_DT_ClearDataTransferStartedFlag,
    .I2CTarget_ReadFromDataTransferBuffer = (void (*)(void *, bool *, uint8_t **, uint16_t *))I2CTarget_DT_ReadFromDataTransferBuffer,
    .I2CTarget_GetDataTransferBufferCount = (uint8_t (*)(void *))I2CTarget_DT_GetDataTransferBufferCount,
    .I2CTarget_IsDataTransferBufferFull = (bool (*)(void *))I2CTarget_DT_IsDataTransferBufferFull,
    .I2CTarget_IsDataTransferBufferNotEmpty = (bool (*)(void *))I2CTarget_DT_IsDataTransferBufferNotEmpty,
    .I2CTarget_GetDataTransferBufferSize = (uint8_t (*)(void *))I2CTarget_DT_GetDataTransferBufferSize,
    .I2CTarget_ClearDataTransferBuffer = (void (*)(void *))I2CTarget_DT_ClearDataTransferBuffer,
    .I2CTarget_GetState = (I2CTargetState (*)(void *))I2CTarget_DT_GetState,
};

// ***** Static Function Prototypes ********************************************

/* Put static function prototypes here */


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void I2CTarget_DT_Create(I2CTarget_DT *self, I2CTarget *base)
{
    self->super = base;

    /* Do the rest of the initialization for struct members etc. */

    /*  Call the base class constructor. What you are doing is connecting the 
    base class's instance member to the instance of the sub class that you just 
    created, along with the list of functions that will be used. From now on,
    you'll be able to use the base class for function calls */
    I2CTarget_Create(base, self, &I2CTargetFunctionTable);
}

// *****************************************************************************

void I2CTarget_DT_CreateInitType(I2CTargetInitType_DT *params, I2CTargetInitType *base)
{
    params->super = base;
    I2CTarget_CreateInitType(base, params);
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void I2CTarget_DT_SetInitTypeParams(I2CTargetInitType_DT *params, uint8_t targetAddress7Bit, 
    DataTransfer *dtArray, uint8_t dtArraySize)
{
    params->targetAddress7Bit = targetAddress7Bit;
    params->ptrToDTArray = dtArray;
    params->dtArraySize = dtArraySize;
}

// *****************************************************************************

void I2CTarget_DT_Init(I2CTarget_DT *self, I2CTargetInitType_DT *params)
{
    // Call the base initialization
    I2CTarget_BaseInit(self->super, params->targetAddress7Bit);
    // Now finish setting up the buffer
    self->private.state = I2CTARGET_STATE_IDLE;
    self->private.transferFinishedStatus = I2CTARGET_TRANSFER_NOT_FINISHED;
    self->private.transferStartedFlag = false;
    DTBuffer_Init(&(self->private.dtBuffer), params->ptrToDTArray, params->dtArraySize);
}

// *****************************************************************************

void I2CTarget_DT_DataTransferFinishedEvent(I2CTarget_DT *self, I2CTargetTransferFinishedStatus *finishedMessage, 
    I2CTargetTransferStatus *transferReport)
{
    self->private.transferFinishedStatus = *finishedMessage;
    self->finishedTransfer = *transferReport;

    if(*finishedMessage == I2CTARGET_TRANSFER_FINISHED_SUCCESS)
        self->private.state = I2CTARGET_STATE_IDLE;
    else
        self->private.state = I2CTARGET_STATE_ERROR;
}

// *****************************************************************************

void I2CTarget_DT_IsDataTransferFinished(I2CTarget_DT *self, I2CTargetTransferFinishedStatus *retFinishedMessage)
{
    *retFinishedMessage = self->private.transferFinishedStatus;
}

// *****************************************************************************

void I2CTarget_DT_GetFinishedDataTransfer(I2CTarget_DT *self, I2CTargetTransferStatus *retTransferReport)
{
    *retTransferReport = self->finishedTransfer;

    /* Clear the finished status flag */
    self->private.transferFinishedStatus = I2CTARGET_TRANSFER_NOT_FINISHED;
    /* @todo Should I also clear the transfer started flag here as well? 
    Or let the user handle it with the separate function? */
    self->private.transferStartedFlag = false;
}

// *****************************************************************************

void I2CTarget_DT_WriteToDataTransferBuffer(I2CTarget_DT *self, bool readTypeTransfer, 
    uint8_t *dataArray, uint16_t length)
{
    DataTransferType type = DATA_TRANSFER_TYPE_WRITE;
    if(readTypeTransfer)
        type = DATA_TRANSFER_TYPE_READ;

    DTBuffer_WriteDataTransfer(&(self->private.dtBuffer), type, dataArray, length);

    /* @todo Update transfer finished status automatically and change state to 
    idle automatically if it was in error state before? */
    self->private.state = I2CTARGET_STATE_IDLE;
    self->private.transferFinishedStatus = I2CTARGET_TRANSFER_NOT_FINISHED;
}

// *****************************************************************************

void I2CTarget_DT_DataTransferStartedEvent(I2CTarget_DT *self)
{
    self->private.transferStartedFlag = true;
    self->private.state = I2CTARGET_STATE_TRANSFER_IN_PROGRESS;
}

// *****************************************************************************

bool I2CTarget_DT_IsDataTransferStarted(I2CTarget_DT *self)
{
    if(self->private.transferStartedFlag)
        return true;
    else
        return false;
}

// *****************************************************************************

void I2CTarget_DT_ClearDataTransferStartedFlag(I2CTarget_DT *self)
{
    self->private.transferStartedFlag = false;
}

// *****************************************************************************

void I2CTarget_DT_ReadFromDataTransferBuffer(I2CTarget_DT *self, bool *retIsReadType, 
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

uint8_t I2CTarget_DT_GetDataTransferBufferCount(I2CTarget_DT *self)
{
    return DTBuffer_GetCount(&(self->private.dtBuffer));
}

// *****************************************************************************

bool I2CTarget_DT_IsDataTransferBufferFull(I2CTarget_DT *self)
{
    return DTBuffer_IsFull(&(self->private.dtBuffer));
}

// *****************************************************************************

bool I2CTarget_DT_IsDataTransferBufferNotEmpty(I2CTarget_DT *self)
{
    return DTBuffer_IsNotEmpty(&(self->private.dtBuffer));
}

// *****************************************************************************

uint8_t I2CTarget_DT_GetDataTransferBufferSize(I2CTarget_DT *self)
{
    return DTBuffer_GetSize(&(self->private.dtBuffer));
}

// *****************************************************************************

void I2CTarget_DT_ClearDataTransferBuffer(I2CTarget_DT *self)
{
    DTBuffer_Flush(&(self->private.dtBuffer));
}

// *****************************************************************************

I2CTargetState I2CTarget_DT_GetState(I2CTarget_DT *self)
{
    return self->private.state;
}

/*
 End of File
 */
