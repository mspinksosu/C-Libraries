/***************************************************************************//**
 * @brief Data Transfer Header File
 * 
 * @file TargetDevice.h
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 5/1/25    Original creation
 * 
 * @details
 *      @todo a generic data transfer object. I originally made this for I2C 
 * target devices, but then I realized it could also apply to SPI or any serial 
 * communication where a separate read and write are performed. For now, I will 
 * just make this a generic object. I might expand this be a generic target 
 * device and move my other I2CTarget code here. - MS
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

#ifndef DATA_TRANSFER_H
#define DATA_TRANSFER_H

#include <stdint.h>
#include <stdbool.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

typedef enum DataTransferTypeTag
{
    DATATRANSFER_TYPE_WRITE = 0,
    DATATRANSFER_TYPE_READ
} DataTransferType;

typedef struct DataTransferTag
{
    DataTransferType transferType;
    uint8_t *ptrDataArray;
    uint16_t length;
} DataTransfer;

typedef struct DTBufferTag
{
    uint8_t count;

    struct
    {
        DataTransfer *buffer;
        uint8_t size;
        uint8_t head;
        uint8_t tail;
    } private;
} DTBuffer;

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Function Prototypes *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void DTBuffer_Init(DTBuffer *self, DataTransfer *arrayIn, uint8_t arrayInSize);

void DTBuffer_WriteDataTransfer(DTBuffer *self, DataTransferType writeOrRead, uint8_t *dataArray, uint16_t length);

// Set return length to 0 if no data available
void DTBuffer_ReadDataTransfer(DTBuffer *self, DataTransfer *returnDataTransfer);

uint8_t DTBuffer_GetCount(DTBuffer *self);

bool DTBuffer_IsFull(DTBuffer *self);

bool DTBuffer_IsNotEmpty(DTBuffer *self);

void DTBuffer_Peek(DTBuffer *self, DataTransfer *returnDataTransfer);

void DTBuffer_Flush(DTBuffer *self);

uint8_t DTBuffer_GetSize(DTBuffer *self);

/* @follow-up I could change the name to TargetDevice and make the I2C temperature 
sensor and I2C accelerometer use that instead of restricting them to just I2C.
TargetDevice_WriteToBuffer(TargetDevice *self, DataTransferType writeOrRead, ... ) - MS */

#endif /* DATA_TRANSFER_H */
