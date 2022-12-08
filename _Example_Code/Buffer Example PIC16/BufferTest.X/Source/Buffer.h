/*******************************************************************************
 * Title: Basic Ring Buffer Header
 * 
 * Author: Matthew Spinks
 * 
 * File: Buffer.h
 * 
 * Description:
 *      An interface to be implemented in other c files. This is for a basic
 *      8 bit ring buffer. To create a buffer, the minimum you will need is a 
 *      buffer type, an array pointer, and the size of the array.
 * 
 *      I did it this way to make the buffer easier to interface with. It
 *      lets you control the size of the array so that your not limited to
 *      a fixed size, or to a fixed number of arrays. Also, since I can't use 
 *      malloc with XC-8 anyways.
 * 
 *      For each function, you will pass the buffer that you wish to perform
 *      the operation on.
 * 
 *      There are two initializations: One has a boolean which will allow data 
 *      to be overwritten when placing data in the buffer. The default setting
 *      is false. 
 * 
 *      The buffer will check for overflow before overwriting any data. If 
 *      overflow is about to happen and you have overwrite disabled, you will 
 *      receive a callback function and boolean notification. If you don't 
 *      clear the notification it will be cleared for you when there is space
 *      in the buffer.
 * 
 * ****************************************************************************/

#ifndef BUFFER_H
#define	BUFFER_H

#include <stdint.h>
#include <stdbool.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

typedef struct Buffer Buffer;

/** Buffer Object. You shouldn't have to access anything in here directly.
 * I've provided functions to do that for you. */
struct Buffer
{
    // public
    uint8_t count;
    bool overflow;
    bool enableOverwrite;
    
    struct
    {
        //  Yo dawg. I heard you liked structs...
        uint8_t *buffer;
        uint8_t size;
        uint8_t head;
        uint8_t tail;
    } private; /** These should be treated as private variables and not modified
    directly without the use of a function. Unfortunately, this is C, so I 
    really can't stop you.*/
};

// ***** Function Prototypes ***************************************************

/**
 * Initializes a Buffer object
 * 
 * Sets up pointers to the buffer. Does not allow the buffer to overwrite
 * values by default
 * 
 * @param self  pointer to the Buffer type that you are going to use
 * 
 * @param arrayIn  pointer to the array that you are going to use
 * 
 * @param arrayInSize  the size of said array 
 * 
 * @return none
 */
void Buffer_Init(Buffer *self, uint8_t *arrayIn, uint8_t arrayInSize);

/**
 * Initializes a Buffer object with overwrite boolean
 * 
 * Gives you a boolean that, when initialized as true, will allow the buffer 
 * to overwrite data once it is full.
 * 
 * @param self  pointer to the Buffer type that you are going to use
 * 
 * @param arrayIn  pointer to the array that you are going to use
 * 
 * @param arrayInSize  the size of said array
 * 
 * @param overwrite  enable overwrite of buffer data if true
 * 
 * @return none
 */
void Buffer_InitWithOverwrite(Buffer *self, uint8_t *arrayIn, uint8_t arrayInSize, bool overwrite);

/**
 * Puts a char into the buffer. Updates the head.
 * 
 * @param self  pointer to the Buffer type that you are using
 * 
 * @param receivedChar  the char to store in the buffer
 * 
 * @return true if write was successful
 */
void Buffer_WriteChar(Buffer *self, uint8_t receivedChar);

/**
 * Reads a char from the buffer. Updates the tail.
 * 
 * @param self  pointer to the Buffer type that you are using
 * 
 * @return a char read from the buffer
 */
uint8_t Buffer_ReadChar(Buffer*);

/**
 * Gets the amount of data stored in the buffer
 * 
 * @param self  pointer to the Buffer type that you are using
 *
 * @return number of bytes in the buffer
 */
uint8_t Buffer_GetCount(Buffer*);

/**
 * A convenience function that tells you if the buffer is full.
 * 
 * @param self  pointer to the Buffer type that you are using
 * 
 * @return true if buffer is full
 */
bool Buffer_IsFull(Buffer*);

/**
 * A convenience function that tells you if there is something in the buffer
 * 
 * @param self  pointer to the Buffer type that you are using
 * 
 * @return true if buffer is not empty
 */
bool Buffer_IsNotEmpty(Buffer*);

/**
 * A convenience function that tells you if the buffer overflowed
 * 
 * The overflow flag is cleared when you call this function. It is also
 * cleared automatically when space appears in the buffer. 
 * 
 * @param self  pointer to the Buffer type that you are using
 * 
 * @return true if buffer did overflow
 */
bool Buffer_DidOverflow(Buffer*);

/**
 * A function pointer that is called when the buffer overflows.
 * 
 * Only works if you have overwrite disabled. If you set this function pointer, 
 * your function will automatically be called whenever the buffer tries to 
 * overwrite data. The overflow is also set.
 * 
 * @param self  pointer to the Buffer type that you are using
 * 
 * @param Function  format: void SomeFunction(void)
 */
void Buffer_SetOverflowCallback(void (*Function)(void));

#endif	/* BUFFER_H */

