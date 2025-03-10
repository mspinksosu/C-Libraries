/***************************************************************************//**
 * @brief Fixed Point Library File
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 3/11/23   Original creation
 * 
 * @file FXP.c
 * 
 * @details
 *      A simple library to do some fixed point math.
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
    uint8_t precisionOfFractionalPart, uint8_t numFractionalBits)
{
    Fxp retFxp = {.type = FXP_U16, .value = integerPart, .carry = false};

    // TODO check if integer part is too big

    /* fxp = integer * 2^b + (fractional * 2^b / 10^p)
    Where b is the number of fractional bits and p is the precision of the 
    fractional part */

    if(numFractionalBits < 16)
    {
        retFxp.numFracBits = numFractionalBits;
        retFxp.value <<= numFractionalBits;
        
        uint32_t frac32 = fractionalPart;

        /* Maximum fractional part is 65535 which is 5 digits */
        if(precisionOfFractionalPart > 5)
            precisionOfFractionalPart = 5;

        /* Calculate the precision of our fractional part input */
        uint32_t tens = 1;
        uint8_t p = 1;
        for(; p < precisionOfFractionalPart; p++)
        {
            tens *= 10;
            if(fractionalPart < tens)
                break;
        }

        /* Reduce fractional part if it is too big */
        for(uint8_t i = precisionOfFractionalPart; i < p; i++)
        {
            frac32 += 5; // round
            frac32 /= 10;
        }

        /* Shift the fractional part up some before performing division. We are
        doing integer division, but we still need to do this step or we could
        lose even more precision. */
        frac32 <<= 16;

        for(uint8_t i = 0; i < precisionOfFractionalPart; i++)
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

uint16_t FXP_GetInteger(Fxp input)
{
    return (uint16_t)(input.value >> input.numFracBits);
}

// *****************************************************************************

uint32_t FXP_GetMantissa(Fxp input)
{
    uint32_t dividend = 1000000000UL; // can return up to 9 digits
    uint8_t divisor = 1;
    uint32_t result = 0;
    uint8_t i = input.numFracBits;

    while(i > 0 && divisor < 6)
    {
        if(input.value & (1 << i))
        {
            result += (dividend >> divisor);
        }
        i--;
        divisor++;
    }
    return result;
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
    Fxp retFxp = {.type = FXP_U16, .value = (uint16_t)input, .carry = false};

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
    retFxp.value = (uint16_t)(result >> shift);

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
    Fxp retFxp = {.type = FXP_U16, .carry = false};
    uint32_t result;
    uint8_t shift;

    /* Limit input to 16 bits so that the result will fit in a 32-bit value */
    while(a.value & 0xFFFF0000)
    {
        a.value >>= 1;
        a.numFracBits--;
    }

    while(b.value & 0xFFFF0000)
    {
        b.value >>= 1;
        b.numFracBits--;
    }

    retFxp.numFracBits = a.numFracBits;
    shift = b.numFracBits;
    /* Fixed point multiplication is actually simpler than addition. When we 
    multiply two fixed point numbers, the result has the same number of 
    decimals places as the sum of their fractional bits. So two 12.4 numbers 
    multiplied will yield a 12.8 result. */
    uint8_t sumFrac = a.numFracBits + b.numFracBits;

    /* Whichever operand has less decimals is the limiting value to be used for
    the result. The result will have the sum of the two fractional bit values. 
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

// *****************************************************************************

Fxp FXP_DivFixedU16(Fxp dividend, Fxp divisor)
{
    Fxp retFxp = {.type = FXP_U16, .carry = false};
    uint32_t result;
    uint8_t i = 32, shiftLeft = 16, shiftRight;

    /* When we divide two fixed point numbers, the result will be the number
    of fractional bits of the dividend minus the divisor. So a 12.4 number 
    divided by a 14.2 number will have 2 fractional bits. First, I have
    to make the dividend much larger than the divisor, otherwise we lose
    precision. */
    if(dividend.value & 0xFFFF0000)
    {
        /* If the number is already greater than 16-bits, figure out how far 
        we can shift left. Otherwise, we just shift left 16 times. */
        shiftLeft = 0;
        while(i > 16)
        {
            if(dividend.value & (1 << i))
                break;
            i--;
            shiftLeft++;
        }
    }
    result = dividend.value << shiftLeft;
    shiftRight = shiftLeft + dividend.numFracBits - divisor.numFracBits;

    /* Whichever operand has less decimals is the limiting value to be used for
    the result. The result of the division will have the dividend plus the 
    amount shifted left, minus the divisor fractional bits. Then the amount to 
    shift back is this value minus the lesser of the two. */
    if(dividend.numFracBits < divisor.numFracBits)
    {
        retFxp.numFracBits = dividend.numFracBits;
    }
    else
    {
        retFxp.numFracBits = divisor.numFracBits;
    }
    shiftRight =- retFxp.numFracBits;
    result = result / divisor.value;

    retFxp.value = (uint16_t)(result >> shiftRight);
    return retFxp;
}

/*
 End of File
 */