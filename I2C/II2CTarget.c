/***************************************************************************//**
 * @brief I2C Target Library
 * 
 * @file I2CTarget.h
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

void I2CTarget_CreateInitType(I2CTargetInitType *params, void *instanceOfSubClass)
{
    params->instance = instanceOfSubClass;
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

void I2CTarget_DataTransferFinishedEvent(I2CTarget *self)
{
    if(self->interface->I2CTarget_DataTransferFinishedEvent != NULL && self->instance != NULL)
    {
        (self->interface->I2CTarget_DataTransferFinishedEvent)(self->instance);
    }
}

// *****************************************************************************

bool I2CTarget_IsDataTransferFinished(I2CTarget *self)
{
    if(self->interface->I2CTarget_IsDataTransferFinished != NULL && self->instance != NULL)
    {
        return (self->interface->I2CTarget_IsDataTransferFinished)(self->instance);
    }
    else
    {
        return false;
    }
}

// *****************************************************************************

void I2CTarget_GetFinishedDataTransfer(I2CTarget *self)
{
    if(self->interface->I2CTarget_GetFinishedDataTransfer != NULL && self->instance != NULL)
    {
        (self->interface->I2CTarget_GetFinishedDataTransfer)(self->instance);
    }
}

// *****************************************************************************

void I2CTarget_RequestDataTransfer(I2CTarget *self, bool readTypeTransfer, 
    uint8_t *array, uint16_t length)
{
    if(self->interface->I2CTarget_RequestDataTransfer != NULL && self->instance != NULL)
    {
        (self->interface->I2CTarget_RequestDataTransfer)(self->instance, 
            readTypeTransfer, array, length);
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

bool I2CTarget_IsDataTransferPending(I2CTarget *self)
{
    if(self->interface->I2CTarget_IsDataTransferPending != NULL && self->instance != NULL)
    {
        return (self->interface->I2CTarget_IsDataTransferPending)(self->instance);
    }
    else
    {
        return false;
    }
}

// *****************************************************************************

void I2CTarget_GetPendingDataTransfer(I2CTarget *self, bool *retIsReadType, 
    uint8_t *retPtrArray, uint16_t *retLength)
{
    if(self->interface->I2CTarget_GetPendingDataTransfer != NULL && self->instance != NULL)
    {
        (self->interface->I2CTarget_GetPendingDataTransfer)(self->instance, 
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
    I2CTargetState retVal = I2C_TARGET_STATE_ERROR;

    if(self->interface->I2CTarget_GetState != NULL && self->instance != NULL)
    {
        retVal = (self->interface->I2CTarget_GetState)(self->instance);
    }
    return retVal;
}

/*
 End of File
 */
