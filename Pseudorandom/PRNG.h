/***************************************************************************//**
 * @brief Pseudorandom Number Generators with Logarithmic Skip Ahead
 * 
 * @file PRNG.h
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 9/23/23   Original creation
 * 
 * @details
 *      By far, the most attractive part of this library is the logarithmic 
 * skip ahead algorithm. It will allow you to traverse a sequence of 
 * pseudorandom values in O(log2(n)) time instead of O(n). More on that later.
 * I have collected three different types of PRNG plus a generic algorithm to
 * shuffle an array for you.
 * 
 * The Big LCG is an extension of a basic linear congruential generator, except 
 * that this one makes use of 64-bit math to get a longer output. The basic 
 * formula for an LCG is: "X_n+1 = (a * X_n + c) % m". LCG's have an advantage 
 * of being very fast if m is chosen to me a power of two because the modulus 
 * can be reduced to a logical AND. The downside is that only the upper bits 
 * of the LCG are pseudorandom. The typical rand function uses this sort of 
 * LCG. The state is a 32-bit variable, but only 15 bits are used. (bits 30 
 * through 16). My big LCG version just expands upon this to get a bigger 
 * number for the output. Instead of using a 32-bit variable, it uses 64-bit 
 * variables to store the state and returns the upper 32 bits. When the 
 * parameters a, m, and c are chosen correctly, the period of the LCG is m.
 * 
 * The Park Miller is special type of LCG that has removes the increment value 
 * c and has more restrictions in the choice of the multiplier a and modulus m. 
 * The value m is chosen to be a prime number close to range of the desired 
 * output. This is why you see the number 2^31-1 pop up lot. It is a Mersenne 
 * prime number that is close to the word size of 32-bits. There are also other 
 * restrictions placed on the relationship between a and m. The Park Miller is 
 * also sometimes referred to as a "Multiplicative Linear Congruential 
 * Generator". Its output is much better than the basic LCG but it is slower 
 * because it uses modulus of a prime number. When a and m are chosen correctly, 
 * the period of the Park Miller is m - 1. In fact, the Park Miller LCG is 
 * a "full-cycle" LCG. Meaning it will produce every number from 1 to m-1 
 * once before the sequence repeats.
 * 
 * // TODO more notes about Park Miller and Schrage
 * // TODO notes about PM value of 0 and X_0
 * // TODO lots of notes about the logarithmic skip ahead algorithm
 * 
 * The actual PRNG functions use a pointer to the seed. I did this to allow you 
 * to just use the individual functions if desired for a little more speed. The 
 * PRNG class just makes it easier to manage the state of the PRNG and do 
 * things like give you a random number from a certain range.
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


// ***** Global Variables ******************************************************

typedef enum PRNGTypeTag
{
    PRNG_TYPE_LCG_BIG,
    PRNG_TYPE_LCG_SMALL,
    PRNG_TYPE_PARK_MILLER,
    PRNG_TYPE_SCHRAGE,
} PRNGType;

typedef struct PRNGTag
{
    void *instance;
    PRNGType type;
    bool isSeeded;
    union PRNGStateType
    {
        uint64_t u64;
        uint32_t u32;
    } state;
} PRNG;

/** 
 * // TODO Description of struct
 * 
 * member1      description of variable member1
 * 
 */

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Function Prototypes *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/* TODO Decide if I want to implement a skip function for Schrage */
/* TODO Finish Doxygen */
/* TODO Add lots of notes */

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 * @param type 
 */
void PRNG_Create(PRNG *self, PRNGType type);

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 * @param seed 
 */
void PRNG_Seed(PRNG *self, uint32_t seed);

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 * @return uint32_t 
 */
uint32_t PRNG_Next(PRNG *self);

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 * @param lower 
 * @param upper 
 * @return uint32_t 
 */
uint32_t PRNG_NextBounded(PRNG *self, uint32_t lower, uint32_t upper);

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 * @param n 
 * @return uint32_t 
 */
uint32_t PRNG_Skip(PRNG *self, int64_t n);

/***************************************************************************//**
 * @brief Shuffle an array using the Fisher-Yates method
 * 
 * @param array  pointer to an array of any type
 * @param n  number of elements
 * @param s  the size in bytes of each element
 * @param seed  seed for the shuffle algorithm
 */
void PRNG_Shuffle(void *array, uint32_t n, size_t s, uint32_t seed);

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Individual PRNG Function Prototypes *********************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief 
 * 
 * // TODO range should be 0 to 2^32-1. period 2^63
 * 
 * @param state 
 * @return uint32_t 
 */
uint32_t LCGBig_Next(uint64_t *state);

/***************************************************************************//**
 * @brief 
 * 
 * // TODO range should be 0 to 2^16-1. period 2^31
 * 
 * @param state 
 * @return uint16_t 
 */
uint16_t LCGSmall_Next(uint32_t *state);

/***************************************************************************//**
 * @brief 
 * 
 * @param state 
 * @param n 
 * @return uint32_t 
 */
uint32_t LCGBig_Skip(uint64_t *state, int64_t n);

/***************************************************************************//**
 * @brief 
 * 
 * @param state 
 * @param n 
 * @return uint16_t 
 */
uint16_t LCGSmall_Skip(uint32_t *state, int32_t n);

/***************************************************************************//**
 * @brief Park Miller 64-bit Double Width Product
 * 
 * // TODO not implemented yet
 * 
 * @param state 
 * @return uint32_t 
 */
uint32_t ParkMillerBigger_Next(uint64_t *state);

/***************************************************************************//**
 * @brief Park Miller (same as C++ rand)
 * 
 * // TODO range should be 1 to 2^31-1. period 2^32-1
 * 
 * @param state 
 * @return uint32_t 
 */
uint32_t ParkMiller_Next(uint64_t *state);

/***************************************************************************//**
 * @brief 
 * 
 * @param state 
 * @param n 
 * @return uint32_t 
 */
uint32_t ParkMiller_Skip(uint64_t *state, int64_t n);

/***************************************************************************//**
 * @brief 
 * 
 * // TODO range should be 1 to 2^31-1. period 2^32-1
 * 
 * @param state 
 * @return uint32_t 
 */
uint32_t Schrage_Next(uint32_t *state);

#endif  /* PRNG_H */