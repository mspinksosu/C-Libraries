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
    .I2CTarget_Init = (void (*)(void *))I2CTarget_DT_Init,
    .I2CTarget_DataTransferFinishedEvent = (void (*)(void *, bool, uint8_t *, uint16_t))I2CTarget_DT_DataTransferFinishedEvent,
    .I2CTarget_IsDataTransferFinished = (bool (*)(void *))I2CTarget_DT_IsDataTransferFinished,
    .I2CTarget_GetFinishedDataTransfer = (void (*)(void *, bool *, uint8_t **, uint16_t *))I2CTarget_DT_GetFinishedDataTransfer,
    .I2CTarget_WriteToDataTransferBuffer = (void (*)(void *, bool, uint8_t *, uint16_t))I2CTarget_DT_WriteToDataTransferBuffer,
    .I2CTarget_DataTransferStartedEvent = (void (*)(void *))I2CTarget_DT_DataTransferStartedEvent,
    .I2CTarget_IsDataTransferStarted = (bool (*)(void *))I2CTarget_DT_IsDataTransferStarted,
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
    DataTransfer *dtArray, uint8_t arraySize)
{
    params->targetAddress7Bit = targetAddress7Bit;
    params->ptrToDTArray = dtArray;
    params->dtArraySize = arraySize;
}

// *****************************************************************************

void I2CTarget_DT_Init(I2CTarget_DT *self, I2CTargetInitType_DT *params)
{
    // Call the base initialization
    I2CTarget_BaseInit(self->super, params->targetAddress7Bit);
    // Now finish setting up the buffer
    self->private.state = I2C_TARGET_STATE_IDLE;
    self->private.flags.all = 0;
    DTBuffer_Init(&(self->private.dtBuffer), params->ptrToDTArray, params->dtArraySize);
}

// *****************************************************************************

void I2CTarget_DT_DataTransferFinishedEvent(I2CTarget_DT *self, bool readTypeTransfer, 
    uint8_t *dataArray, uint16_t length)
{
    DataTransferType type = DATA_TRANSFER_TYPE_WRITE;
    if(readTypeTransfer)
        type = DATA_TRANSFER_TYPE_READ;

    self->finishedTransfer.transferType = type;
    self->finishedTransfer.ptrDataArray = dataArray;
    self->finishedTransfer.length = length;

    self->private.flags.transferFinished = 1;
    self->private.state = I2C_TARGET_STATE_IDLE;
}

// *****************************************************************************

bool I2CTarget_DT_IsDataTransferFinished(I2CTarget_DT *self)
{
    if(self->private.flags.transferFinished)
        return true;
    else
        return false;
}

// *****************************************************************************

/* @todo Call the get finished function to clear the flags? It is kind of 
similar to how other modules like a UART work, where you have to read the 
receive register in order to clear the rx flag. - MS */

void I2CTarget_DT_GetFinishedDataTransfer(I2CTarget_DT *self, bool *retIsReadType, 
    uint8_t **retPtrArray, uint16_t *retLength)
{
    if(self->finishedTransfer.transferType = DATA_TRANSFER_TYPE_READ)
        *retIsReadType = true;
    else
        *retIsReadType = false;
    *retPtrArray = self->finishedTransfer.ptrDataArray;
    *retLength = self->finishedTransfer.length;

    // @todo clear finished flag here?
    self->private.flags.transferStarted = 0;
    self->private.flags.transferFinished = 0;
}

// *****************************************************************************

void I2CTarget_DT_WriteToDataTransferBuffer(I2CTarget_DT *self, bool readTypeTransfer, 
    uint8_t *dataArray, uint16_t length)
{
    DataTransferType type = DATA_TRANSFER_TYPE_WRITE;
    if(readTypeTransfer)
        type = DATA_TRANSFER_TYPE_READ;

    DTBuffer_WriteDataTransfer(&(self->private.dtBuffer), type, dataArray, length);
}

// *****************************************************************************

void I2CTarget_DT_DataTransferStartedEvent(I2CTarget_DT *self)
{
    self->private.flags.transferStarted = 1;
    self->private.state = I2C_TARGET_STATE_TRANSFER_IN_PROGRESS;
}

// *****************************************************************************

bool I2CTarget_DT_IsDataTransferStarted(I2CTarget_DT *self)
{
    if(self->private.flags.transferStarted)
        return true;
    else
        return false;
}

// *****************************************************************************

void I2CTarget_DT_ReadFromDataTransferBuffer(I2CTarget_DT *self, bool *retIsReadType, 
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
        *retPtrArray = retDataTransfer.ptrDataArray;
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

}

/*
 End of File
 */
