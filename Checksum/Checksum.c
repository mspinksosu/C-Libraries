/***************************************************************************//**
 * @brief Checksum C File Template
 * 
 * @author Matthew Spinks
 * 
 * @date 2/25/23   Original creation
 * 
 * @file Checksum.c
 * 
 * @details
 *      I decided to make a simple file to hold some different checksum 
 * routines, since it's something that I use a lot.
 * 
 ******************************************************************************/

#include "Checksum.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************


// ***** Static Functions Prototypes *******************************************


// *****************************************************************************

uint8_t Checksum_TwosComp8Bit(const uint8_t *array, uint16_t length)
{
    uint8_t checksum = 0;
    
    for(uint16_t i = 0; i < length; i++)
    {
        checksum += array[i];
    }
    
    return (uint8_t)(checksum * -1);
}

// *****************************************************************************

uint16_t Checksum_TwosComp16Bit(const uint8_t *array, uint16_t length)
{
    uint16_t checksum = 0;
    
    for(uint16_t i = 0; i < length; i++)
    {
        checksum += array[i];
    }
    
    /* TODO Test this on a couple different compilers to see if they bitch 
    about potential overflow */
    return (uint16_t)(checksum * -1);
}

// *****************************************************************************

uint8_t Checksum_OnesComp8Bit(const uint8_t *array, uint16_t length)
{
    /* I used to think a one's complement checksum was just the sum and then
    the one's complement. It's actually the one's complement sum and then you
    take the one's complement of that. The difference is any carry bits
    resulting from addition are not discarded like with a two's comp checksum.
    They are carried back around. This makes it stronger than a two's comp
    checksum. Bit shift carries are easy to do in assembly, but not so much in
    C. The easiest way is to take the upper half of your sum which contains all 
    the carries, then add it back into the bottom half. Then take the one's 
    comp of (invert) that result. When you do this operation again with the
    checksum value included, the result will be zero. */
    uint16_t checksum = 0;
    
    for(uint16_t i = 0; i < length; i++)
    {
        checksum += array[i];
    }
    
    /* Add the carry bits back into the result. Then invert the result */
    checksum = (checksum & 0x00FF) + (checksum >> 8);
    return (uint8_t)(~checksum);
}

// *****************************************************************************

uint16_t Checksum_OnesComp16Bit(const uint8_t *array, uint16_t length)
{
    uint32_t checksum = 0;
    
    for(uint16_t i = 0; i < length; i++)
    {
        checksum += array[i];
    }
    
    /* Add the carry bits back into the result. Then invert the result */
    checksum = (checksum & 0x0000FFFF) + (checksum >> 16);
    return (uint16_t)(~checksum);
}

/*
 End of File
 */