/***************************************************************************//**
 * @brief Target Device Interface
 * 
 * @file ITargetDevice.c
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

#include "ITargetDevice.h"
#include <stddef.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************


// ***** Static Functions Prototypes *******************************************


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void TargetDevice_Create(TargetDevice *self, void *instanceOfSubclass, TargetDeviceInterface *interface)
{
    self->instance = instanceOfSubclass;
    self->interface = interface;
}

// *****************************************************************************

void TargetDevice_CreateInitType(TargetDeviceInitType *params, void *instanceOfSubClass)
{
    params->instance = instanceOfSubClass;
}

// *****************************************************************************

void TargetDevice_BaseInit(TargetDevice *self, uint8_t targetAddress7Bit)
{
    self->targetAddress7Bit = targetAddress7Bit;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void TargetDevice_Init(TargetDevice *self, TargetDeviceInitType *params)
{
    if(self->interface->TargetDevice_Init != NULL && self->instance != NULL 
        && params->instance != NULL)
    {
        (self->interface->TargetDevice_Init)(self->instance, params->instance);
    }
}

// *****************************************************************************

void TargetDevice_DataTransferFinishedEvent(TargetDevice *self, TargetDeviceTransferFinishedStatus *finishedMessage, 
    TargetDeviceTransferStatus *transferReport)
{
    if(self->interface->TargetDevice_DataTransferFinishedEvent != NULL && self->instance != NULL)
    {
        (self->interface->TargetDevice_DataTransferFinishedEvent)(self->instance, 
            finishedMessage, transferReport);
    }
}

// *****************************************************************************

void TargetDevice_IsDataTransferFinished(TargetDevice *self, TargetDeviceTransferFinishedStatus *retFinishedMessage)
{
    if(self->interface->TargetDevice_IsDataTransferFinished != NULL && self->instance != NULL)
    {
        (self->interface->TargetDevice_IsDataTransferFinished)(self->instance, 
            retFinishedMessage);
    }
}

// *****************************************************************************

void TargetDevice_GetFinishedDataTransfer(TargetDevice *self, TargetDeviceTransferStatus *retTransferReport)
{
    if(self->interface->TargetDevice_GetFinishedDataTransfer != NULL && self->instance != NULL)
    {
        (self->interface->TargetDevice_GetFinishedDataTransfer)(self->instance, 
            retTransferReport);
    }
}

// *****************************************************************************

void TargetDevice_WriteToDataTransferBuffer(TargetDevice *self, bool readTypeTransfer, 
    uint8_t *dataArray, uint16_t length)
{
    if(self->interface->TargetDevice_WriteToDataTransferBuffer != NULL && self->instance != NULL)
    {
        (self->interface->TargetDevice_WriteToDataTransferBuffer)(self->instance, 
            readTypeTransfer, dataArray, length);
    }
}

// *****************************************************************************

void TargetDevice_DataTransferStartedEvent(TargetDevice *self)
{
    if(self->interface->TargetDevice_DataTransferStartedEvent != NULL && self->instance != NULL)
    {
        (self->interface->TargetDevice_DataTransferStartedEvent)(self->instance);
    }
}

// *****************************************************************************

bool TargetDevice_IsDataTransferStarted(TargetDevice *self)
{
    if(self->interface->TargetDevice_IsDataTransferStarted != NULL && self->instance != NULL)
    {
        return (self->interface->TargetDevice_IsDataTransferStarted)(self->instance);
    }
    else
    {
        return false;
    }
}

// *****************************************************************************

void TargetDevice_ClearDataTransferStartedFlag(TargetDevice *self)
{
    if(self->interface->TargetDevice_ClearDataTransferStartedFlag != NULL && self->instance != NULL)
    {
        (self->interface->TargetDevice_ClearDataTransferStartedFlag)(self->instance);
    }
}

// *****************************************************************************

void TargetDevice_ReadFromDataTransferBuffer(TargetDevice *self, bool *retIsReadType, 
    uint8_t **retPtrArray, uint16_t *retLength)
{
    if(self->interface->TargetDevice_ReadFromDataTransferBuffer != NULL && self->instance != NULL)
    {
        (self->interface->TargetDevice_ReadFromDataTransferBuffer)(self->instance, 
            retIsReadType, retPtrArray, retLength);
    }
}

// *****************************************************************************

uint8_t TargetDevice_GetDataTransferBufferCount(TargetDevice *self)
{
    if(self->interface->TargetDevice_GetDataTransferBufferCount != NULL && self->instance != NULL)
    {
        return (self->interface->TargetDevice_GetDataTransferBufferCount)(self->instance);
    }
    else
    {
        return 0;
    }
}

// *****************************************************************************

bool TargetDevice_IsDataTransferBufferFull(TargetDevice *self)
{
    if(self->interface->TargetDevice_IsDataTransferBufferFull != NULL && self->instance != NULL)
    {
        return (self->interface->TargetDevice_IsDataTransferBufferFull)(self->instance);
    }
    else
    {
        return false;
    }
}

// *****************************************************************************

bool TargetDevice_IsDataTransferBufferNotEmpty(TargetDevice *self)
{
    if(self->interface->TargetDevice_IsDataTransferBufferNotEmpty != NULL && self->instance != NULL)
    {
        return (self->interface->TargetDevice_IsDataTransferBufferNotEmpty)(self->instance);
    }
    else
    {
        return false;
    }
}

// *****************************************************************************

uint8_t TargetDevice_GetDataTransferBufferSize(TargetDevice *self)
{
    if(self->interface->TargetDevice_GetDataTransferBufferSize != NULL && self->instance != NULL)
    {
        return (self->interface->TargetDevice_GetDataTransferBufferSize)(self->instance);
    }
    else
    {
        return 0;
    }
}

// *****************************************************************************

void TargetDevice_ClearDataTransferBuffer(TargetDevice *self)
{
    if(self->interface->TargetDevice_ClearDataTransferBuffer != NULL && self->instance != NULL)
    {
        (self->interface->TargetDevice_ClearDataTransferBuffer)(self->instance);
    }
}

// *****************************************************************************

TargetDeviceState TargetDevice_GetState(TargetDevice *self)
{
    TargetDeviceState retVal = TARGETDEVICE_STATE_IDLE;

    if(self->interface->TargetDevice_GetState != NULL && self->instance != NULL)
    {
        retVal = (self->interface->TargetDevice_GetState)(self->instance);
    }
    return retVal;
}

/*
 End of File
 */
