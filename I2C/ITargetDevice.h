/***************************************************************************//**
 * @brief Target Device Interface Header
 * 
 * @file ITargetDevice.h
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 5/1/25    Original creation
 * @date 7/1/25    Re-factored into generic target device
 * 
 * @details
 *      @todo add more details. I decided to take my I2C slave code that was 
 * originally for the I2C manager and break it out into a more generic object. 
 * While I'm at it, I'll update the terminology to use "controller" and 
 * "target" since that is what is preferred now.
 * 
 * @todo re-factoring I2C target code into more generic target device which 
 * can work with SPI also. - MS
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

#ifndef ITARGETDEVICE_H
#define ITARGETDEVICE_H

#include <stdint.h>
#include <stdbool.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

typedef enum TargetDeviceStateTag
{
    I2CTARGET_STATE_IDLE = 0,
    I2CTARGET_STATE_TRANSFER_IN_PROGRESS,
} TargetDeviceState;

typedef enum TargetDeviceTransferFinishedStatusTag
{
    I2CTARGET_TRANSFER_NOT_FINISHED = 0,
    I2CTARGET_TRANSFER_FINISHED_SUCCESS,
    I2CTARGET_TRANSFER_FINISHED_FAIL
} TargetDeviceTransferFinishedStatus;

// @todo I'm considering making the data transfer type part of this file. Or at least making its own version
typedef struct TargetDeviceTransferStatus
{
    bool isReadType;
    uint8_t *ptrArray;
    uint16_t sizeOfArray;
    uint16_t numBytesTransferred;
} TargetDeviceTransferStatus;

typedef struct TargetDeviceInterfaceTag
{
    void (*TargetDevice_Init)(void *instance, void *params);
    void (*TargetDevice_DataTransferFinishedEvent)(void *instance, TargetDeviceTransferFinishedStatus *, TargetDeviceTransferStatus *);
    void (*TargetDevice_IsDataTransferFinished)(void *instance, TargetDeviceTransferFinishedStatus *);
    void (*TargetDevice_GetFinishedDataTransfer)(void *instance, TargetDeviceTransferStatus *);
    void (*TargetDevice_WriteToDataTransferBuffer)(void *instance, bool, uint8_t *, uint16_t);
    void (*TargetDevice_DataTransferStartedEvent)(void *instance);
    bool (*TargetDevice_IsDataTransferStarted)(void *instance);
    void (*TargetDevice_ClearDataTransferStartedFlag)(void *instance);
    void (*TargetDevice_ReadFromDataTransferBuffer)(void *instance, bool *, uint8_t **, uint16_t *);
    uint8_t (*TargetDevice_GetDataTransferBufferCount)(void *instance);
    bool (*TargetDevice_IsDataTransferBufferFull)(void *instance);
    bool (*TargetDevice_IsDataTransferBufferNotEmpty)(void *instance);
    uint8_t (*TargetDevice_GetDataTransferBufferSize)(void *instance);
    void (*TargetDevice_ClearDataTransferBuffer)(void *instance);
    TargetDeviceState (*TargetDevice_GetState)(void *instance);
} TargetDeviceInterface;

// @todo decided if I want to keep the old callback function pointers
/* callback function pointer. The context is so that you can know which of
your I2C devices initiated the callback. */
// typedef void (*TargetDeviceCallbackFunc)(TargetDevice *targetContext);

typedef struct TargetDeviceTag
{
    TargetDeviceInterface *interface;
    uint8_t targetAddress7Bit; // 7-bit address, right justified // @todo need to move to an I2C sub class
    void *instance;
} TargetDevice;

typedef struct TargetDeviceInitTypeTage
{
    void *instance;
} TargetDeviceInitType;

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

void TargetDevice_Create(TargetDevice *self, void *instanceOfSubclass, TargetDeviceInterface *interface);

void TargetDevice_CreateInitType(TargetDeviceInitType *params, void *instanceOfSubClass);

void TargetDevice_BaseInit(TargetDevice *self, uint8_t targetAddress7Bit);

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

// @note make sure this function initializes the base class
void TargetDevice_Init(TargetDevice *self, TargetDeviceInitType *params);

void TargetDevice_DataTransferFinishedEvent(TargetDevice *self, TargetDeviceTransferFinishedStatus *finishedMessage, 
    TargetDeviceTransferStatus *transferReport);

void TargetDevice_IsDataTransferFinished(TargetDevice *self, TargetDeviceTransferFinishedStatus *retFinishedMessage);

/* @follow-up As of right now, I've decided that getting the received data using 
this function should clear the transfer finished flag. - MS */
void TargetDevice_GetFinishedDataTransfer(TargetDevice *self, TargetDeviceTransferStatus *retTransferReport);

void TargetDevice_WriteToDataTransferBuffer(TargetDevice *self, bool readTypeTransfer, 
    uint8_t *dataArray, uint16_t length);

void TargetDevice_DataTransferStartedEvent(TargetDevice *self);

bool TargetDevice_IsDataTransferStarted(TargetDevice *self);

/* @follow-up Adding for completeness. I wasn't sure if I wanted read from 
buffer to clear the transfer started flag or not, so I added this for now. - MS */
void TargetDevice_ClearDataTransferStartedFlag(TargetDevice *self);

// Set return length to 0 if no data available
void TargetDevice_ReadFromDataTransferBuffer(TargetDevice *self, bool *retIsReadType, 
    uint8_t **retPtrArray, uint16_t *retLength);

uint8_t TargetDevice_GetDataTransferBufferCount(TargetDevice *self);

bool TargetDevice_IsDataTransferBufferFull(TargetDevice *self);

bool TargetDevice_IsDataTransferBufferNotEmpty(TargetDevice *self);

uint8_t TargetDevice_GetDataTransferBufferSize(TargetDevice *self);

void TargetDevice_ClearDataTransferBuffer(TargetDevice *self);

TargetDeviceState TargetDevice_GetState(TargetDevice *self);

// @todo add function to set state?
// @todo add separate function to reset started/finished flags?
// @todo possible callback functions
// TargetDevice_SetTransferFinishedCallbackFunc

#endif /* ITARGETDEVICE_H */
