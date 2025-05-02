/***************************************************************************//**
 * @brief I2C Target Implementation Header (Non-Processor Specific)
 * 
 * @file I2CTarget.h
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

/* @todo Should I stick with my usual naming convention of "Class_Subclass.." 
or change the name of the file to I2CTarget to shorten it a little bit ? - MS */

#ifndef TARGET_DEVICE_I2C_H
#define TARGET_DEVICE_I2C_H

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

    uint8_t targetAddress7Bit; // 7-bit address, right justified
    struct
    {
        TargetDeviceState state;
        DTBuffer dtBuffer;
        union {
            struct {
                unsigned transferPending        :1;
                unsigned transferStarted        :1;
                unsigned transferFinished       :1;
                unsigned                        :5;
            };
            uint8_t all;
        } flags;
    } private;
    DataTransfer finishedTransfer;
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
 * @param self 
 * @param targetAddress7Bit 
 */
void TargetDevice_I2C_SetInitTypeParams(TargetDevice_I2C *self, uint8_t targetAddress7Bit);

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

// @todo doxygen

void TargetDevice_I2C_Init(TargetDevice_I2C *self, TargetDeviceInitType_I2C *params);

void TargetDevice_I2C_DataTransferFinishedEvent(TargetDevice_I2C *self);

bool TargetDevice_I2C_IsDataTransferFinished(TargetDevice_I2C *self);

void TargetDevice_I2C_GetFinishedDataTransfer(TargetDevice_I2C *self, bool *retIsReadType, 
    uint8_t **retPtrArray, uint16_t *retLength);

// place in buffer
void TargetDevice_I2C_RequestDataTransfer(TargetDevice_I2C *self, bool readTypeTransfer, 
    uint8_t *dataArray, uint16_t length);

// not pending anymore
void TargetDevice_I2C_DataTransferStartedEvent(TargetDevice_I2C *self);

bool TargetDevice_I2C_IsDataTransferPending(TargetDevice_I2C *self);

// sets transferPending to false after calling
void TargetDevice_I2C_GetPendingDataTransfer(TargetDevice_I2C *self, bool *retIsReadType, 
    uint8_t **retPtrArray, uint16_t *retLength);

uint8_t TargetDevice_I2C_GetDataTransferBufferCount(TargetDevice_I2C *self);

bool TargetDevice_I2C_IsDataTransferBufferFull(TargetDevice_I2C *self);

uint8_t TargetDevice_I2C_GetDataTransferBufferSize(TargetDevice_I2C *self);

void TargetDevice_I2C_ClearDataTransferBuffer(TargetDevice_I2C *self);

TargetDeviceState TargetDevice_I2C_GetState(TargetDevice_I2C *self);

#endif /* TARGET_DEVICE_I2C_H */
