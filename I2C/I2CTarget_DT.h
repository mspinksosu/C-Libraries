/***************************************************************************//**
 * @brief I2C Target Implementation Header
 * 
 * @file I2CTarget_DT.h
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
 * @section license License
 * SPDX-FileCopyrightText: © 2025 Matthew Spinks
 * SPDX-License-Identifier: Zlib
 * 
 * This software is released under the Zlib license. You are free alter and
 * redistribute it, but you must not misrepresent the origin of the software.
 * This notice may not be removed. <http://www.zlib.net/zlib_license.html>
 * 
 ******************************************************************************/

/* @note The size of the data transfer buffer should be one more than the 
amount that you would like the target to be able to hold at once. I would 
suggest a minimum size of 3. That is enough to hold one write data request, 
followed by one read data request. So that you could perform a repeated start 
if needed. - MS */

#ifndef I2CTARGET_DT_H
#define I2CTARGET_DT_H

#include "II2CTarget.h"
#include "DataTransfer.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/* If you need to extend the base class, then declare your processor specific
class here. Your processor specific functions should all use this type in place 
of the base class type. */
typedef struct I2CTarget_DTTag
{
    I2CTarget *super; // include the base class first
    DataTransfer finishedTransfer;
    struct
    {
        I2CTargetState state;
        DTBuffer dtBuffer;
        union {
            struct {
                unsigned transferStarted        :1;
                unsigned transferFinished       :1;
                unsigned                        :6;
            };
            uint8_t all;
        } flags;
    } private;
} I2CTarget_DT;

typedef struct I2CTargetInitType_DTTag
{
    I2CTargetInitType *super; // include the base class first
    uint8_t targetAddress7Bit; // 7-bit address, right justified
    DataTransfer *ptrToDTArray;
    uint8_t dtArraySize;
} I2CTargetInitType_DT;

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
void I2CTarget_DT_Create(I2CTarget_DT *self, I2CTarget *base);

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 * @param base 
 */
void I2CTarget_DT_CreateInitType(I2CTargetInitType_DT *self, I2CTargetInitType *base);

/***************************************************************************//**
 * @brief 
 * 
 * @param params 
 * @param targetAddress7Bit 
 * @param dtArray 
 * @param arraySize 
 */
void I2CTarget_DT_SetInitTypeParams(I2CTargetInitType_DT *params, uint8_t targetAddress7Bit, 
    DataTransfer *dtArray, uint8_t dtArraySize);

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void I2CTarget_DT_Init(I2CTarget_DT *self, I2CTargetInitType_DT *params);

void I2CTarget_DT_DataTransferFinishedEvent(I2CTarget_DT *self, bool readTypeTransfer, 
    uint8_t *dataArray, uint16_t length);

bool I2CTarget_DT_IsDataTransferFinished(I2CTarget_DT *self);

void I2CTarget_DT_GetFinishedDataTransfer(I2CTarget_DT *self, bool *retIsReadType, 
    uint8_t **retPtrArray, uint16_t *retLength);

void I2CTarget_DT_WriteToDataTransferBuffer(I2CTarget_DT *self, bool readTypeTransfer, 
    uint8_t *dataArray, uint16_t length);

void I2CTarget_DT_DataTransferStartedEvent(I2CTarget_DT *self);

bool I2CTarget_DT_IsDataTransferStarted(I2CTarget_DT *self);

void I2CTarget_DT_ClearDataTransferStartedFlag(I2CTarget_DT *self);

void I2CTarget_DT_ReadFromDataTransferBuffer(I2CTarget_DT *self, bool *retIsReadType, 
    uint8_t **retPtrArray, uint16_t *retLength);

uint8_t I2CTarget_DT_GetDataTransferBufferCount(I2CTarget_DT *self);

bool I2CTarget_DT_IsDataTransferBufferFull(I2CTarget_DT *self);

bool I2CTarget_DT_IsDataTransferBufferNotEmpty(I2CTarget_DT *self);

uint8_t I2CTarget_DT_GetDataTransferBufferSize(I2CTarget_DT *self);

void I2CTarget_DT_ClearDataTransferBuffer(I2CTarget_DT *self);

I2CTargetState I2CTarget_DT_GetState(I2CTarget_DT *self);

#endif /* I2CTARGET_DT_H */
