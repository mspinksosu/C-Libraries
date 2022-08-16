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

#ifndef BUFFER_H
#define	BUFFER_H

#include <stdint.h>
#include <stdbool.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

typedef struct Buffer Buffer;

/* overflow callback function. Make your function pointer follow this format */
typedef void (*BufferOverflowCallbackFunc)(void);

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
 * count  the current count of many bytes are in the buffer
 * 
 * overflow  true if buffer has overflowed
 * 
 * enableOverwrite  if true, the buffer will overwrite if it is full
 * 
 * buffer  pointer to the array which will form your ring buffer
 * 
 * size  the size of your array
 * 
 * head  keeps track of the current index of data being written into the buffer
 * 
 * tail  keeps track of the current index of data being read from the buffer
 */

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Function Prototypes *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

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
void Buffer_Init(Buffer *self, uint8_t *arrayIn, uint8_t arrayInSize);

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
void Buffer_InitWithOverwrite(Buffer *self, uint8_t *arrayIn, uint8_t arrayInSize, bool overwrite);

/*******************************************************************************
 * @brief Put a byte into the buffer then update the head.
 * 
 * @param self  pointer to the Buffer that you are using
 * 
 * @param receivedByte  the byte to store in the buffer
 */
void Buffer_WriteByte(Buffer *self, uint8_t receivedByte);

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
uint8_t Buffer_ReadByte(Buffer *self);

/*******************************************************************************
 * @brief Read a byte from the buffer but don't update the tail
 * 
 * @param self  pointer to the Buffer that you are using
 * 
 * @return uint8_t  byte read from the buffer. 0 if empty
 */
uint8_t Buffer_Peek(Buffer *self);

/***************************************************************************//**
 * @brief Clear the buffer
 * 
 * @param self  pointer to the Buffer that you are using
 */
void Buffer_Flush(Buffer *self);

/*******************************************************************************
 * @brief Get amount of data stored in the buffer
 * 
 * @param self  pointer to the Buffer that you are using
 *
 * @return uint8_t  number of bytes in the buffer
 */
uint8_t Buffer_GetCount(Buffer *self);

/*******************************************************************************
 * @brief Is the buffer full
 * 
 * @param self  pointer to the Buffer that you are using
 * 
 * @return true if buffer is full
 */
bool Buffer_IsFull(Buffer *self);

/*******************************************************************************
 * @brief Is there something in the buffer
 * 
 * Useful for transmit buffers
 * 
 * @param self  pointer to the Buffer that you are using
 * 
 * @return true if buffer is not empty
 */
bool Buffer_IsNotEmpty(Buffer *self);

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
bool Buffer_DidOverflow(Buffer *self);

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
void Buffer_SetOverflowCallback(Buffer *self, BufferOverflowCallbackFunc Function);

#endif  /* BUFFER_H */

