/***************************************************************************//**
 * @brief Flexible Bit Field Library
 * 
 * @file BitField.c
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 5/14/22   Original creation
 * @date 10/30/22  Added variadic functions to modify list of bits
 * @date 6/4/23    Fixed bug in variadic functions
 * 
 * @details
 *      This is a simple library. It works best when your just dealing with 
 * setting, clearing, and inverting bits. That being said, I did go ahead and 
 * add the basic logic functions NOT, AND, OR, XOR, XNOR anyways just because
 * it might be necessary. It's not super fast, but if you're having to do 
 * a whole lot of work with masks and literals, maybe you should re-think what 
 * it is you're trying to accomplish. 
 * 
 * @section license License
 * SPDX-FileCopyrightText: © 2022 Matthew Spinks
 * SPDX-License-Identifier: Zlib
 * 
 * This software is released under the Zlib license. You are free alter and
 * redistribute it, but you must not misrepresent the origin of the software.
 * This notice may not be removed. <http://www.zlib.net/zlib_license.html>
 * 
 ******************************************************************************/

#include "BitField.h"
#include <stdarg.h>
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
    if(self->ptrToArray == NULL || bitPos >= (self->sizeOfArray) * 8)
        return;

    uint8_t i = bitPos / 8;
    uint8_t bit = bitPos % 8;

    self->ptrToArray[i] |= (1 << bit);
}

// *****************************************************************************

void BitField_ClearBit(BitField *self, uint8_t bitPos)
{
    if(self->ptrToArray == NULL || bitPos >= (self->sizeOfArray) * 8)
        return;

    uint8_t i = bitPos / 8;
    uint8_t bit = bitPos % 8;

    self->ptrToArray[i] &= ~(1 << bit);
}

// *****************************************************************************

void BitField_InvertBit(BitField *self, uint8_t bitPos)
{
    if(self->ptrToArray == NULL || bitPos >= (self->sizeOfArray) * 8)
        return;

    uint8_t i = bitPos / 8;
    uint8_t bit = bitPos % 8;

    self->ptrToArray[i] ^= (1 << bit);
}

// *****************************************************************************

uint8_t BitField_GetBit(BitField *self, uint8_t bitPos)
{
    uint8_t result = 0;

    if(bitPos < (self->sizeOfArray) * 8)
    {
        uint8_t i = bitPos / 8;
        uint8_t bit = bitPos % 8;
        if(self->ptrToArray[i] & (1 << bit))
            result = 1;
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
    uint8_t byte = startBitPos >> 3; // divide by 8
    uint8_t endByte = endBitPos >> 3;
    uint8_t bit = startBitPos & 0x07; // modulo 8
    uint8_t endBit = endBitPos & 0x07;
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

void BitField_SetBits(BitField *self, uint8_t numBitsToSet, ... )
{
    if(self->ptrToArray == NULL)
        return;
    /* va_arg will perform interger promotion on arguments. If we don't use int
    we will usually get a compiler warning. It will convert the int to your 
    type for you, however I think it's a good idea to check it rather than just 
    typecasting it without verifying it. */

    va_list list;
    int bitPos;
    uint8_t bit, i;
    va_start(list, numBitsToSet);

    while(numBitsToSet > 0)
    {
        bitPos = va_arg(list, int);
        if(bitPos < (self->sizeOfArray) * 8)
        {
            i = bitPos / 8;
            bit = bitPos % 8;
            self->ptrToArray[i] |= (1 << bit);
        }
        numBitsToSet--;
    }
    va_end(list);
}

// *****************************************************************************

void BitField_ClearBits(BitField *self, uint8_t numBitsToClear, ... )
{
    if(self->ptrToArray == NULL)
        return;

    va_list list;
    int bitPos;
    uint8_t bit, i;
    va_start(list, numBitsToClear);

    while(numBitsToClear > 0)
    {
        bitPos = va_arg(list, int);
        if(bitPos < (self->sizeOfArray) * 8)
        {
            i = bitPos / 8;
            bit = bitPos % 8;
            self->ptrToArray[i] &= ~(1 << bit);
        }
        numBitsToClear--;
    }
    va_end(list);
}

// *****************************************************************************

void BitField_InvertBits(BitField *self, uint8_t numBits, ... )
{
    if(self->ptrToArray == NULL)
        return;

    va_list list;
    int bitPos;
    uint8_t bit, i;
    va_start(list, numBits);

    while(numBits > 0)
    {
        bitPos = va_arg(list, int);
        if(bitPos < (self->sizeOfArray) * 8)
        {
            i = bitPos / 8;
            bit = bitPos % 8;
            self->ptrToArray[i] ^= (1 << bit);
        }
        numBits--;
    }
    va_end(list);
}

// *****************************************************************************

uint8_t BitField_Compare(BitField *bf1, BitField *bf2)
{
    if(memcmp((uint8_t*)(bf1->ptrToArray), (uint8_t*)(bf2->ptrToArray), bf1->sizeOfArray) == 0)
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