/***************************************************************************//**
 * @brief Flexible Bit Field Library Header File
 * 
 * @author Matthew Spinks
 * 
 * @date 5/14/22  Original creation
 * 
 * @file BitField.h
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
 * To make a flexible bit field, all you need is an array of bytes and the size 
 * of the array.
 * 
 ******************************************************************************/

#ifndef BITFIELD_H
#define BITFIELD_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

typedef struct BitFieldTag
{
    uint8_t *ptrToArray;
    uint8_t sizeOfArray;
} BitField;

// ***** Function Prototypes ***************************************************

void BitField_Init(BitField *self, uint8_t *ptrToArray, uint8_t sizeOfArray);

void BitField_SetBit(BitField *self, uint8_t bitPos);

void BitField_ClearBit(BitField *self, uint8_t bitPos);

void BitField_InvertBit(BitField *self, uint8_t bitPos);

uint8_t BitField_GetBit(BitField *self, uint8_t bitPos);

void BitField_LogicalNot(BitField *bf1, BitField *result);

void BitField_LogicalAnd(BitField *bf1, BitField *bf2, BitField *result);

void BitField_LogicalOr(BitField *bf1, BitField *bf2, BitField *result);

void BitField_LogicalXor(BitField *bf1, BitField *bf2, BitField *result);

void BitField_LogicalXnor(BitField *bf1, BitField *bf2, BitField *result);

void BitField_SetBitRangeEqualTo(BitField *self, uint8_t endBitPos, uint8_t startBitPos, uint32_t literal);

uint32_t BitField_GetBitRange(BitField *self, uint8_t endBitPos, uint8_t startBitPos);

#endif	/* BITFIELD_H */