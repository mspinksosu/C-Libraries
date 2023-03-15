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

Fxp FXP_ConvertToFixedU16(uint16_t integerPart, uint16_t fractionalPart, 
    uint8_t precision, uint8_t numFractionalBits)
{
    Fxp retFxp = {.type = FXP_U16, .value = integerPart, .carry = false};

    /* interger * 2^b + (fractional * 2^b / 10^p)
    Where b is the number of fractional bits and p is the precision */

    if(numFractionalBits < 16)
    {
        retFxp.numFracBits = numFractionalBits;
        retFxp.value <<= numFractionalBits;
        
        uint32_t frac32 = fractionalPart;

        /* Maximum fractional part is 65535 which is 5 digits */
        if(precision > 5)
            precision = 5;

        /* Calculate the precision of our fractional part input */
        uint32_t tens = 1;
        uint8_t p = 1;
        for(; p < precision; p++)
        {
            tens *= 10;
            if(fractionalPart < tens)
                break;
        }

        /* Reduce fractional part if it is too big */
        for(uint8_t i = precision; i < p; i++)
        {
            frac32 += 5; // round
            frac32 /= 10;
        }

        /* Shift the fractional part up some before performing division. We are
        doing integer division, but we still need to do this step or we could
        lose even more precision. */
        frac32 <<= 16;

        for(uint8_t i = 0; i < precision; i++)
        {
            frac32 = frac32 / 10;
        }

        /* Shift the result back down and append it to the integer part */
        frac32 >>= (16 - numFractionalBits);
        retFxp.value += frac32;
    }
    return retFxp;
}

// *****************************************************************************

uint16_t FXP_ConvertToU16(Fxp input)
{
    uint32_t result = input.value;
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
    return (uint16_t)result;

    /* TODO make routine for rounding negative numbers eventually */
}

// *****************************************************************************

void FXP_ConvertFixedU16(Fxp *input, uint8_t numFractionalBits)
{
    if(numFractionalBits > 16) // TODO add type check for 32
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

// *****************************************************************************

Fxp FXP_ConvertFloatToFixedU16(float input, uint8_t numFractionalBits)
{
    Fxp retFxp = {.value = (uint16_t)input};

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

// *****************************************************************************

float FXP_ConvertFixedU16ToFloat(Fxp input)
{
    return ((float)input.value / (float)(1 << input.numFracBits));
}

// *****************************************************************************

Fxp FXP_AddFixedU16(Fxp a, Fxp b)
{
    Fxp retFxp = {.type = FXP_U16, .carry = false};
    uint32_t result;
    uint8_t shift;

    /* The result should have the same number of decimal places as the operand
    with the least number of decimal places. Otherwise, our result may not 
    fit. We have to assume the operand with the least number of decimal places
    is sized correctly to fit it's largest expected value. First I will 
    make the one with less match the other, then perform rounding. */

    // TODO check for 16 bit vs 32 bit
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

    if(retFxp.value & 0xFFFF0000)
        retFxp.carry = true;

    return retFxp;
}

// *****************************************************************************

Fxp FXP_SubFixedU16(Fxp a, Fxp b)
{
    Fxp retFxp = {.type = FXP_U16, .carry = false};
    uint32_t result;
    uint8_t shift;

    if(a.numFracBits < b.numFracBits)
    {
        retFxp.numFracBits = a.numFracBits;
        shift = b.numFracBits - a.numFracBits;
        a.value <<= shift;
        result = a.value - b.value;
        if(b.value > a.value)
            retFxp.carry = true;
    }
    else
    {
        retFxp.numFracBits = b.numFracBits;
        shift = a.numFracBits - b.numFracBits;
        b.value <<= shift;
        result = b.value - a.value;
        if(a.value > b.value)
            retFxp.carry = true;
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

// *****************************************************************************

Fxp FXP_MulFixedU16(Fxp a, Fxp b)
{
    Fxp retFxp = {.numFracBits = a.numFracBits};
    uint32_t result;
    uint8_t shift = b.numFracBits;

    /* Fixed point multiplication is actually simpler than addition. When we 
    multiply two fixed point numbers, the result has the same number of 
    decimals as the sum of their fractional bits. So two 12.4 numbers 
    multiplied will yield a 12.8 result. */
    uint8_t sumFrac = a.numFracBits + b.numFracBits;

    /* Whichever operand has less decimals is the limiting value to be used for
    the result. We know the result will have the sum of the two fractional bits.
    Then amount we have to shift back right is the sum minus our desired 
    fractional bits. Which is simply the other value's fractional bits. */
    if(b.numFracBits < a.numFracBits)
    {
        retFxp.numFracBits = b.numFracBits;
        shift = a.numFracBits;
    }

    result = a.value * b.value;

    if(sumFrac > 0)
    {
        /* Add 0.5 to round up */
        result += (1 << (sumFrac - 1));
    }

    retFxp.value = (uint16_t)(result >> shift);

    if(retFxp.value & 0xFFFF0000)
        retFxp.carry = true;

    return retFxp;
}

/*
 End of File
 */