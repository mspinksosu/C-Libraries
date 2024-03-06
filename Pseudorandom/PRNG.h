/***************************************************************************//**
 * @brief Pseudorandom Number Generators with Logarithmic Skip
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
 * shuffle an array for you. Each random number generator is basically just 
 * an object that holds its current state. Using this library, you can create 
 * multiple PRNG's of different types and seed each one independently.
 * 
 * The Big LCG is an extension of a basic linear congruential generator, except 
 * that this one makes use of 64-bit math to get a longer output. The basic 
 * formula for an LCG is: "X_n+1 = (a * X_n + c) % m". LCG's have an advantage 
 * of being very fast if m is chosen to be a power of two because the modulus 
 * can be reduced to a logical AND. The downside is that only the upper bits 
 * of the LCG are pseudorandom. The typical rand() function uses this sort of 
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
 * PRNG class just makes it easier to manage multiple PRNG's and do things like 
 * give you a random number from a certain range.
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
    PRNGType type;
    bool isSeeded;
    union PRNGStateType
    {
        uint64_t u64;
        uint32_t u32;
    } state;
} PRNG;

/** 
 * Description of struct members. You shouldn't really mess with any of these
 * variables directly. That is why I made functions for you to use.
 * 
 * type  The type of PRNG that you want to use
 * 
 * isSeeded  If you don't give the PRNG an initial value, I will set it to a 
 *           default value for you.
 * 
 * state  The internal value of the PRNG. May be a 32-bit or 64-bit value 
 *        depending on the type. The union makes it so you don't have to worry 
 *        about it. Be aware that output will either be a 16-bit or a 32-bit 
 *        number, but my functions all return a 32-bit number for simplicity. 
 *        A 64-bit PRNG will return a 32-bit number, and a 32-bit PRNG will 
 *        return a 16-bit number.
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
 * @brief Create a PRNG object
 * 
 * @param self  pointer to the PRNG that you are using
 * 
 * @param type  PRNG_TYPE_LCG_BIG, PRNG_TYPE_LCG_SMALL, PRNG_TYPE_PARK_MILLER,
 *              PRNG_TYPE_SCHRAGE,
 */
void PRNG_Create(PRNG *self, PRNGType type);

/***************************************************************************//**
 * @brief Seed the random number generator
 * 
 * @param self  pointer to the PRNG that you are using
 * 
 * @param seed  if you seed the PRNG with 0, I will change it to a default 
 *              value (usually 1)
 */
void PRNG_Seed(PRNG *self, uint32_t seed);

/***************************************************************************//**
 * @brief Get the next pseudorandom number in the sequence
 * 
 * @param self  pointer to the PRNG that you are using
 * 
 * @return uint32_t  output. Could be 16-bit value depending on the PRNG type
 */
uint32_t PRNG_Next(PRNG *self);

/***************************************************************************//**
 * @brief Return a random number within a specified boundary
 * 
 * @param self  pointer to the PRNG that you are using
 * 
 * @param lower  lower bound inclusive
 * 
 * @param upper  upper bound inclusive
 * 
 * @return uint32_t  output. Could be 16-bit value depending on the PRNG type
 */
uint32_t PRNG_NextBounded(PRNG *self, uint32_t lower, uint32_t upper);

/***************************************************************************//**
 * @brief Perform logarithmic skip (forwards or backwards)
 * 
 * Finds the nth number in the sequence. n = 0 will output 0.
 * 
 * @param self  pointer to the PRNG that you are using
 * 
 * @param n  nth number. positive = forwards, negative = backwards
 * 
 * @return uint32_t  output. Could be 16-bit value depending on the PRNG type
 */
uint32_t PRNG_Skip(PRNG *self, int64_t n);

/***************************************************************************//**
 * @brief Shuffle an array using the Fisher-Yates method
 * 
 * @param array  pointer to an array of any type
 * 
 * @param n  number of elements
 * 
 * @param s  the size in bytes of each element
 * 
 * @param seed  seed for the shuffle algorithm
 */
void PRNG_Shuffle(void *array, uint32_t n, size_t s, uint32_t seed);

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Individual PRNG Function Prototypes *********************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Big Linear Congruential Generator
 * 
 * // TODO range should be 0 to 2^32-1. period 2^63
 * 
 * @param state 
 * @return uint32_t 
 */
uint32_t LCGBig_Next(uint64_t *state);

/***************************************************************************//**
 * @brief Small Linear Congruential Generator
 * 
 * // TODO range should be 0 to 2^16-1. period 2^31
 * 
 * @param state 
 * @return uint16_t 
 */
uint16_t LCGSmall_Next(uint32_t *state);

/***************************************************************************//**
 * @brief Logarithmic skip function for Big LCG
 * 
 * @param state 
 * @param n 
 * @return uint32_t 
 */
uint32_t LCGBig_Skip(uint64_t *state, int64_t n);

/***************************************************************************//**
 * @brief Logarithmic skip function for small LCG
 * 
 * @param state 
 * @param n 
 * @return uint16_t 
 */
uint16_t LCGSmall_Skip(uint32_t *state, int32_t n);

/***************************************************************************//**
 * @brief Park Miller 64-bit double width product
 * 
 * // TODO range should be 1 to 2^31-1. period should be 2^63-5
 * 
 * @param state 
 * @return uint32_t 
 */
uint32_t ParkMillerBigger_Next(uint64_t *state);

/***************************************************************************//**
 * @brief Park Miller MLCG (same as C++ rand)
 * 
 * // TODO range should be 1 to 2^31-1. period 2^32-1
 * 
 * @param state 
 * @return uint32_t 
 */
uint32_t ParkMiller_Next(uint64_t *state);

/***************************************************************************//**
 * @brief Logarithmic skip function for Park Miller
 * 
 * @param state 
 * @param n 
 * @return uint32_t 
 */
uint32_t ParkMiller_Skip(uint64_t *state, int64_t n);

/***************************************************************************//**
 * @brief Implementation of Park Miller using Schrage's method
 * 
 * May or may not be faster depending on your target device word size and 
 * compiler. This was created to avoid using a double width product.
 * 
 * // TODO range should be 1 to 2^31-1. period 2^32-1
 * 
 * @param state 
 * @return uint32_t 
 */
uint32_t Schrage_Next(uint32_t *state);

#endif  /* PRNG_H */