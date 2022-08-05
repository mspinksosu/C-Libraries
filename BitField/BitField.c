/***************************************************************************//**
 * @brief Flexible Bit Field Library
 * 
 * @author Matthew Spinks
 * 
 * @date 5/14/22  Original creation
 * 
 * @file BitField.c
 * 
 * @details
 *      I like using bit fields a lot personally. In my opinion, I think
 * they're great. As long as you stick with accessing the bits by using the 
 * members of the struct and the proper dot notation, they work just fine. If 
 * you try to do something like a mem copy, send a bit field out a serial
 * port, or use it in a different processor and try to convert a bit field to
 * a unsigned int, things don't always work out as intended. This is why I 
 * think they get a bad rap.
 * 
 * But maybe you do need to do some of those things. Maybe you're transmitting
 * your bits to something else, and you want to always make sure your bits are
 * packed and your bytes are in the right order. Maybe you're just scared 
 * because someone told you bitfields aren't portable. This is a library that 
 * can help you.
 * 
 * Instead of storing everything in a single word, (or multiple words) I put 
 * the bits in an array. Now your bits can be packed into an array from 
 * 1 to 32 bytes! Now your bits can be transmitted out a serial port easily.
 * This is a simple library. It works best when your just dealing with 
 * setting and clearing bits. If you have to do a bunch of masks with literals, 
 * maybe you should re-think what it is you're trying to accomplish.
 * 
 * To make a flexible bit field, all you need is an array of bytes and the size 
 * of the array.
 * 
 * A good way to manage the size of your array automatically would be to define
 * your bits in an enum, with the very last value being "TOTAL". Then declare
 * your array like so:
 * 
 * uint8_t inputMaskArray[TOTAL / 8 + 1];
 * 
 * Example Usage:
 *      BifField_Init(&myBifField, &inputMaskArray, sizeof(inputMaskArray));
 *      BifField_SetBit(&myBitField, INPUT_A);
 *      if(BitField_GetBit(&myBitField, INPUT_A)) { do something }
 * 
 ******************************************************************************/

#include "BitField.h"

// ***** Defines ***************************************************************


// ***** Static Function Prototypes ********************************************


// ***** Global Variables ******************************************************


/***************************************************************************//**
 * @brief Initialize a BitField
 * 
 * This uses an array to hold a bit mask. This makes it so we don't have to 
 * have a single variable to hold all of our bits. The functions will determine 
 * where the bits go in the array
 * 
 * @param self  pointer to the BitField you are using
 * 
 * @param ptrToArray  pointer to the array you are going to use
 * 
 * @param sizeOfArray  size of said array
 */
void BitField_Init(BitField *self, uint8_t *ptrToArray, uint8_t sizeOfArray)
{
    if(ptrToArray == NULL || sizeOfArray == 0)
        return;
       
    self->ptrToArray = ptrToArray;
    self->sizeOfArray = sizeOfArray;
}

/***************************************************************************//**
 * @brief Set a bit
 * 
 * @param self  pointer to the BitField you are using
 * 
 * @param bitPos  the position of the bit in the mask. LSB = 0
 */
void BitField_SetBit(BitField *self, uint8_t bitPos)
{
    if(bitPos >= (self->sizeOfArray) * 8)
        return;

    uint8_t i = bitPos / 8;
    uint8_t bit = bitPos % 8;

    self->ptrToArray[i] |= (1 << bit);
}

/***************************************************************************//**
 * @brief Clear a bit
 * 
 * @param self  pointer to the BitField you are using
 * 
 * @param bitPos  the position of the bit in the mask. LSB = 0
 */
void BitField_ClearBit(BitField *self, uint8_t bitPos)
{
    if(bitPos >= (self->sizeOfArray) * 8)
        return;

    uint8_t i = bitPos / 8;
    uint8_t bit = bitPos % 8;

    self->ptrToArray[i] &= ~(1 << bit);
}

/***************************************************************************//**
 * @brief Invert a bit
 * 
 * @param self  pointer to the BitField you are using
 * 
 * @param bitPos  the position of the bit in the mask. LSB = 0
 */
