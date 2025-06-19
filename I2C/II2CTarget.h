/***************************************************************************//**
 * @brief I2C Target Device Interface Header
 * 
 * @file II2CTarget.h
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
 * @todo should I just go ahead and change this file to generic TargetType 
 * while I'm at it?
 * 
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

#ifndef II2CTARGET_H
#define II2CTARGET_H

#include <stdint.h>
#include <stdbool.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

typedef enum I2CTargetStateTag
{
    I2CTARGET_STATE_IDLE = 0,
    I2CTARGET_STATE_TRANSFER_IN_PROGRESS,
} I2CTargetState;

typedef enum I2CTargetTransferFinishedStatusTag
{
    I2CTARGET_TRANSFER_NOT_FINISHED = 0,
    I2CTARGET_TRANSFER_FINISHED_SUCCESS,
    I2CTARGET_TRANSFER_FINISHED_FAIL
} I2CTargetTransferFinishedStatus;

// @todo I'm considering making the data transfer type part of this file. Or at least making its own version
typedef struct I2CTargetTransferStatus
{
    bool isReadType;
    uint8_t *ptrArray;
    uint16_t sizeOfArray;
    uint16_t numBytesTransferred;
} I2CTargetTransferStatus;

typedef struct I2CTargetInterfaceTag
{
    void (*I2CTarget_Init)(void *instance, void *params);
    void (*I2CTarget_DataTransferFinishedEvent)(void *instance, I2CTargetTransferFinishedStatus *, I2CTargetTransferStatus *);
    void (*I2CTarget_IsDataTransferFinished)(void *instance, I2CTargetTransferFinishedStatus *);
    void (*I2CTarget_GetFinishedDataTransfer)(void *instance, I2CTargetTransferStatus *);
    void (*I2CTarget_WriteToDataTransferBuffer)(void *instance, bool, uint8_t *, uint16_t);
    void (*I2CTarget_DataTransferStartedEvent)(void *instance);
    bool (*I2CTarget_IsDataTransferStarted)(void *instance);
    void (*I2CTarget_ClearDataTransferStartedFlag)(void *instance);
    void (*I2CTarget_ReadFromDataTransferBuffer)(void *instance, bool *, uint8_t **, uint16_t *);
    uint8_t (*I2CTarget_GetDataTransferBufferCount)(void *instance);
    bool (*I2CTarget_IsDataTransferBufferFull)(void *instance);
    bool (*I2CTarget_IsDataTransferBufferNotEmpty)(void *instance);
    uint8_t (*I2CTarget_GetDataTransferBufferSize)(void *instance);
    void (*I2CTarget_ClearDataTransferBuffer)(void *instance);
    I2CTargetState (*I2CTarget_GetState)(void *instance);
} I2CTargetInterface;

// @todo decided if I want to keep the old callback function pointers
/* callback function pointer. The context is so that you can know which of
your I2C devices initiated the callback. */
// typedef void (*I2CTargetCallbackFunc)(I2CTarget *i2cTargetContext);

typedef struct I2CTargetTag
{
    I2CTargetInterface *interface;
    uint8_t targetAddress7Bit; // 7-bit address, right justified
    void *instance;
} I2CTarget;

typedef struct I2CTargetInitTypeTage
{
    void *instance;
} I2CTargetInitType;

/** 
 * Description of struct
 * 
 * member1      description of variable member1
 * 
 */

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

// @todo doxygen

void I2CTarget_Create(I2CTarget *self, void *instanceOfSubclass, I2CTargetInterface *interface);

void I2CTarget_CreateInitType(I2CTargetInitType *params, void *instanceOfSubClass);

void I2CTarget_BaseInit(I2CTarget *self, uint8_t targetAddress7Bit);

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

// @note make sure this function initializes the base class
void I2CTarget_Init(I2CTarget *self, I2CTargetInitType *params);

void I2CTarget_DataTransferFinishedEvent(I2CTarget *self, I2CTargetTransferFinishedStatus *finishedMessage, 
    I2CTargetTransferStatus *transferReport);

void I2CTarget_IsDataTransferFinished(I2CTarget *self, I2CTargetTransferFinishedStatus *retFinishedMessage);

/* @follow-up As of right now, I've decided that getting the received data using 
this function should clear the transfer finished flag. - MS */
void I2CTarget_GetFinishedDataTransfer(I2CTarget *self, I2CTargetTransferStatus *retTransferReport);

void I2CTarget_WriteToDataTransferBuffer(I2CTarget *self, bool readTypeTransfer, 
    uint8_t *dataArray, uint16_t length);

void I2CTarget_DataTransferStartedEvent(I2CTarget *self);

bool I2CTarget_IsDataTransferStarted(I2CTarget *self);

/* @follow-up Adding for completeness. I wasn't sure if I wanted read from 
buffer to clear the transfer started flag or not, so I added this for now. - MS */
void I2CTarget_ClearDataTransferStartedFlag(I2CTarget *self);

// Set return length to 0 if no data available
void I2CTarget_ReadFromDataTransferBuffer(I2CTarget *self, bool *retIsReadType, 
    uint8_t **retPtrArray, uint16_t *retLength);

uint8_t I2CTarget_GetDataTransferBufferCount(I2CTarget *self);

bool I2CTarget_IsDataTransferBufferFull(I2CTarget *self);

bool I2CTarget_IsDataTransferBufferNotEmpty(I2CTarget *self);

uint8_t I2CTarget_GetDataTransferBufferSize(I2CTarget *self);

void I2CTarget_ClearDataTransferBuffer(I2CTarget *self);

I2CTargetState I2CTarget_GetState(I2CTarget *self);

// @todo add function to set state?
// @todo add separate function to reset started/finished flags?
// @todo possible callback functions
// I2CTarget_SetTransferFinishedCallbackFunc

#endif /* II2CTARGET_H */
