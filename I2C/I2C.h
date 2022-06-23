/* *****************************************************************************
 * @Summary Basic I2C Interface Header
 * 
 * @author  Matthew Spinks
 * 
 * @File I2C.h 
 * 
 * @Description
 *      A very rough draft. For now, the only thing in here will be objects
 * that can be shared between multiple I2C peripherals. At least until I figure
 * out how I want to organize my library.
 * 
*******************************************************************************/

#ifndef I2C_H
#define	I2C_H

#include <stdint.h>
#include <stdbool.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

typedef struct I2CObject I2CObject;

// callback function pointer. The context is so that you can know which of
// your I2C devices initiated the callback.
typedef void (*I2CObjectCallbackFunc)(I2CObject *i2cObjectContext);

struct I2CObject
{
    uint8_t slaveAddress; // 7-bit address, right justified
    uint8_t *writeBuffer;
    uint8_t *readBuffer;
    
    uint8_t numBytesToSend;
    uint8_t numBytesToRead;
    
    // function pointers for call backs
    I2CObjectCallbackFunc transmitFinishedCallback;
    I2CObjectCallbackFunc receivedFinishedCallback;
    
    // Yo dawg. I heard you liked structs...
    struct
    {
        uint8_t writeCount;
        uint8_t readCount;
    } private;

    /** These variables should be treated as private and not modified directly
     * without the use of a function. Unfortunately, this is C, so I really 
     * can't stop you.*/
};

/* TODO: description of variables below
 * 
 * period   The period of the timer. When the count reaches this number, a flag
 *          will be set.
 * 
 */

// ***** Function Prototypes ***************************************************

// TODO Decide if I want to declare the size of each buffer to the object, or 
// just let the user handle it
void I2C_InitObject(I2CObject *self, uint8_t slaveAddress, uint8_t *writeBuffer, uint8_t *readBuffer);

/*
 End of File
 */

#endif	/* I2C_H */
