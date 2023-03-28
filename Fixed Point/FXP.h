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

Fxp FXP_ConvertToFixedU16(uint16_t integerPart, uint16_t fractionalPart, 
    uint8_t precision, uint8_t numFractionalBits);

uint16_t FXP_ConvertToU16(Fxp input); // perform rounding

uint16_t FXP_GetInteger(Fxp input);

uint32_t FXP_GetMantissa(Fxp input);

void FXP_ConvertFixedU16(Fxp *input, uint8_t numFractionalBits); // This can probably be made a static function

Fxp FXP_ConvertFloatToFixedU16(float input, uint8_t numFractionalBits);

float FXP_ConvertFixedU16ToFloat(Fxp input);

Fxp FXP_AddFixedU16(Fxp a, Fxp b);

Fxp FXP_SubFixedU16(Fxp a, Fxp b);

Fxp FXP_MulFixedU16(Fxp a, Fxp b);

Fxp FXP_DivFixedU16(Fxp value, Fxp divisor);

#endif  /* FXP_H */