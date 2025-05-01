/***************************************************************************//**
 * @brief Data Transfer Header File
 * 
 * @file TargetDevice.h
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 5/1/25   Original creation
 * 
 * @details
 *      @todo a generic data transfer object. I originally made this for I2C 
 * slave devices, but then I realized it could also apply to SPI or any serial 
 * communication where a separate read and write are performed. For now, I will 
 * just make this a generic object. I might expand this be a generic slave 
 * (or target device) and change the file to TargetDevice.h. - MS
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
    DATA_TRANSFER_TYPE_WRITE = 0,
    DATA_TRANSFER_TYPE_READ
} DataTransferType;

typedef struct DataTransferTag
{
    DataTransferType transferType;
    uint8_t *dataArray;
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

/* @todo Do I want to add functions similar to my buffer library to handle 
managing an array of data transfer objects? */

void DTBuffer_BufferInit(DTBuffer *self, DataTransfer *arrayIn, uint8_t arrayInSize);

void DTBuffer_InitWithOverwrite(DTBuffer *self, DataTransfer *arrayIn, uint8_t arrayInSize, bool overwrite);

uint8_t DTBuffer_GetBufferCount(DTBuffer *self);

bool DTBuffer_IsBufferFull(DTBuffer *self);

bool DTBuffer_IsBufferNotEmpty(DTBuffer *self);

uint8_t DTBuffer_GetBufferSize(DTBuffer *self);

void DTBuffer_WriteToBuffer(DTBuffer *self, DataTransferType writeOrRead, uint8_t *data, uint16_t length);

uint8_t DTBuffer_ReadFromBuffer(DTBuffer *self, DataTransfer *returnDataTransfer);

/* @follow-up I could change the name to TargetDevice and make the I2C temperature 
sensor and I2C accelerometer a subclass of that.
TargetDevice_WriteToBuffer(TargetDevice *self, DataTransferType writeOrRead, ... ) - MS */

#endif /* DATA_TRANSFER_H */
