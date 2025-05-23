/***************************************************************************//**
 * @brief Data Transfer Library
 * 
 * @file DataTransfer.c
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 5/1/25    Original creation
 * 
 * @details
 *      @todo details
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

#include "DataTransfer.h"

// ***** Defines ***************************************************************

#define CircularIncrement(i, size) i == (size - 1) ? 0 : i + 1

// ***** Global Variables ******************************************************


// ***** Static Functions Prototypes *******************************************


// *****************************************************************************

void DTBuffer_Init(DTBuffer *self, DataTransfer *arrayIn, uint8_t arrayInSize)
{
    self->private.buffer = arrayIn;
    self->private.size = arrayInSize;
    self->count = 0;
}

// *****************************************************************************

void DTBuffer_WriteDataTransfer(DTBuffer *self, DataTransferType writeOrRead, uint8_t *dataArray, uint16_t length)
{
    if(self->count < self->private.size)
    {
        // There is space in the buffer
        uint8_t tempHead = CircularIncrement(self->private.head, self->private.size);
        self->private.buffer[self->private.head].transferType = writeOrRead;
        self->private.buffer[self->private.head].ptrDataArray = dataArray;
        self->private.buffer[self->private.head].length = length;
        self->private.head = tempHead;
        self->count++;
    }
}

// *****************************************************************************

void DTBuffer_ReadDataTransfer(DTBuffer *self, DataTransfer *returnDataTransfer)
{
    if(self->count > 0)
    {
        /* The buffer is not empty. Get the data from the buffer to be 
        processed and clear the transfer finished flag */
        *returnDataTransfer = self->private.buffer[self->private.tail];
        self->private.tail = CircularIncrement(self->private.tail, self->private.size);
        self->count--;
    }
    else
    {
        /* The buffer is empty. Set the returned data length to 0 */
        returnDataTransfer->length = 0;
    }
}

// *****************************************************************************

uint8_t DTBuffer_GetCount(DTBuffer *self)
{
    return self->count;
}

// *****************************************************************************

bool DTBuffer_IsFull(DTBuffer *self)
{
    if(self->count >= self->private.size)
        return true;
    else
        return false;
}

// *****************************************************************************

bool DTBuffer_IsNotEmpty(DTBuffer *self)
{
    if(self->count != 0)
        return true;
    else
        return false;
}

// *****************************************************************************

void DTBuffer_Peek(DTBuffer *self, DataTransfer *returnDataTransfer)
{
    if(self->count > 0)
    {
        *returnDataTransfer = self->private.buffer[self->private.tail];
    }
    else
    {
        returnDataTransfer->length = 0;
    }
}

// *****************************************************************************

void DTBuffer_Flush(DTBuffer *self)
{
    self->private.tail = self->private.head;
    self->count = 0;
}

// *****************************************************************************

uint8_t DTBuffer_GetSize(DTBuffer *self)
{
    return self->private.size;
}

/*
 End of File
 */
