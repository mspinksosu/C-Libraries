/***************************************************************************//**
 * @brief Basic Ring Buffer
 * 
 * @author Matthew Spinks
 * 
 * @date 4/1/19     Original creation
 *       10/5/21    Updated documention
 *       05/16/22   Fixed bug with tail not getting updated with circular inc
 * 
 * @file Buffer.c
 * 
 * @details
 *      A basic 8-bit ring buffer. To create a buffer, the minimum you will 
 * need is a Buffer object, an array pointer, and the size of the array.
 * 
 *      This library lets you control the size of your ring buffer as well as 
 * the way the buffer handles overflows. Multiple functions are provided to get 
 * the status of the buffer. For each function, you will pass the buffer that 
 * you wish to perform the operation on.
 * 
 *      There are two initializations: One has a boolean which will allow data 
 * to be overwritten when placing data in the buffer. The default setting is 
 * false. 
 * 
 *      There is a buffer overflow callback function. The function you create
 * for the callback must follow the prototype listed in Buffer.h. If overflow 
 * is about to happen and you have overwrite disabled, you will receive a 
 * callback and a boolean notification. If you don't clear the notification, it 
 * will be cleared for you when there is space in the buffer.
 * 
 ******************************************************************************/

#include "Buffer.h"

// ***** Defines ***************************************************************

/*  I'm going to use a simple check to go around the ring buffer. In the past, 
    I would use a logical AND type of modulo division. It worked really quickly, 
    but it restricted the buffer size to powers of two only. */
#define CircularIncrement(i, size) i == (size - 1) ? 0 : i + 1

// ***** Global Variables ******************************************************


// ***** Static Function Prototypes ********************************************


// *****************************************************************************

void Buffer_Init(Buffer *self, uint8_t *arrayIn, uint8_t arrayInSize)
{
    Buffer_InitWithOverwrite(self, arrayIn, arrayInSize, false);
}

// *****************************************************************************

void Buffer_InitWithOverwrite(Buffer *self, uint8_t *arrayIn, uint8_t arrayInSize, bool overwrite)
{
    self->private.buffer = arrayIn;
    self->private.size = arrayInSize;
    self->enableOverwrite = overwrite;
    self->count = 0;
}

// *****************************************************************************

void Buffer_WriteByte(Buffer *self, uint8_t receivedByte)
{
    uint8_t tempHead = CircularIncrement(self->private.head, self->private.size);
    
    if(tempHead != self->private.tail)
    {
        // There is space in the buffer
        self->private.buffer[self->private.head] = receivedByte;
        self->private.head = tempHead;
        self->count++;
    }
    else if(self->enableOverwrite)
    {
        // There is no space in the buffer and overwrite is enabled
        self->private.buffer[self->private.head] = receivedByte;
        self->private.head = tempHead; // Mark the next space to be overwritten
        self->private.tail = CircularIncrement(self->private.tail, self->private.size); // Move the tail up one
        self->overflow = true;
    }
    else
    {
        // There is no space in the buffer and overwrite is disabled
        if(self->overflow == false)
        {
            // We are about to overflow. Go ahead and store the last byte
            self->private.buffer[self->private.head] = receivedByte;
        }
        
        self->overflow = true; // Notify of overflow
        
        if(self->private.bufferOverflowCallbackFunc)
        {
            self->private.bufferOverflowCallbackFunc();
        }
    }
}

// *****************************************************************************

uint8_t Buffer_ReadByte(Buffer *self)
{
    uint8_t dataToReturn = 0;
    
    if(self->private.head != self->private.tail)
    {
        // The buffer is not empty
        dataToReturn =  self->private.buffer[self->private.tail];
        self->private.tail = CircularIncrement(self->private.tail, self->private.size);
        self->count--;
        self->overflow = false;
    }
    return dataToReturn;
}

// *****************************************************************************

uint8_t Buffer_Peek(Buffer *self)
{
    uint8_t dataToReturn = 0;
    
    if(self->private.head != self->private.tail)
    {
        dataToReturn =  self->private.buffer[self->private.tail];
    }
    return dataToReturn;
}

// *****************************************************************************

void Buffer_Flush(Buffer *self)
{
    self->private.tail = self->private.head;
    self->count = 0;
}

// *****************************************************************************

uint8_t Buffer_GetCount(Buffer *self)
{
/*  For now, I'm using a simple counter. If I choose to limit by buffer
    to powers of two, then I can do away with the counter and get the size 
    based on the head and tail with modulo division. Example below
    return (uint8_t)(self->private.size + self->private.head - self->private.tail) & (self->private.size - 1); 
    */
    return self->count;
}

// *****************************************************************************

bool Buffer_IsFull(Buffer *self)
{
    uint8_t tempHead = CircularIncrement(self->private.head, self->private.size);
    
    if(tempHead == self->private.tail)
        return true;
    else
        return false;
}

// *****************************************************************************

bool Buffer_IsNotEmpty(Buffer *self)
{
    if(self->count != 0)
        return true;
    else
        return false;
}

// *****************************************************************************

bool Buffer_DidOverflow(Buffer *self)
{
    // Automatically clear the flag
    bool temp = self->overflow;
    self->overflow = false;
    return temp;
}

// *****************************************************************************

void Buffer_SetOverflowCallback(Buffer *self, BufferOverflowCallbackFunc Function)
{
    self->private.bufferOverflowCallbackFunc = Function;
}

/*
 End of File
 */