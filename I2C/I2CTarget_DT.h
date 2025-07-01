/***************************************************************************//**
 * @brief I2C Target Device Implementation Header
 * 
 * @file TargetDevice_I2C.h
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 5/1/25    Original creation
 * 
 * @details
 *      @todo add more details. Generic I2C target object. "DT" stands for 
 * data transfer as this implementation will include my data transfer buffer 
 * library that I made. - MS
 * 
 * @note The size of the data transfer buffer can be a minimum of 1. I would 
 * suggest a minimum size of 2. That is enough to hold one write data request, 
 * followed by one read data request. So that you could perform a repeated start 
 * if needed. - MS 
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

#ifndef I2CTARGET_DT_H
#define I2CTARGET_DT_H

#include "ITargetDevice.h"
#include "DataTransfer.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/* If you need to extend the base class, then declare your processor specific
class here. Your processor specific functions should all use this type in place 
of the base class type. */
typedef struct TargetDevice_DTTag
{
    TargetDevice *super; // include the base class first
    TargetDeviceTransferStatus finishedTransfer;
    struct
    {
        TargetDeviceState state;
        DTBuffer dtBuffer;
        bool transferStartedFlag;
        TargetDeviceTransferFinishedStatus transferFinishedStatus;
    } private;
} TargetDevice_DT;

typedef struct TargetDeviceInitType_DTTag
{
    TargetDeviceInitType *super; // include the base class first
    uint8_t targetAddress7Bit; // 7-bit address, right justified
    DataTransfer *ptrToDTArray;
    uint8_t dtArraySize;
} TargetDeviceInitType_DT;

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

/***************************************************************************//**
 * @brief Connects the sub class to the base class
 * 
 * @param self
 * @param base
 */
void TargetDevice_DT_Create(TargetDevice_DT *self, TargetDevice *base);

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 * @param base 
 */
void TargetDevice_DT_CreateInitType(TargetDeviceInitType_DT *self, TargetDeviceInitType *base);

/***************************************************************************//**
 * @brief 
 * 
 * @param params 
 * @param targetAddress7Bit 
 * @param dtArray 
 * @param arraySize 
 */
void TargetDevice_DT_SetInitTypeParams(TargetDeviceInitType_DT *params, uint8_t targetAddress7Bit, 
    DataTransfer *dtArray, uint8_t dtArraySize);

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void TargetDevice_DT_Init(TargetDevice_DT *self, TargetDeviceInitType_DT *params);

void TargetDevice_DT_DataTransferFinishedEvent(TargetDevice_DT *self, TargetDeviceTransferFinishedStatus *finishedMessage, 
    TargetDeviceTransferStatus *transferReport);

void TargetDevice_DT_IsDataTransferFinished(TargetDevice_DT *self, TargetDeviceTransferFinishedStatus *retFinishedMessage);

void TargetDevice_DT_GetFinishedDataTransfer(TargetDevice_DT *self, TargetDeviceTransferStatus *retTransferReport);

void TargetDevice_DT_WriteToDataTransferBuffer(TargetDevice_DT *self, bool readTypeTransfer, 
    uint8_t *dataArray, uint16_t length);

void TargetDevice_DT_DataTransferStartedEvent(TargetDevice_DT *self);

bool TargetDevice_DT_IsDataTransferStarted(TargetDevice_DT *self);

void TargetDevice_DT_ClearDataTransferStartedFlag(TargetDevice_DT *self);

void TargetDevice_DT_ReadFromDataTransferBuffer(TargetDevice_DT *self, bool *retIsReadType, 
    uint8_t **retPtrArray, uint16_t *retLength);

uint8_t TargetDevice_DT_GetDataTransferBufferCount(TargetDevice_DT *self);

bool TargetDevice_DT_IsDataTransferBufferFull(TargetDevice_DT *self);

bool TargetDevice_DT_IsDataTransferBufferNotEmpty(TargetDevice_DT *self);

uint8_t TargetDevice_DT_GetDataTransferBufferSize(TargetDevice_DT *self);

void TargetDevice_DT_ClearDataTransferBuffer(TargetDevice_DT *self);

TargetDeviceState TargetDevice_DT_GetState(TargetDevice_DT *self);

#endif /* I2CTARGET_DT_H */
