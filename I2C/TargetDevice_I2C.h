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

#ifndef TARGETDEVICE_I2C_H
#define TARGETDEVICE_I2C_H

#include "ITargetDevice.h"
#include "DataTransfer.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/* If you need to extend the base class, then declare your processor specific
class here. Your processor specific functions should all use this type in place 
of the base class type. */
typedef struct TargetDevice_I2CTag
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
} TargetDevice_I2C;

typedef struct TargetDeviceInitType_I2CTag
{
    TargetDeviceInitType *super; // include the base class first
    uint8_t targetAddress7Bit; // 7-bit address, right justified
    DataTransfer *ptrToDTArray;
    uint8_t dtArraySize;
} TargetDeviceInitType_I2C;

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
void TargetDevice_I2C_Create(TargetDevice_I2C *self, TargetDevice *base);

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 * @param base 
 */
void TargetDevice_I2C_CreateInitType(TargetDeviceInitType_I2C *self, TargetDeviceInitType *base);

/***************************************************************************//**
 * @brief 
 * 
 * @param params 
 * @param targetAddress7Bit 
 * @param dtArray 
 * @param arraySize 
 */
void TargetDevice_I2C_SetInitTypeParams(TargetDeviceInitType_I2C *params, uint8_t targetAddress7Bit, 
    DataTransfer *dtArray, uint8_t dtArraySize);

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void TargetDevice_I2C_Init(TargetDevice_I2C *self, TargetDeviceInitType_I2C *params);

void TargetDevice_I2C_DataTransferFinishedEvent(TargetDevice_I2C *self, TargetDeviceTransferFinishedStatus *finishedMessage, 
    TargetDeviceTransferStatus *transferReport);

void TargetDevice_I2C_IsDataTransferFinished(TargetDevice_I2C *self, TargetDeviceTransferFinishedStatus *retFinishedMessage);

void TargetDevice_I2C_GetFinishedDataTransfer(TargetDevice_I2C *self, TargetDeviceTransferStatus *retTransferReport);

void TargetDevice_I2C_WriteToDataTransferBuffer(TargetDevice_I2C *self, bool readTypeTransfer, 
    uint8_t *dataArray, uint16_t length);

void TargetDevice_I2C_DataTransferStartedEvent(TargetDevice_I2C *self);

bool TargetDevice_I2C_IsDataTransferStarted(TargetDevice_I2C *self);

void TargetDevice_I2C_ClearDataTransferStartedFlag(TargetDevice_I2C *self);

void TargetDevice_I2C_ReadFromDataTransferBuffer(TargetDevice_I2C *self, bool *retIsReadType, 
    uint8_t **retPtrArray, uint16_t *retLength);

uint8_t TargetDevice_I2C_GetDataTransferBufferCount(TargetDevice_I2C *self);

bool TargetDevice_I2C_IsDataTransferBufferFull(TargetDevice_I2C *self);

bool TargetDevice_I2C_IsDataTransferBufferNotEmpty(TargetDevice_I2C *self);

uint8_t TargetDevice_I2C_GetDataTransferBufferSize(TargetDevice_I2C *self);

void TargetDevice_I2C_ClearDataTransferBuffer(TargetDevice_I2C *self);

TargetDeviceState TargetDevice_I2C_GetState(TargetDevice_I2C *self);

#endif /* TARGETDEVICE_I2C_H */
