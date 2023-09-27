/***************************************************************************//**
 * @brief PRNG Library Header File
 * 
 * @file PRNG.h
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 9/23/23   Original creation
 * 
 * @details
 *      I am currently testing three different types of PRNG. The LCG is an 
 * extension of a basic linear congruential generator, except that this one 
 * makes use of 64-bit math to get a longer output. The basic formula for an 
 * LCG is: "X_n+1 = (a * X_n + c) % m". LCG's have an advantage of being very 
 * fast if m is chosen to me a power of two because the modulus can be reduced 
 * to a logical AND. The downside is that only the upper bits of the LCG are 
 * psuedo random. The typical rand function uses this sort of LCG. The state 
 * is a 32-bit variable, but only 15 bits are used. (bits 30 through 16).
 * My version just expands upon this to get a bigger number. Instead of using 
 * a 32-bit variable, it uses 64-bit variables to store the state and returns 
 * the upper 32 bits. When the parameters a, m, and c are chosen correctly, the 
 * period of the LCG is m.
 * 
 * The Park Miller is special type of LCG that has removes the increment value 
 * c and has more restrictions in the choice of the multiplier a and modulus m. 
 * The value m is chosen to be a prime number close to range of the desired 
 * output. This is why you see the number 2^31-1 pop up lot. It is a Mersenne 
 * prime number that is close to the word size of 32-bits. There are also other 
 * restrictions placed on the relationship between a and m. The Park Miller is 
 * also sometimes referred to as a "Multiplicative Linear Congruential 
 * Generator". Its output is much better than the basic LCG but it is slower  // TODO notes about PM value of 0 and X_0
 * because it uses modulus of a prime number. When a and m are chosen correctly, 
 * the period of the Park Miller is m - 1. In fact, the Park Miller LCG is 
 * a "full-cycle" LCG. Meaning it will produce every number from 1 to m-1 
 * once before the sequence repeats.
 * 
 * // TODO more notes about Park Miller and Schrage
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

#ifndef PRNG_H
#define PRNG_H

#include <stdint.h>
#include <stdbool.h>

// ***** Defines ***************************************************************

/* Option for the Park Miller. True = multiplier a and modulo m will both be 
64-bit numbers. False = multiplier a and modulo m will both be 32-bit numbers. 
Setting it to false makes it the same implementation as C++ minstd_rand. */
#define PM_USE_DOUBLE_WIDTH_64_PRODUCT      false

// ***** Global Variables ******************************************************

/* Class specific variables */
typedef struct LCGTag
{
    uint64_t state;
    bool isSeeded;
} LCG;

typedef struct ParkMillerTag
{
    uint64_t state;
    bool isSeeded;
} ParkMiller;

typedef struct SchrageTag
{
    uint32_t state;
    bool isSeeded;
} Schrage;

/* TODO After I test all of these individually, I'll probably combine them into 
a single class with a type specifier. - MS */

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

/* TODO combine all PRNG's into a class. Change the function signature of the 
other functions to take the state value as an argument directly. This will 
still allow the use of the individual functions if desired for a little more 
speed. */
// void PRNG_Seed(PRNG *self);

// uint32_t PRNG_Next(PRNG *self);

// uint32_t PRNG_NextBounded(PRNG *self, uint32_t lower, uint32_t upper);

// uint32_t PRNG_LCGSkipAhead(PRNG *self, uint32_t skip);


void PRNG_LCGSeed(LCG *self, uint32_t seed);

uint32_t PRNG_LCGNext(LCG *self);

uint32_t PRNG_LCGBounded(LCG *self, uint32_t lower, uint32_t upper);

uint32_t PRNG_LCGSkip(LCG *self, int64_t ns);


void PRNG_ParkMillerSeed(ParkMiller *self, uint32_t seed);

uint32_t PRNG_ParkMillerNext(ParkMiller *self);

uint32_t PRNG_ParkMillerBounded(ParkMiller *self, uint32_t lower, uint32_t upper);

uint32_t PRNG_ParkMillerSkip(ParkMiller *self, int32_t ns);


void PRNG_SchrageSeed(Schrage *self, uint32_t seed);

uint32_t PRNG_SchrageNext(Schrage *self);

uint32_t PRNG_SchrageBounded(Schrage *self, uint32_t lower, uint32_t upper);


#endif  /* PRNG_H */