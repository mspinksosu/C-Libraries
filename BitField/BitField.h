/***************************************************************************//**
 * @brief Flexible Bit Field Library Header File
 * 
 * @file BitField.h
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 5/14/22   Original creation
 * @date 10/30/22  Added variadic functions to modify list of bits
 * 
 * @details
 *      I like using bit fields a lot personally. In my opinion, I think
 * they're great. As long as you stick with accessing the bits by using the 
 * members of the struct and the proper dot notation, they work just fine. If 
 * you try to do something like a mem copy, send a bit field out a serial port,
 * or try to convert a whole field to a unsigned int, things don't always work 
 * out as intended. This is why I think they get a bad rap.
 * 
 * But maybe you do need to do some of those things. Maybe you're transmitting 
 * your bits to something else, and you want to always make sure your bits are 
 * always packed and your bytes are in the right order. Maybe you're just 
 * worried because someone told you bitfields are evil. This is a library that 
 * can help you.
 * 
 * Instead of storing everything in a single word, (or multiple words) I put 
 * the bits in an array. Now your bits can be packed into an array which can be
 * 1 to 32 bytes! To make a flexible bit field, all you need is an array of 
 * bytes and the size of the array. Yes it can be a little cumbersome sometimes
 * but it is portable.
 * 
 * @section example_code Example Code:
 *      A simple way to manage the size of your array automatically would be to 
 * define your bits in an enum, with the very last value being "TOTAL". Then 
 * declare your array like so:
 * 
 *      enum { INPUT_A = 0, INPUT_B, TOTAL };
 *      uint8_t inputMaskArray[TOTAL / 8 + 1];
 * 
 *      BifField_Init(&myBifField, &inputMaskArray, sizeof(inputMaskArray));
 *      BifField_SetBit(&myBitField, INPUT_A);
 *      if(BitField_GetBit(&myBitField, INPUT_A)) 
 *      {
 *          // do something 
 *      }
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

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Function Prototypes *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

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
void BitField_Init(BitField *self, uint8_t *ptrToArray, uint8_t sizeOfArray);

/***************************************************************************//**
 * @brief Set a bit
 * 
 * @param self  pointer to the BitField you are using
 * 
 * @param bitPos  the position of the bit in the mask. LSB = 0
 */
void BitField_SetBit(BitField *self, uint8_t bitPos);

/***************************************************************************//**
 * @brief Clear a bit
 * 
 * @param self  pointer to the BitField you are using
 * 
 * @param bitPos  the position of the bit in the mask. LSB = 0
 */
void BitField_ClearBit(BitField *self, uint8_t bitPos);

/***************************************************************************//**
 * @brief Invert a bit
 * 
 * @param self  pointer to the BitField you are using
 * 
 * @param bitPos  the position of the bit in the mask. LSB = 0
 */
void BitField_InvertBit(BitField *self, uint8_t bitPos);

/***************************************************************************//**
 * @brief Get a bit
 * 
 * @param self  pointer to the BitField you are using
 * 
 * @param bitPos  the position of the bit in the mask. LSB = 0
 * 
 * @return uint8_t  the value of the bit, either 0x01 or 0x00
 */
uint8_t BitField_GetBit(BitField *self, uint8_t bitPos);

/***************************************************************************//**
 * @brief Set a range of bits
 * 
 * The start and end of the range can be in whatever order you like. The 
 * function begins at the LSB of the literal and at the lower bit position of
 * the BitField. Only the bits in the range are affected. It is possible for 
 * the literal to be larger than the bit field and vice versa. Any other bits 
 * are ignored. The start and end bit numbers must be smaller than the size of 
 * the bitfield.
 * 
 * @param self  pointer to the BitField you are using
 * 
 * @param startBitPos  the bit number of the start of the range
 * 
 * @param endBitPos  the bit number of the end of the range
 * 
 * @param literal  the value that you want the bits set to
 */
void BitField_SetBitRangeEqualTo(BitField *self, uint8_t endBitPos, uint8_t startBitPos, uint32_t literal);

/***************************************************************************//**
 * @brief Get a range of bits
 * 
 * The start and end of the range can be in whatever order you like. The result
 * is right justified. If you ask for more than 32 bits, you will still get
 * 32 bits. The start and end bit numbers must be smaller than the size of the
 * bitfield.
 * 
 * @param self  pointer to the BitField you are using
 * 
 * @param startBitPos  the bit number of the start of the range
 * 
 * @param endBitPos  the bit number of the end of the range
 * 
 * @return uint32_t  result truncated to 32 bits
 */
uint32_t BitField_GetBitRange(BitField *self, uint8_t endBitPos, uint8_t startBitPos);

/***************************************************************************//**
 * @brief Set multiple bits
 * 
 * You must provide the number of bits you want to set followed by a comma
 * separated list of the bits. The bit numbers must be smaller than the size of
 * the bitfield.
 * 
 * @param self  pointer to the BitField you are using
 * 
 * @param numBitsToSet  how many bits you want to modify
 * 
 * @param ...  comma separated list bit positions. LSB = 0
 */
void BitField_SetBits(BitField *self, uint8_t numBitsToSet, ... );

/***************************************************************************//**
 * @brief Clear multiple bits
 * 
 * You must provide the number of bits you want to clear followed by a comma
 * separated list of the bits. The bit numbers must be smaller than the size of
 * the bitfield.
 * 
 * @param self  pointer to the BitField you are using
 * 
 * @param numBitsToSet  how many bits you want to modify
 * 
 * @param ...  comma separated list bit positions. LSB = 0
 */
void BitField_ClearBits(BitField *self, uint8_t numBitsToSet, ... );

/***************************************************************************//**
 * @brief Invert multiple bits
 * 
 * You must provide the number of bits you want to invert followed by a comma
 * separated list of the bits. The bit numbers must be smaller than the size of
 * the bitfield.
 * 
 * @param self  pointer to the BitField you are using
 * 
 * @param numBitsToSet  how many bits you want to modify
 * 
 * @param ...  comma separated list bit positions. LSB = 0
 */
void BitField_InvertBits(BitField *self, uint8_t numBitsToSet, ... );

/***************************************************************************//**
 * @brief Compare two BitFields
 * 
 * @param bf1  pointer to the first BitField
 * 
 * @param bf2  pointer to the second BitField
 * 
 * @return uint8_t  0 bf1 is equal to bf2
 */
uint8_t BitField_Compare(BitField *bf1, BitField *bf2);

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
void BitField_LogicalNot(BitField *bf1, BitField *result);

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
void BitField_LogicalAnd(BitField *bf1, BitField *bf2, BitField *result);

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
void BitField_LogicalOr(BitField *bf1, BitField *bf2, BitField *result);

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
void BitField_LogicalXor(BitField *bf1, BitField *bf2, BitField *result);

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
void BitField_LogicalXnor(BitField *bf1, BitField *bf2, BitField *result);

#endif /* BITFIELD_H */