/***************************************************************************//**
 * @brief Fixed Point Library Header File
 * 
 * @author Matthew Spinks
 * 
 * @date 3/11/23   Original creation
 * 
 * @file FXP.h
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
 ******************************************************************************/

#ifndef FXP_H
#define FXP_H

#include <stdint.h>
#include <stdbool.h>
#include <float.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/* Class specific variables */
typedef struct FxpU32Tag
{
    uint32_t value;
    uint8_t numFracBits;
} FxpU32;

typedef struct FxpU16Tag
{
    uint16_t value;
    uint8_t numFracBits;
} FxpU16;

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

FxpU16 FXP_ConvertToFixedU16(uint16_t input, uint8_t numFractionalBits);

uint16_t FXP_ConvertToU16(FxpU16 input); // perform rounding

void FXP_ConvertFixedU16(FxpU16 *input, uint8_t numFractionalBits); // This can probably be made a static function

FxpU16 FXP_ConvertFloatToFixedU16(float input, uint8_t numFractionalBits);

float FXP_ConvertFixedU16ToFloat(FxpU16 input);

FxpU16 FXP_AddFixedU16(FxpU16 a, FxpU16 b);

FxpU16 FXP_SubFixedU16(FxpU16 a, FxpU16 b);

FxpU16 FXP_MulFixedU16(FxpU16 a, FxpU16 b);

#endif  /* FXP_H */