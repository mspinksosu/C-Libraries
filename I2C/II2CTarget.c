/***************************************************************************//**
 * @brief I2C Target Device Interface
 * 
 * @file II2CTarget.c
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
 * @section license License
 * SPDX-FileCopyrightText: © 2025 Matthew Spinks
 * SPDX-License-Identifier: Zlib
 * 
 * This software is released under the Zlib license. You are free alter and
 * redistribute it, but you must not misrepresent the origin of the software.
 * This notice may not be removed. <http://www.zlib.net/zlib_license.html>
 * 
 ******************************************************************************/

#include "II2CTarget.h"
#include <stddef.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************


// ***** Static Functions Prototypes *******************************************


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void I2CTarget_Create(I2CTarget *self, void *instanceOfSubclass, I2CTargetInterface *interface)
{
    self->instance = instanceOfSubclass;
    self->interface = interface;
}

// *****************************************************************************

void I2CTarget_CreateInitType(I2CTargetInitType *params, void *instanceOfSubClass)
{
    params->instance = instanceOfSubClass;
}

// *****************************************************************************

void I2CTarget_BaseInit(I2CTarget *self, uint8_t targetAddress7Bit)
{
    self->targetAddress7Bit = targetAddress7Bit;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void I2CTarget_Init(I2CTarget *self, I2CTargetInitType *params)
{
    if(self->interface->I2CTarget_Init != NULL && self->instance != NULL 
        && params->instance != NULL)
    {
        (self->interface->I2CTarget_Init)(self->instance, params->instance);
    }
}

// *****************************************************************************

void I2CTarget_DataTransferFinishedEvent(I2CTarget *self, I2CTargetTransferFinishedStatus *finishedMessage, 
    I2CTargetTransferStatus *transferReport)
{
    if(self->interface->I2CTarget_DataTransferFinishedEvent != NULL && self->instance != NULL)
    {
        (self->interface->I2CTarget_DataTransferFinishedEvent)(self->instance, 
            finishedMessage, transferReport);
    }
}

// *****************************************************************************

void I2CTarget_IsDataTransferFinished(I2CTarget *self, I2CTargetTransferFinishedStatus *retFinishedMessage)
{
    if(self->interface->I2CTarget_IsDataTransferFinished != NULL && self->instance != NULL)
    {
        (self->interface->I2CTarget_IsDataTransferFinished)(self->instance, 
            retFinishedMessage);
    }
}

// *****************************************************************************

void I2CTarget_GetFinishedDataTransfer(I2CTarget *self, I2CTargetTransferStatus *retTransferReport)
{
    if(self->interface->I2CTarget_GetFinishedDataTransfer != NULL && self->instance != NULL)
    {
        (self->interface->I2CTarget_GetFinishedDataTransfer)(self->instance, 
            retTransferReport);
    }
}

// *****************************************************************************

void I2CTarget_WriteToDataTransferBuffer(I2CTarget *self, bool readTypeTransfer, 
    uint8_t *dataArray, uint16_t length)
{
    if(self->interface->I2CTarget_WriteToDataTransferBuffer != NULL && self->instance != NULL)
    {
        (self->interface->I2CTarget_WriteToDataTransferBuffer)(self->instance, 
            readTypeTransfer, dataArray, length);
    }
}

// *****************************************************************************

void I2CTarget_DataTransferStartedEvent(I2CTarget *self)
{
    if(self->interface->I2CTarget_DataTransferStartedEvent != NULL && self->instance != NULL)
    {
        (self->interface->I2CTarget_DataTransferStartedEvent)(self->instance);
    }
}

// *****************************************************************************

bool I2CTarget_IsDataTransferStarted(I2CTarget *self)
{
    if(self->interface->I2CTarget_IsDataTransferStarted != NULL && self->instance != NULL)
    {
        return (self->interface->I2CTarget_IsDataTransferStarted)(self->instance);
    }
    else
    {
        return false;
    }
}

// *****************************************************************************

void I2CTarget_ClearDataTransferStartedFlag(I2CTarget *self)
{
    if(self->interface->I2CTarget_ClearDataTransferStartedFlag != NULL && self->instance != NULL)
    {
        (self->interface->I2CTarget_ClearDataTransferStartedFlag)(self->instance);
    }
}

// *****************************************************************************

void I2CTarget_ReadFromDataTransferBuffer(I2CTarget *self, bool *retIsReadType, 
    uint8_t **retPtrArray, uint16_t *retLength)
{
    if(self->interface->I2CTarget_ReadFromDataTransferBuffer != NULL && self->instance != NULL)
    {
        (self->interface->I2CTarget_ReadFromDataTransferBuffer)(self->instance, 
            retIsReadType, retPtrArray, retLength);
    }
}

// *****************************************************************************

uint8_t I2CTarget_GetDataTransferBufferCount(I2CTarget *self)
{
    if(self->interface->I2CTarget_GetDataTransferBufferCount != NULL && self->instance != NULL)
    {
        return (self->interface->I2CTarget_GetDataTransferBufferCount)(self->instance);
    }
    else
    {
        return 0;
    }
}

// *****************************************************************************

bool I2CTarget_IsDataTransferBufferFull(I2CTarget *self)
{
    if(self->interface->I2CTarget_IsDataTransferBufferFull != NULL && self->instance != NULL)
    {
        return (self->interface->I2CTarget_IsDataTransferBufferFull)(self->instance);
    }
    else
    {
        return false;
    }
}

// *****************************************************************************

bool I2CTarget_IsDataTransferBufferNotEmpty(I2CTarget *self)
{
    if(self->interface->I2CTarget_IsDataTransferBufferNotEmpty != NULL && self->instance != NULL)
    {
        return (self->interface->I2CTarget_IsDataTransferBufferNotEmpty)(self->instance);
    }
    else
    {
        return false;
    }
}

// *****************************************************************************

uint8_t I2CTarget_GetDataTransferBufferSize(I2CTarget *self)
{
    if(self->interface->I2CTarget_GetDataTransferBufferSize != NULL && self->instance != NULL)
    {
        return (self->interface->I2CTarget_GetDataTransferBufferSize)(self->instance);
    }
    else
    {
        return 0;
    }
}

// *****************************************************************************

void I2CTarget_ClearDataTransferBuffer(I2CTarget *self)
{
    if(self->interface->I2CTarget_ClearDataTransferBuffer != NULL && self->instance != NULL)
    {
        (self->interface->I2CTarget_ClearDataTransferBuffer)(self->instance);
    }
}

// *****************************************************************************

I2CTargetState I2CTarget_GetState(I2CTarget *self)
{
    I2CTargetState retVal = I2CTARGET_STATE_IDLE;

    if(self->interface->I2CTarget_GetState != NULL && self->instance != NULL)
    {
        retVal = (self->interface->I2CTarget_GetState)(self->instance);
    }
    return retVal;
}

/*
 End of File
 */
