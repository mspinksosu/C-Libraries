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

// ***** Function Prototypes ***************************************************


// ***** Global Variables ******************************************************


/*******************************************************************************
 * @brief Initializes a Buffer object.
 * 
 * Sets up pointers to the buffer. Does not allow the buffer to overwrite
 * values by default.
 * 
 * @param self  pointer to the Buffer that you are using
 * 
 * @param arrayIn  pointer to the array that you are going to use
 * 
 * @param arrayInSize  the size of said array 
 */
void Buffer_Init(Buffer *self, uint8_t *arrayIn, uint8_t arrayInSize)
{
    Buffer_InitWithOverwrite(self, arrayIn, arrayInSize, false);
}

/*******************************************************************************
 * @brief Initializes a Buffer object with overwrite option.
 * 
 * Gives you a boolean that, when initialized as true, will allow the buffer 
 * to overwrite data once it is full.
 * 
 * If you are using this as a transmit buffer and you need to check for
 * space in the buffer, you should do it beforehand. I've used a while-loop to 
 * wait for space in the buffer before. It works well for microcontrollers that 
 * have interrupts which run automatically, but doesn't work well for everyone.
 * 
 * If you using it to transmit out of, you should probably not have the
 * overwrite boolean enabled.
 * 
 * @param self  pointer to the Buffer that you are using
 * 
 * @param arrayIn  pointer to the array that you are going to use
 * 
 * @param arrayInSize  the size of said array
 * 
 * @param overwrite  enable overwrite of buffer data if true
 */
void Buffer_InitWithOverwrite(Buffer *self, uint8_t *arrayIn, uint8_t arrayInSize, bool overwrite)
{
    self->private.buffer = arrayIn;
    self->private.size = arrayInSize;
    self->enableOverwrite = overwrite;
    self->count = 0;
}

/*******************************************************************************
 * @brief Put a byte into the buffer then update the head.
 * 
 * @param self  pointer to the Buffer that you are using
 * 
 * @param receivedByte  the byte to store in the buffer
 */
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

/*******************************************************************************
 * @brief Read a byte from the buffer then update the tail
 * 
 * Right now, I have it set to return zero if the buffer is empty.
 * It is your responsibility to check if the buffer has data beforehand.
 * I've provided the function Buffer_IsFull for you to use.
 * 
 * @param self  pointer to the Buffer that you are using
 * 
 * @return uint8_t  byte read from the buffer. 0 if empty
 */
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

/*******************************************************************************
 * @brief Read a byte from the buffer but don't update the tail
 * 
 * @param self  pointer to the Buffer that you are using
 * 
 * @return uint8_t  byte read from the buffer. 0 if empty
 */
uint8_t Buffer_Peek(Buffer *self)
{
    uint8_t dataToReturn = 0;
    
    if(self->private.head != self->private.tail)
    {
        dataToReturn =  self->private.buffer[self->private.tail];
    }
    return dataToReturn;
}

/***************************************************************************//**
 * @brief Clear the buffer
 * 
 * @param self  pointer to the Buffer that you are using
 */
void Buffer_Flush(Buffer *self)
{
    self->private.tail = self->private.head;
    self->count = 0;
}

/*******************************************************************************
 * @brief Get amount of data stored in the buffer
 * 
 * @param self  pointer to the Buffer that you are using
 *
 * @return uint8_t  number of bytes in the buffer
 */
uint8_t Buffer_GetCount(Buffer *self)
{
/*  For now, I'm using a simple counter. If I choose to limit by buffer
    to powers of two, then I can do away with the counter and get the size 
    based on the head and tail with modulo division. Example below
    return (uint8_t)(self->private.size + self->private.head - self->private.tail) & (self->private.size - 1); 
    */
    return self->count;
}

/*******************************************************************************
 * @brief Is the buffer full
 * 
 * @param self  pointer to the Buffer that you are using
 * 
 * @return true if buffer is full
 */
bool Buffer_IsFull(Buffer *self)
{
    uint8_t tempHead = CircularIncrement(self->private.head, self->private.size);
    
    if(tempHead == self->private.tail)
        return true;
    else
        return false;
}

/*******************************************************************************
 * @brief Is there something in the buffer
 * 
 * Useful for transmit buffers
 * 
 * @param self  pointer to the Buffer that you are using
 * 
 * @return true if buffer is not empty
 */
bool Buffer_IsNotEmpty(Buffer *self)
{
    if(self->count != 0)
        return true;
    else
        return false;
}

/*******************************************************************************
 * @brief Check if the buffer overflowed
 * 
 * The overflow flag is cleared when you call this function. It is also
 * cleared automatically when space appears in the buffer. 
 * 
 * @param self  pointer to the Buffer that you are using
 * 
 * @return true if buffer did overflow
 */
bool Buffer_DidOverflow(Buffer *self)
{
    // Automatically clear the flag
    bool temp = self->overflow;
    self->overflow = false;
    return temp;
}

/*******************************************************************************
 * @brief A function pointer that is called when the buffer overflows
 * 
 * Only works if you have overwrite disabled. If you set this function pointer, 
 * your function will automatically be called whenever the buffer tries to 
 * overwrite data. The overflow boolean is also set.
 * 
 * @param self  pointer to the Buffer that you are using
 * 
 * @param Function  format: void SomeFunction(void)
 */
void Buffer_SetOverflowCallback(Buffer *self, BufferOverflowCallbackFunc Function)
{
    self->private.bufferOverflowCallbackFunc = Function;
}