void BitField_InvertBit(BitField *self, uint8_t bitPos)
{
    if(bitPos >= (self->sizeOfArray) * 8)
        return;

    uint8_t i = bitPos / 8;
    uint8_t bit = bitPos % 8;

    self->ptrToArray[i] ^= (1 << bit);
}

/***************************************************************************//**
 * @brief Get a bit
 * 
 * @param self  pointer to the BitField you are using
 * 
 * @param bitPos  the position of the bit in the mask. LSB = 0
 * 
 * @return uint8_t  the value of the bit, either 0x01 or 0x00
 */
uint8_t BitField_GetBit(BitField *self, uint8_t bitPos)
{
    uint8_t result;

    if(bitPos < (self->sizeOfArray) * 8)
    {
        uint8_t i = bitPos / 8;
        uint8_t bit = bitPos % 8;
        result = self->ptrToArray[i] & (1 << bit);
    }
    return result;
}

/***************************************************************************//**
 * @brief  Invert a Bitfield and store the result
 * 
 * The operands must be the same size. The result can be stored in the same
 * Bitfield if desired.
 * 
 * @param bf1  the Bitfield that you want to invert
 * 
 * @param result  the Bitfield where the result will be placed
 */
void BitField_LogicalNot(BitField *bf1, BitField *result)
{
    if(bf1->sizeOfArray != result->sizeOfArray)
            return;

    for(uint8_t i = 0; i < result->sizeOfArray; i++)
    {
        for(uint8_t bit = 0; bit < 8; bit++)
        {
            result->ptrToArray[i] = ~(bf1->ptrToArray[i]);
        }
    }
}

/***************************************************************************//**
 * @brief Take the logical AND of two Bitfields and store the result
 * 
 * The operands must be the same size. The result can be stored in one of the 
 * same Bitfields if desired.
 * 
 * @param bf1  Bitfield operand one
 * 
 * @param bf2  Bitfield operand two
 * 
 * @param result  the Bitfield where the result will be placed
 */
void BitField_LogicalAnd(BitField *bf1, BitField *bf2, BitField *result)
{
    if((bf1->sizeOfArray != bf2->sizeOfArray) || (bf1->sizeOfArray != result->sizeOfArray))
        return;

    for(uint8_t i = 0; i < result->sizeOfArray; i++)
    {
        result->ptrToArray[i] = (bf1->ptrToArray[i]) & (bf2->ptrToArray[i]);
    }
}

/***************************************************************************//**
 * @brief Take the logical OR of two Bitfields and store the result
 * 
 * The operands must be the same size. The result can be stored in one of the 
 * same Bitfields if desired.
 * 
 * @param bf1  Bitfield operand one
 * 
 * @param bf2  Bitfield operand two
 * 
 * @param result  the Bitfield where the result will be placed
 */
void BitField_LogicalOr(BitField *bf1, BitField *bf2, BitField *result)
{
    if((bf1->sizeOfArray != bf2->sizeOfArray) || (bf1->sizeOfArray != result->sizeOfArray))
        return;

    for(uint8_t i = 0; i < result->sizeOfArray; i++)
    {
        result->ptrToArray[i] = (bf1->ptrToArray[i]) | (bf2->ptrToArray[i]);
    }
}

/***************************************************************************//**
 * @brief Take the logical XOR of two Bitfields and store the result
 * 
 * The operands must be the same size. The result can be stored in one of the 
 * same Bitfields if desired.
 * 
 * @param bf1  Bitfield operand one
 * 
 * @param bf2  Bitfield operand two
 * 
 * @param result  the Bitfield where the result will be placed
 */
void BitField_LogicalXor(BitField *bf1, BitField *bf2, BitField *result)
{
    if((bf1->sizeOfArray != bf2->sizeOfArray) || (bf1->sizeOfArray != result->sizeOfArray))
        return;

    for(uint8_t i = 0; i < result->sizeOfArray; i++)
    {
        result->ptrToArray[i] = (bf1->ptrToArray[i]) ^ (bf2->ptrToArray[i]);
    }
}

