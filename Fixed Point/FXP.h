/***************************************************************************//**
 * @brief Fixed Point Library Header File
 * 
 * @file FXP.h
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 3/11/23   Original creation
 * 
 * @details
 *      This is not meant to be as fast or as efficient as possible. It's meant
 * to be a simple library to help save a couple instruction cycles by avoiding
 * performing large amounts of floating point calculations. It also serves as
 * a nice educational tool for myself to learn how fixed point arithmetic works
 * in general.
 * 
 * The number of fractional bits defines the format of the number. For example,
 * a 16-bit number with 5 fractional bits is a 11.5 format. A 32-bit number
 * with 10 fractional bits is a 22.10 format and so on.
 * 
 * // TODO more notes. Add basic formula for conversion
 * 
 * @section license License
 * SPDX-FileCopyrightText: © 2023 Matthew Spinks
 * SPDX-License-Identifier: Zlib
 * 
 * This software is released under the Zlib license. You are free alter and
 * redistribute it, but you must not misrepresent the origin of the software.
 * This notice may not be removed. <http://www.zlib.net/zlib_license.html>
 * 
 ******************************************************************************/

#ifndef FXP_H
#define FXP_H

#include <stdint.h>
#include <stdbool.h>
#include <float.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

typedef enum FxpTypeTag
{
    FXP_U16,
    FXP_U32,
    // TODO add signed
} FxpType;

/* Class specific variables */
typedef struct FxpTag
{
    FxpType type;
    uint32_t value;
    uint8_t numFracBits;
    bool carry; // TODO
} Fxp;

/** 
 * Description of struct
 * 
 * member1      description of variable member1
 * 
 */

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Function Prototypes *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

// TODO finish doxygen

/***************************************************************************//**
 * @brief 
 * 
 * // TODO notes. Can input up to 5 digits of precision for the fractional part
 * 
 * Examples:
 * 12345.6 in 11.5 format: 12345, 6, 1, 5
 * 1234.56 in 10.6 format: 1234, 56, 2, 6
 * 1000.2 in 12.4 format: 1000, 2, 1, 4
 * 1000.20 in 12.4 format: 1000, 20, 2, 4
 * 100.5 in 10.6 format: 100, 5, 1, 6
 * 100.05 in 10.6 format: 100, 5, 2, 6
 * 10.005 in 6.10 format: 10, 5, 3, 10
 * 
 * @param integerPart 
 * @param fractionalPart 
 * @param precisionOfFractionalPart 
 * @param numFractionalBits 
 * @return Fxp 
 */
Fxp FXP_ConvertToFixedU16(uint16_t integerPart, uint16_t fractionalPart, 
    uint8_t precisionOfFractionalPart, uint8_t numFractionalBits);

/***************************************************************************//**
 * @brief 
 * 
 * // TODO notes about rounding
 * 
 * @param input 
 * @return uint16_t 
 */
uint16_t FXP_ConvertToU16(Fxp input);

/***************************************************************************//**
 * @brief 
 * 
 * @param input 
 * @return uint16_t 
 */
uint16_t FXP_GetInteger(Fxp input);

/***************************************************************************//**
 * @brief 
 * 
 * // TODO notes. Can return up to 9 digits
 * 
 * @param input 
 * @return uint32_t 
 */
uint32_t FXP_GetMantissa(Fxp input);

/***************************************************************************//**
 * @brief 
 * 
 * @param input 
 * @param numFractionalBits 
 */
void FXP_ConvertFixedU16(Fxp *input, uint8_t numFractionalBits); // This can probably be made a static function

/***************************************************************************//**
 * @brief 
 * 
 * @param input 
 * @param numFractionalBits 
 * @return Fxp 
 */
Fxp FXP_ConvertFloatToFixedU16(float input, uint8_t numFractionalBits);

/***************************************************************************//**
 * @brief 
 * 
 * @param input 
 * @return float 
 */
float FXP_ConvertFixedU16ToFloat(Fxp input);

/***************************************************************************//**
 * @brief 
 * 
 * @param a 
 * @param b 
 * @return Fxp 
 */
Fxp FXP_AddFixedU16(Fxp a, Fxp b);

/***************************************************************************//**
 * @brief 
 * 
 * @param a 
 * @param b 
 * @return Fxp 
 */
Fxp FXP_SubFixedU16(Fxp a, Fxp b);

/***************************************************************************//**
 * @brief 
 * 
 * @param a 
 * @param b 
 * @return Fxp 
 */
Fxp FXP_MulFixedU16(Fxp a, Fxp b);

/***************************************************************************//**
 * @brief 
 * 
 * @param value 
 * @param divisor 
 * @return Fxp 
 */
Fxp FXP_DivFixedU16(Fxp value, Fxp divisor);

#endif  /* FXP_H */