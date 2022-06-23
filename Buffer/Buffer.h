/***************************************************************************//**
 * @brief Basic Ring Buffer Header
 * 
 * @author Matthew Spinks
 * 
 * @date 4/1/19     Original creation
 *       10/5/21    Updated documention
 *       2/21/22    Added doxygen
 * 
 * @file Buffer.h
 * 
 * @details
 *      A basic 8-bit ring buffer. To create a buffer, the minimum you will 
 * need is a Buffer object, an array pointer, and the size of the array.
 * 
 ******************************************************************************/

#ifndef BUFFER_H
#define	BUFFER_H

#include <stdint.h>
#include <stdbool.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

typedef struct Buffer Buffer;

/*  overflow callback function pointer. Make your function pointer follow this 
    format */
typedef void (*BufferOverflowCallbackFunc)(void);

/*  Buffer object. You shouldn't really need to access anything in here 
    directly. I've provided functions to do that for you. */
struct Buffer
{
    uint8_t count;
    bool overflow;
    bool enableOverwrite;
    
    struct
    { //  Yo dawg. I heard you liked structs...
        uint8_t *buffer;
        uint8_t size;
        uint8_t head;
        uint8_t tail;
        BufferOverflowCallbackFunc bufferOverflowCallbackFunc;
    } private;
};

/** 
 * The variables below should be treated as private. You should only access 
 * them with the use of a function.
 * 
 * buffer   pointer to the array which will form your ring buffer
 * 
 * size     the size of your array
 * 
 * head     keeps track of the current index of data being written into the
 *          buffer
 * 
 * tail     keeps track of the current index of data being read out from the
 *          buffer
 */

// ***** Function Prototypes ***************************************************

void Buffer_Init(Buffer *self, uint8_t *arrayIn, uint8_t arrayInSize);

void Buffer_InitWithOverwrite(Buffer *self, uint8_t *arrayIn, uint8_t arrayInSize, bool overwrite);

void Buffer_WriteByte(Buffer *self, uint8_t receivedByte);

uint8_t Buffer_ReadByte(Buffer *self);

uint8_t Buffer_Peek(Buffer *self);

void Buffer_Flush(Buffer *self);

uint8_t Buffer_GetCount(Buffer *self);

bool Buffer_IsFull(Buffer *self);

bool Buffer_IsNotEmpty(Buffer *self);

bool Buffer_DidOverflow(Buffer *self);

void Buffer_SetOverflowCallback(Buffer *self, BufferOverflowCallbackFunc Function);

#endif  /* BUFFER_H */