/***************************************************************************//**
 * @brief Take the logical XNOR of two Bitfields and store the result
 * 
 * The operands must be the same size. The result can be stored in one of the 
 * same Bitfields if desired.
 * 
 * @param bf1  Bitfield operand one
 * 
 * @param bf2  Bitfield operand two
 * 
 * @param result  the Bitfield where the result will be placed
 */
void BitField_LogicalXnor(BitField *bf1, BitField *bf2, BitField *result)
{
    if((bf1->sizeOfArray != bf2->sizeOfArray) || (bf1->sizeOfArray != result->sizeOfArray))
        return;

    for(uint8_t i = 0; i < result->sizeOfArray; i++)
    {
        result->ptrToArray[i] = ~((bf1->ptrToArray[i]) ^ (bf2->ptrToArray[i]));
    }
}

/***************************************************************************//**
 * @brief Set a range of bits
 * 
 * The start and end of the range can be in whatever order you like. The 
 * function begins at the LSB of the literal and at the lower bit position of
 * the BitField. Only the bits in the range are affected. It is possible for 
 * the literal to be larger than the bit field and vice versa. Any other bits 
 * are ignored.
 * 
 * @param self  pointer to the BitField you are using
 * 
 * @param startBitPos  the bit number of the start of the range
 * 
 * @param endBitPos  the bit number of the end of the range
 * 
 * @param literal  the value that you want the bits set to
 */
void BitField_SetBitRangeEqualTo(BitField *self, uint8_t endBitPos, uint8_t startBitPos, uint32_t literal)
{
    if((startBitPos >= (self->sizeOfArray) * 8) || (endBitPos >= (self->sizeOfArray) * 8))
        return;

    if(startBitPos > endBitPos)
    {
        uint8_t tmp = endBitPos;
        endBitPos = startBitPos;
        startBitPos = tmp;
    }
    
    uint8_t byte = startBitPos / 8;
    uint8_t endByte = endBitPos / 8;
    uint8_t bit = startBitPos % 8;
    uint8_t endBit = endBitPos % 8;
    uint8_t s = 0;

    while(byte <= endByte)
    {
        self->ptrToArray[byte] &= ~(1UL << bit);
        
        if(literal & (1UL << s))
        {
            self->ptrToArray[byte] |= (1UL << bit);
        }
        s++;
        bit++;

        if(byte == endByte && bit > endBit || s > 31)
        {
            break;
        }
        else if(bit > 7)
        {
            bit = 0;
            byte++;
        }
    }
    
}

/***************************************************************************//**
 * @brief Get a range of bits
 * 
 * The start and end of the range can be in whatever order you like. The result
 * is right justified. Any other bits are ignored.
 * 
 * @param self  pointer to the BitField you are using
 * 
 * @param startBitPos  the bit number of the start of the range
 * 
 * @param endBitPos  the bit number of the end of the range
 * 
 * @return uint32_t  result truncated to 32 bits
 */
uint32_t BitField_GetBitRange(BitField *self, uint8_t endBitPos, uint8_t startBitPos)
{
    if((startBitPos >= (self->sizeOfArray) * 8) || (endBitPos >= (self->sizeOfArray) * 8))
        return 0;

    if(startBitPos > endBitPos)
    {
        uint8_t tmp = endBitPos;
        endBitPos = startBitPos;
        startBitPos = tmp;
    }

    uint32_t result = 0;
    uint8_t byte = startBitPos / 8;
    uint8_t endByte = endBitPos / 8;
    uint8_t bit = startBitPos % 8;
    uint8_t endBit = endBitPos % 8;
    uint8_t d = 0;

    while(byte <= endByte)
    {
        if(self->ptrToArray[byte] & (1UL << bit))
        {
            result |= (1UL << d);
        }
        d++;
        bit++;

        if(byte == endByte && bit > endBit || d > 31)
        {
            break;
        }
        else if(bit > 7)
        {
            bit = 0;
            byte++;
        }
    }
    return result;
}

/*
 End of File
 */