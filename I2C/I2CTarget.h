/***************************************************************************//**
 * @brief I2C Target Header File
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

#ifndef I2C_TARGET_H
#define I2C_TARGET_H

#include <stdint.h>
#include <stdbool.h>

/* @note @todo For now I'm going to go ahead and make the I2C target dependent 
on the data transfer type, while I think of different ways to implement the 
library. It is sort of universal type object anyway, so I might end up keeping 
it like this. The only function that really needs it right now is the receive 
function. In a way it behaves sort of like the normal receive function, 
except that instead of returning a byte, it is returning an object. - MS */
#include "DataTransfer.h"

// ***** Defines ***************************************************************

/* @note The size of the data transfer buffer should be one more than the 
amount that you would like the target to be able to hold at once. I would 
suggest a minimum size of 3. That is enough to hold one write data request, 
followed by one read data request. - MS */
#define I2CTARGET_DT_BUFFER_SIZE 3 // @remove later

// ***** Global Variables ******************************************************

typedef enum I2CTargetStateTag
{
    I2C_TARGET_STATE_IDLE = 0,
    I2C_TARGET_STATE_TRANSFER_IN_PROGRESS,
    I2C_TARGET_STATE_ERROR,
} I2CTargetState;

// @todo decided if I want to keep the old callback function pointers
/* callback function pointer. The context is so that you can know which of
your I2C devices initiated the callback. */
// typedef void (*I2CTargetCallbackFunc)(I2CTarget *i2cTargetContext);

/* @todo decide if I want to make this the base class or not */
typedef struct I2CTargetTag
{
    uint8_t targetAddress7Bit; // 7-bit address, right justified
    I2CTargetState state;
    bool transferStartedEventFlag;
    bool transferFinishedEventFlag; // @todo might leave these to sub class to implement
    void *instance;

    // struct
    // {
    //     DTBuffer *buffer;
    //     DataTransfer dtArray[I2CTARGET_DT_BUFFER_SIZE];
    // } private;
} I2CTarget;

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Function Prototypes *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void I2CTarget_Init(I2CTarget *self, uint8_t targetAddress7Bit);

void I2CTarget_WriteDTObject(I2CTarget *self, bool readTypeTransfer, uint8_t *array, uint16_t length);

// @todo should I2CTarget be dependent on DataTransfer.h?
uint8_t I2CTarget_ReadDTObject(I2CTarget *self, DataTransfer *returnDataTransfer);

// @todo should these become part of I2CTarget_Node?
// I2CTarget_IsReadyForDataTransfer

uint8_t I2CTarget_GetDTBufferCount(I2CTarget *self);

bool I2CTarget_IsDTBufferFull(I2CTarget *self);

bool I2CTarget_IsDTBufferNotEmpty(I2CTarget *self);

uint8_t I2CTarget_GetDTBufferSize(I2CTarget *self);

void I2CTarget_FlushDTBuffer(I2CTarget *self);

bool I2CTarget_GetDTStartedEvent(I2CTarget *self);

void I2CTarget_ClearDTStartedEventFlag(I2CTarget *self);

bool I2CTarget_GetDTFinishedEvent(I2CTarget *self);

void I2CTarget_ClearDTFinishedEventFlag(I2CTarget *self);

I2CTargetState I2CTarget_GetState(I2CTarget *self);

// @todo possible callback functions
// I2CTarget_TransferFinishedCallback
// I2CTarget_WriteTransferFinishedCallback
// I2CTarget_ReadTransferFinishedCallback

#endif /* I2C_TARGET_H */
