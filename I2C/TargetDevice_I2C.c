/***************************************************************************//**
 * @brief I2C Target Implementation (Non-Processor Specific)
 * 
 * @file I2CTarget.c
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

#include "TargetDevice_I2C.h" // @todo change name to something that includes data transfer buffer? I2CTarget_DT? - MS
#include <stddef.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/*  The sub class must implement the functions provided in the interface. In 
    this case we are declaring an interface struct and initializing its members 
    (which are function pointers) the our local functions. Typecasting is 
    necessary. When a new sub class object is created, we will set its interface
    member equal to this table. */
TargetDeviceInterface TargetDeviceFunctionTable = {
    .TargetDevice_Init = (void (*)(void *))TargetDevice_I2C_Init,
    .TargetDevice_DataTransferFinishedEvent = (void (*)(void *))TargetDevice_I2C_DataTransferFinishedEvent,
    .TargetDevice_IsDataTransferFinished = (bool (*)(void *))TargetDevice_I2C_IsDataTransferFinished,
    .TargetDevice_GetFinishedDataTransfer = (void (*)(bool *, uint8_t *, uint16_t *))TargetDevice_I2C_GetFinishedDataTransfer,
    .TargetDevice_RequestDataTransfer = (void (*)(void *, uint8_t *, uint16_t))TargetDevice_I2C_RequestDataTransfer,
    .TargetDevice_DataTransferStartedEvent = (void (*)(void *))TargetDevice_I2C_DataTransferStartedEvent,
    .TargetDevice_IsDataTransferPending = (bool (*)(void *))TargetDevice_I2C_IsDataTransferPending,
    .TargetDevice_GetPendingDataTransfer = (void (*)(bool *, uint8_t *, uint16_t *))TargetDevice_I2C_GetPendingDataTransfer,
    .TargetDevice_GetDataTransferBufferCount = (uint8_t (*)(void *))TargetDevice_I2C_GetDataTransferBufferCount,
    .TargetDevice_IsDataTransferBufferFull = (bool (*)(void *))TargetDevice_I2C_IsDataTransferBufferFull,
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

// *****************************************************************************

void TargetDevice_I2C_SetInitTypeParams(TargetDeviceInitType_I2C *params, uint8_t targetAddress7Bit, 
    DataTransfer *dtArray, uint8_t arraySize)
{
    params->targetAddress7Bit = targetAddress7Bit;
    params->ptrToDTArray = dtArray;
    params->dtArraySize = arraySize;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void TargetDevice_I2C_Init(TargetDevice_I2C *self, TargetDeviceInitType_I2C *params)
{
    self->targetAddress7Bit = params->targetAddress7Bit;
    self->private.state = I2C_TARGET_STATE_IDLE;
    self->private.flags.all = 0;
    DTBuffer_Init(&(self->private.dtBuffer), params->ptrToDTArray, params->dtArraySize);
}

// *****************************************************************************

void TargetDevice_I2C_DataTransferFinishedEvent(TargetDevice_I2C *self)
{

}

// *****************************************************************************

bool TargetDevice_I2C_IsDataTransferFinished(TargetDevice_I2C *self)
{
    if(self->private.flags.transferFinished)
        return true;
    else
        return false;
}

// *****************************************************************************

void TargetDevice_I2C_GetFinishedDataTransfer(TargetDevice_I2C *self, bool *retIsReadType, 
    uint8_t **retPtrArray, uint16_t *retLength)
{
    if(self->finishedTransfer.transferType = DATA_TRANSFER_TYPE_READ)
        *retIsReadType = true;
    else
        *retIsReadType = false;
    **retPtrArray = self->finishedTransfer.ptrDataArray;
    *retLength = self->finishedTransfer.length;

    // @todo clear finished flag here?
}

// *****************************************************************************

void TargetDevice_I2C_RequestDataTransfer(TargetDevice_I2C *self, bool readTypeTransfer, 
    uint8_t *dataArray, uint16_t length)
{
    DataTransferType type = DATA_TRANSFER_TYPE_WRITE;
    if(readTypeTransfer)
        type = DATA_TRANSFER_TYPE_READ;

    DTBuffer_WriteDataTransfer(&(self->private.dtBuffer), type, dataArray, length);
}

// *****************************************************************************

void TargetDevice_I2C_DataTransferStartedEvent(TargetDevice_I2C *self)
{
    // @todo private.flags.transferStarted? clear finished?
}

// *****************************************************************************

bool TargetDevice_I2C_IsDataTransferPending(TargetDevice_I2C *self)
{
    if(self->private.flags.transferPending)
        return true;
    else
        return false;
}

// *****************************************************************************

void TargetDevice_I2C_GetPendingDataTransfer(TargetDevice_I2C *self, bool *retIsReadType, 
    uint8_t **retPtrArray, uint16_t *retLength)
{
    DataTransfer retDataTransfer;
    uint8_t error = DTBuffer_ReadDataTransfer(&(self->private.dtBuffer), &retDataTransfer);
    if(error == 0)
    {
        if(retDataTransfer.transferType = DATA_TRANSFER_TYPE_READ)
            *retIsReadType = true;
        else
            *retIsReadType = false;
        **retPtrArray = retDataTransfer.ptrDataArray;
        *retLength = retDataTransfer.length;
    }

    // @todo call transfer started event?
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

}

/*
 End of File
 */
