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
 *      This is a simple library. It works best when your just dealing with 
 * setting, clearing, and inverting bits. That being said, I did go ahead and 
 * add the basic logic functions NOT, AND, OR, XOR, XNOR anyways just because
 * it might be necessary. It's not super fast, but if you're having to do 
 * a whole lot of work with masks and literals, maybe you should re-think what 
 * it is you're trying to accomplish. 
 * 
 ******************************************************************************/

#include "BitField.h"
#include <string.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************


// ***** Static Function Prototypes ********************************************


// *****************************************************************************

void BitField_Init(BitField *self, uint8_t *ptrToArray, uint8_t sizeOfArray)
{
    if(ptrToArray == NULL || sizeOfArray == 0)
        return;
       
    self->ptrToArray = ptrToArray;
    self->sizeOfArray = sizeOfArray;
}

// *****************************************************************************

void BitField_SetBit(BitField *self, uint8_t bitPos)
{
    if(bitPos >= (self->sizeOfArray) * 8)
        return;

    uint8_t i = bitPos / 8;
    uint8_t bit = bitPos % 8;

    self->ptrToArray[i] |= (1 << bit);
}

// *****************************************************************************

void BitField_ClearBit(BitField *self, uint8_t bitPos)
{
    if(bitPos >= (self->sizeOfArray) * 8)
        return;

    uint8_t i = bitPos / 8;
    uint8_t bit = bitPos % 8;

    self->ptrToArray[i] &= ~(1 << bit);
}

// *****************************************************************************

void BitField_InvertBit(BitField *self, uint8_t bitPos)
{
    if(bitPos >= (self->sizeOfArray) * 8)
        return;

    uint8_t i = bitPos / 8;
    uint8_t bit = bitPos % 8;

    self->ptrToArray[i] ^= (1 << bit);
}

// *****************************************************************************

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

// *****************************************************************************

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

// *****************************************************************************

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

// *****************************************************************************

uint8_t BitField_Compare(BitField *bf1, BitField *bf2)
{
    if(memcmp((uint8_t)(bf1->ptrToArray), (const uint8_t)(bf2->ptrToArray), bf1->sizeOfArray) == 0)
        return 0;
    else
        return 1;
}

// *****************************************************************************

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

// *****************************************************************************

void BitField_LogicalAnd(BitField *bf1, BitField *bf2, BitField *result)
{
    if((bf1->sizeOfArray != bf2->sizeOfArray) || (bf1->sizeOfArray != result->sizeOfArray))
        return;

    for(uint8_t i = 0; i < result->sizeOfArray; i++)
    {
        result->ptrToArray[i] = (bf1->ptrToArray[i]) & (bf2->ptrToArray[i]);
    }
}

// *****************************************************************************

void BitField_LogicalOr(BitField *bf1, BitField *bf2, BitField *result)
{
    if((bf1->sizeOfArray != bf2->sizeOfArray) || (bf1->sizeOfArray != result->sizeOfArray))
        return;

    for(uint8_t i = 0; i < result->sizeOfArray; i++)
    {
        result->ptrToArray[i] = (bf1->ptrToArray[i]) | (bf2->ptrToArray[i]);
    }
}

// *****************************************************************************

void BitField_LogicalXor(BitField *bf1, BitField *bf2, BitField *result)
{
    if((bf1->sizeOfArray != bf2->sizeOfArray) || (bf1->sizeOfArray != result->sizeOfArray))
        return;

    for(uint8_t i = 0; i < result->sizeOfArray; i++)
    {
        result->ptrToArray[i] = (bf1->ptrToArray[i]) ^ (bf2->ptrToArray[i]);
    }
}

// *****************************************************************************

void BitField_LogicalXnor(BitField *bf1, BitField *bf2, BitField *result)
{
    if((bf1->sizeOfArray != bf2->sizeOfArray) || (bf1->sizeOfArray != result->sizeOfArray))
        return;

    for(uint8_t i = 0; i < result->sizeOfArray; i++)
    {
        result->ptrToArray[i] = ~((bf1->ptrToArray[i]) ^ (bf2->ptrToArray[i]));
    }
}

/*
 End of File
 */