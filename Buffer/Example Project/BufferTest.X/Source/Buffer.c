/*******************************************************************************
 * Title: Basic Ring Buffer
 * 
 * Author: Matthew Spinks
 * 
 * File: Buffer.c
 * 
 * Description:
 *      A basic 8-bit ring buffer. To create a buffer, the minimum you will 
 *      need is a buffer type, an array pointer, and the size of the array.
 * 
 * ****************************************************************************/

#include "Buffer.h"

// ***** Defines ***************************************************************

// I'm going to try to use a simple check to go around the ring buffer.
// In the past, I would just use a logical AND type of modulo division.
// It worked really fast, but it is restricted to powers of two.
#define CircularIncrement(i, size) i == (size - 1) ? 0 : i + 1

// ***** Function Prototypes ***************************************************


// ***** Global Variables ******************************************************

// local function pointer
void (*Buffer_OverflowCallback)(void);

// ----- Initialize ------------------------------------------------------------

void Buffer_Init(Buffer *self, uint8_t *arrayIn, uint8_t arrayInSize)
{
    Buffer_InitWithOverwrite(self, arrayIn, arrayInSize, false);
}

void Buffer_InitWithOverwrite(Buffer *self, uint8_t *arrayIn, uint8_t arrayInSize, bool overwrite)
{
    self->private.buffer = arrayIn;
    self->private.size = arrayInSize;
    self->enableOverwrite = overwrite;
    self->count = 0;
}

// ----- Buffer Write ----------------------------------------------------------

// If you are using this as a transmit buffer and you need to check for 
// space in the buffer, you should do it beforehand. I've used a while-loop to 
// wait for space in the buffer before. It works well for microcontrollers that 
// have interrupts which run automatically, but doesn't work well for everyone.

// Obviously, if you using it to transmit out of, you should not have the
// overwrite boolean enabled.

void Buffer_WriteChar(Buffer *self, uint8_t receivedChar)
{
    uint8_t tempHead = CircularIncrement(self->private.head, self->private.size);
    
    if(tempHead != self->private.tail)
    {
        // There is space in the buffer
        self->private.buffer[self->private.head] = receivedChar;
        self->private.head = tempHead;
        self->count++;
    }
    else if(self->enableOverwrite)
    {
        // There is no space in the buffer and overwrite is enabled
        self->private.buffer[self->private.head] = receivedChar;
        self->private.head = tempHead; // Mark the next space to be overwritten
        CircularIncrement(self->private.tail, self->private.size); // Move the tail up one
        self->overflow = true;
    }
    else
    {
        // There is no space in the buffer and overwrite is disabled
        if(self->overflow == false)
        {
            // We are about to overflow. Go ahead and store the last char
            self->private.buffer[self->private.head] = receivedChar;
        }
        
        self->overflow = true; // Notify of overflow
        
        if(Buffer_OverflowCallback)
        {
            Buffer_OverflowCallback;
        }
    }
}

// ----- Buffer Read -----------------------------------------------------------

// Right now, I have it set to return zero if the buffer is empty.
// It is your responsibility to check if the buffer has data beforehand.
// I've provided the function Buffer_IsFull for you to use.
uint8_t Buffer_ReadChar(Buffer *self)
{
    uint8_t dataToReturn = 0;
    
    if(self->private.head != self->private.tail)
    {
        // The buffer is not empty
        dataToReturn =  self->private.buffer[self->private.tail];
        CircularIncrement(self->private.tail, self->private.size);
        self->count--;
        self->overflow = false;
    }
    return dataToReturn;
}

// ----- Buffer Count ----------------------------------------------------------

// Number of bytes currently in the buffer.
uint8_t Buffer_GetCount(Buffer *self)
{
    // For now, I'm using a simple counter. If I choose to limit by buffer
    // to powers of two, then I can do away with the counter and get the size 
    // based on the head and tail with modulo division.
    //return (uint8_t)(self->private.size + self->private.head - self->private.tail) & (self->private.size - 1);
    return self->count;
}

// -----------------------------------------------------------------------------

// A convenience function. Whether or not you use it is entirely up to you.
bool Buffer_IsFull(Buffer *self)
{
    uint8_t tempHead = CircularIncrement(self->private.head, self->private.size);
    
    if(tempHead == self->private.tail)
        return true;
    else
        return false;
}

// -----------------------------------------------------------------------------

// Another convenience function.
bool Buffer_IsNotEmpty(Buffer *self)
{
    if(self->count != 0)
        return true;
    else
        return false;
}

// ----- Get Overflow ----------------------------------------------------------

bool Buffer_DidOverflow(Buffer *self)
{
    // Automatically clear the flag
    bool temp = self->overflow;
    self->overflow = false;
    return temp;
}

// ----- Set Overflow Callback -------------------------------------------------

void Buffer_SetOverflowCallback(void (*Function)(void))
{
    Buffer_OverflowCallback = Function;
}

