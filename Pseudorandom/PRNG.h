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

// TODO see if it's possible to combine big and small classes together
typedef enum PRNGTypeTag
{
    PRNG_TYPE_LCG_BIG,
    PRNG_TYPE_LCG_SMALL,
    PRNG_TYPE_PARK_MILLER_BIG,
    PRNG_TYPE_PARK_MILLER_SMALL,
    PRNG_TYPE_SCHRAGE_BIG,
    PRNG_TYPE_SCHRAGE_SMALL,
} PRNGType;

typedef enum PRNGBigTypeTag
{
    PRNG_LCG_BIG,
    PRNG_PARK_MILLER_BIG,
    PRNG_SCHRAGE_BIG,
} PRNGBigType;

typedef enum PRNGSmallTypeTag
{
    PRNG_LCG_SMALL,
    PRNG_PARK_MILLER_SMALL,
    PRNG_SCHRAGE_SMALL,
} PRNGSmallType;

typedef struct PRNGTag
{
    void *instance;
    PRNGType type;
    bool isSeeded;
    // union PRNGStateType
    // {
    //     uint64_t state64;
    //     uint32_t state32;
    // } PRNGstate;
} PRNG;

typedef struct PRNGBigTag
{
    PRNG *super;
    uint64_t state;
} PRNGBig;

typedef struct PRNGSmallTag
{
    PRNG *super;
    uint32_t state;
} PRNGSmall;

// // TODO ---------- move these into classes--------------------------------------
// typedef struct LCGTag
// {
//     uint64_t state;
// } LCG;

// typedef struct ParkMillerTag
// {
//     uint64_t state;
// } ParkMiller;

// typedef struct SchrageTag
// {
//     uint32_t state;
// } Schrage;

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

// Possible types: LCGBig, LCGSmall, ParkMillerBig, ParkMillerSmall, SchrageBig, SchrageSmall

void PRNGBig_Create(PRNGBig *self, PRNG *base, PRNGBigType type);
void PRNGSmall_Create(PRNGSmall *self, PRNG *base, PRNGSmallType type);

void PRNGBig_Seed(PRNGBig *self, uint32_t seed);
void PRNGSmall_Seed(PRNGSmall *self, uint32_t seed);

/* TODO The small LCG returns 16-bit, but Park Miller returns 32. 
Should I make the Park Miller big and make an even bigger version? */

uint32_t PRNGBig_Next(PRNGBig *self);
uint16_t PRNGSmall_Next(PRNGSmall *self);

uint32_t PRNGBig_NextBounded(PRNGBig *self, uint32_t lower, uint32_t upper);
uint16_t PRNGSmall_NextBounded(PRNGSmall *self, uint16_t lower, uint16_t upper);

uint32_t LCGBig_Next(uint64_t *state);
uint16_t LCGSmall_Next(uint32_t *state); // not implemented yet

uint32_t ParkMillerBigger_Next(uint64_t *state); // not implemented yet
uint32_t ParkMillerBig_Next(uint32_t *state);
uint16_t ParkMillerSmall_Next(uint32_t *state); // TODO no small version of park miller required I think

uint32_t SchrageBig_Next(uint32_t *state);
uint16_t SchrageSmall_Next(uint32_t *state); // not implemented yet

uint32_t LCGBig_Skip(uint64_t *state, int64_t n);
uint16_t LCGSmall_Skip(uint32_t *state, int32_t n); // not implemented yet

uint32_t ParkMillerBig_Skip(uint32_t *state, int64_t n);
uint16_t ParkMillerSmall_Skip(uint32_t *state, int32_t n); // not implemented yet

// -----------------------------------------------------------------------------

// void PRNG_LCGSeed(LCG *self, uint32_t seed);

// uint32_t PRNG_LCGNext(LCG *self);

// uint32_t PRNG_LCGBounded(LCG *self, uint32_t lower, uint32_t upper);

// uint32_t PRNG_LCGSkip(LCG *self, int64_t n);


// void PRNG_ParkMillerSeed(ParkMiller *self, uint32_t seed);

// uint32_t PRNG_ParkMillerNext(ParkMiller *self);

// uint32_t PRNG_ParkMillerBounded(ParkMiller *self, uint32_t lower, uint32_t upper);

// uint32_t PRNG_ParkMillerSkip(ParkMiller *self, int64_t n);


// void PRNG_SchrageSeed(Schrage *self, uint32_t seed);

// uint32_t PRNG_SchrageNext(Schrage *self);

// uint32_t PRNG_SchrageBounded(Schrage *self, uint32_t lower, uint32_t upper);


#endif  /* PRNG_H */