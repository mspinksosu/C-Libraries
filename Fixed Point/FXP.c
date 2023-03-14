/***************************************************************************//**
 * @brief Fixed Point Library File
 * 
 * @author Matthew Spinks
 * 
 * @date 3/11/23   Original creation
 * 
 * @file FXP.c
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

#include "FXP.h"

// ***** Defines ***************************************************************

#define ROUND_FLOAT_CONVERSION  0

#ifdef ROUND_FLOAT_CONVERSION
#include <math.h>
#endif

// ***** Global Variables ******************************************************


// ***** Static Functions Prototypes *******************************************


// *****************************************************************************

FxpU16 FXP_ConvertToFixedU16(uint16_t integerPart, uint16_t fractionalPart, 
    uint8_t precision, uint8_t numFractionalBits)
{
    FxpU16 retFxp = {integerPart, 0};

    if(numFractionalBits < 16)
    {
        retFxp.numFracBits = numFractionalBits;
        retFxp.value <<= numFractionalBits;
        
        uint32_t f32 = fractionalPart << numFractionalBits;

        for(uint8_t i = 0; i < precision; i++)
        {
            f32 = f32 / 10;
        }
        retFxp.value += f32;
    }
    return retFxp;
}

uint16_t FXP_ConvertToU16(FxpU16 input)
{
    uint16_t result = input.value;
    /* Rounding is performed by adding 0.5 to a number. Imagine if a number 
    was in a 11.5 format, then the value 1 would be 2^5 or 1 << 5. The number
    0.5 is then 2^4 or 1 << 4. */
    if(input.numFracBits > 0)
    {
        /* Add 0.5 to round up */
        result += (1 << (input.numFracBits - 1));
        /* Shift back right to get the final result */
        result >>= input.numFracBits;
    }
    return result;

    /* TODO make routine for rounding negative numbers */
}

void FXP_ConvertFixedU16(FxpU16 *input, uint8_t numFractionalBits)
{
    if(numFractionalBits > 16)
        return;

    if(numFractionalBits > input->numFracBits)
    {
        input->value <<= (numFractionalBits - input->numFracBits);
    }
    else
    {
        input->value >>= (input->numFracBits - numFractionalBits);
    }
}

FxpU16 FXP_ConvertFloatToFixedU16(float input, uint8_t numFractionalBits)
{
    FxpU16 retFxp = {(uint16_t)input, 0};

    if(numFractionalBits < 16)
    {
        retFxp.numFracBits = numFractionalBits;
        #if ROUND_FLOAT_CONVERSION
            retFxp.value = (uint16_t)(round(input * (1 << numFractionalBits)));
        #else
            retFxp.value = (uint16_t)(input * (1 << numFractionalBits));
        #endif
    }
    return retFxp;
}

float FXP_ConvertFixedU16ToFloat(FxpU16 input)
{
    return ((float)input.value / (float)(1 << input.numFracBits));
}

FxpU16 FXP_AddFixedU16(FxpU16 a, FxpU16 b)
{
    FxpU16 retFxp;
    uint32_t result;
    uint8_t shift;

    /* The result should have the same number of decimal places as the operand
    with the least number of decimal places. Otherwise, our result may not 
    fit. We have to assume the operand with the least number of decimals is 
    sized correctly to fit it's largest expected value. First, I will make the
    one with less decimal places match the other, then perform rounding. */
    if(a.numFracBits < b.numFracBits)
    {
        retFxp.numFracBits = a.numFracBits;
        shift = b.numFracBits - a.numFracBits;
        result = (a.value << shift) + b.value;
    }
    else
    {
        retFxp.numFracBits = b.numFracBits;
        shift = a.numFracBits - b.numFracBits;
        result = (b.value << shift) + a.value;
    }

    if(shift > 0)
    {
        /* Add 0.5 to round up */
        result += (1 << (shift - 1));
    }

    /* Shift back right to get the final result */
    retFxp.value = (uint16_t)(result >> retFxp.numFracBits);
    return retFxp;
}

FxpU16 FXP_SubFixedU16(FxpU16 a, FxpU16 b)
{
    FxpU16 retFxp;
    uint32_t result;
    uint8_t shift;

    if(a.numFracBits < b.numFracBits)
    {
        retFxp.numFracBits = a.numFracBits;
        shift = b.numFracBits - a.numFracBits;
        result = (a.value << shift) - b.value;
    }
    else
    {
        retFxp.numFracBits = b.numFracBits;
        shift = a.numFracBits - b.numFracBits;
        result = (b.value << shift) - a.value;
    }

    if(shift > 0)
    {
        /* Add 0.5 to round up */
        result += (1 << (shift - 1));
    }

    /* Shift back right to get the final result */
    retFxp.value = (uint16_t)(result >> shift);
    return retFxp;
}

FxpU16 FXP_MulFixedU16(FxpU16 a, FxpU16 b)
{
    FxpU16 retFxp;
    uint32_t result;
    uint8_t shift;

    /* When we multiply two fixed point numbers, the result has the same
    number of decimals as the product of their fractional bits. So two 12.4
    numbers multiplied will yield a 12.8 result. Then we round back to the 
    original number of decimal places. */

}

/*
 End of File
 */