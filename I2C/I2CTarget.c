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
 * SPDX-FileCopyrightText: © 2019 Matthew Spinks
 * SPDX-License-Identifier: Zlib
 * 
 * This software is released under the Zlib license. You are free alter and
 * redistribute it, but you must not misrepresent the origin of the software.
 * This notice may not be removed. <http://www.zlib.net/zlib_license.html>
 * 
 ******************************************************************************/

#include "I2CTarget.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************


// ***** Static Functions Prototypes *******************************************


// *****************************************************************************

void I2CTarget_Init(I2CTarget *self, I2CTargetInitType *params)
{
    self->targetAddress7Bit = params->targetAddress7Bit;

    self->state = I2C_TARGET_STATE_IDLE;
    self->transferStartedEventFlag = false;
    self->transferFinishedEventFlag = false;

    // initialize buffer
    DTBuffer_Init(&(self->private.buffer), &(self->private.dtArray), I2CSLAVE_DT_BUFFER_SIZE);

}

// *****************************************************************************

// @todo change these to interface functions?

void I2CTarget_WriteDTObject(I2CTarget *self, bool readTypeTransfer, uint8_t *array, uint16_t length)
{
    DTBuffer_WriteDataTransfer(&(self->private.buffer), writeOrRead, array, length);
}

// *****************************************************************************

uint8_t I2CTarget_ReadDTObject(I2CTarget *self, DataTransfer *returnDataTransfer)
{
    return DTBuffer_ReadDataTransfer(&(self->private.buffer), returnDataTransfer);
}

// *****************************************************************************

uint8_t I2CTarget_GetDTBufferCount(I2CTarget *self)
{
    return DTBuffer_GetCount(&(self->private.buffer));
}

// *****************************************************************************

bool I2CTarget_IsDTBufferFull(I2CTarget *self)
{
    return DTBuffer_IsFull(&(self->private.buffer));
}

// *****************************************************************************

bool I2CTarget_IsDTBufferNotEmpty(I2CTarget *self)
{
    return DTBuffer_IsNotEmpty(&(self->private.buffer));
}

// *****************************************************************************

uint8_t I2CTarget_GetDTBufferSize(I2CTarget *self)
{
    return DTBuffer_GetCount(&(self->private.buffer));
}

// *****************************************************************************

void I2CTarget_FlushDTBuffer(I2CTarget *self)
{
    DTBuffer_Flush(&(self->private.buffer));
}

// *****************************************************************************

bool I2CTarget_GetDTStartedEvent(I2CTarget *self)
{
    return self->private.transferStartedEventFlag;
}

// *****************************************************************************

void I2CTarget_ClearDTStartedEventFlag(I2CTarget *self)
{
    self->private.transferStartedEventFlag = false;
}

// *****************************************************************************

bool I2CTarget_GetDTFinishedEvent(I2CTarget *self)
{
    return self->private.transferFinishedEventFlag;
}

// *****************************************************************************

void I2CTarget_ClearDTFinishedEventFlag(I2CTarget *self)
{
    self->private.transferFinishedEventFlag = false;
}

// *****************************************************************************

I2CTargetState I2CTarget_GetState(I2CTarget *self)
{
    return self->state;
}

/*
 End of File
 */
