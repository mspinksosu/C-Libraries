/***************************************************************************//**
 * @brief Pseudorandom Number Generators with Logarithmic Skip
 * 
 * @file PRNG.c
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 9/23/23   Original creation
 * 
 * @details
 *      The values of a and m for the LCG's and the big Park Miller LCG were 
 * chosen from "Tables of Linear Congruential Generators of Different Sizes and 
 * Good Lattice Structure by L'Ecuyer". In that paper, many different values 
 * were tested and ranked by the author based on performance with a spectral 
 * test. Then top values were chosen and compiled into different lists.
 * 
 * // TODO add notes about logarithmic skip
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

#include "PRNG.h"
#include "string.h"

// ***** Defines ***************************************************************

/* Using this LCG requires 64-bit math. Modulus m is power of two */
#define LCG_BIG_M               (1ULL << 63)
#define LCG_BIG_MASK            (LCG_BIG_M - 1ULL)
#define LCG_BIG_A               3249286849523012805ULL
/* m and c must be relatively prime, so c = 1 is commonly chosen */
#define LCG_BIG_C               1ULL
#define LCG_BIG_DEFAULT_SEED    1ULL

/* Modulus m is power of two */
#define LCG_SMALL_M             (1UL << 31)
#define LCG_SMALL_MASK          (LCG_SMALL_M - 1UL)
#define LCG_SMALL_A             20501397UL
/* m and c must be relatively prime, so c = 1 is commonly chosen */
#define LCG_SMALL_C             1UL
#define LCG_SMALL_DEFAULT_SEED  1UL

/* For the Park Miller, modulus m is chosen to be a prime number. */
#define PM_BIGGER_M             ((1ULL << 63) - 25ULL)
#define PM_BIGGER_A             6458928179451363983ULL

/* This is also the default value for C++ minstd_rand */
#define PM_BIG_M                ((1UL << 31) - 1UL)
#define PM_BIG_A                48271UL

/* For a mulplicative LCG, the initial value X_0 must be co-prime to m. If m 
is chosen to be a prime number, then any value from 0 < X_0 < m will work. */
#define PM_DEFAULT_SEED         1UL

/* Precomputed values for Schrage's method. It uses the same multiplier and 
modulus as the 32-bit Park Miller */
#define SCH_M                   ((1UL << 31) - 1UL)
#define SCH_A                   48271UL
#define SCH_Q                   44488 // Q = M / A
#define SCH_R                   3399  // R = M % A

#define DEBUG_PRINT             true

#if DEBUG_PRINT
#include <stdio.h>
#endif

// ***** Global Variables ******************************************************


// ***** Static Functions Prototypes *******************************************


// *****************************************************************************

void PRNG_Seed(PRNG *self, uint32_t seed)
{
    if(seed == 0)
    {
        switch(self->type)
        {
            case PRNG_TYPE_LCG_BIG:
                seed = LCG_BIG_DEFAULT_SEED;
                break;
            case PRNG_TYPE_LCG_SMALL:
                seed = LCG_SMALL_DEFAULT_SEED;
                break;
            case PRNG_TYPE_PARK_MILLER:
                seed = PM_DEFAULT_SEED;
                break;
            case PRNG_TYPE_SCHRAGE:
                seed = PM_DEFAULT_SEED;
                break;
        }
    }
    self->state.u64 = seed;
    self->isSeeded = true;
}

// *****************************************************************************

uint32_t PRNG_Next(PRNG *self)
{
    uint32_t result = 0;

    if(!self->isSeeded)
        PRNG_Seed(self, 0);

    switch(self->type)
    {
        case PRNG_TYPE_LCG_BIG:
            result = LCGBig_Next(&(self->state.u64));
            break;
        case PRNG_TYPE_LCG_SMALL:
            result = LCGSmall_Next(&(self->state.u32));
            break;
        case PRNG_TYPE_PARK_MILLER:
            result = ParkMiller_Next(&(self->state.u64));
            break;
        case PRNG_TYPE_SCHRAGE:
            result = Schrage_Next(&(self->state.u32));
            break;
    }
    return result;
}

// *****************************************************************************

uint32_t PRNG_NextBounded(PRNG *self, uint32_t lower, uint32_t upper)
{
    uint32_t result = 0;
    uint32_t randMax = 0xFFFFFFFF;

    if(!self->isSeeded)
        PRNG_Seed(self, 0);

    if(lower > upper)
    {
        uint32_t temp = lower;
        lower = upper;
        upper = lower;
    }

    switch(self->type)
    {
        case PRNG_TYPE_LCG_BIG:
            randMax = 0xFFFFFFFF;
            break;
        case PRNG_TYPE_LCG_SMALL:
            randMax = LCG_SMALL_M;
            break;
        case PRNG_TYPE_PARK_MILLER:
            randMax = PM_BIG_M;
            break;
        case PRNG_TYPE_SCHRAGE:
            randMax = SCH_M;
            break;
    }

    // output = output % (upper - lower + 1) + min
    uint32_t range = upper - lower;
    if(range < 0xFFFFFFFF)
        range++;

    /* method for removing modulo bias */
    uint32_t threshold = randMax - randMax % range;
    do {
        switch(self->type)
        {
            case PRNG_TYPE_LCG_BIG:
                result = LCGBig_Next(&(self->state.u64));
                break;
            case PRNG_TYPE_LCG_SMALL:
                result = LCGSmall_Next(&(self->state.u32));
                break;
            case PRNG_TYPE_PARK_MILLER:
                result = ParkMiller_Next(&(self->state.u64));
                break;
            case PRNG_TYPE_SCHRAGE:
                result = Schrage_Next(&(self->state.u32));
                break;
        }
    } while(result >= threshold);

    return (result % range) + lower;

    /* TODO I've tested this modulo bias method against the naive modulo 
    method, and I haven't seen a difference yet. Maybe it only works for very 
    large values... */
}

// *****************************************************************************

uint32_t PRNG_Skip(PRNG *self, int64_t n)
{
    uint32_t result = 0;

    if(!self->isSeeded)
        PRNG_Seed(self, 0);

    switch(self->type)
    {
        case PRNG_TYPE_LCG_BIG:
            result = LCGBig_Skip(&(self->state.u64), n);
            break;
        case PRNG_TYPE_LCG_SMALL:
            result = LCGSmall_Skip(&(self->state.u32), (int32_t)n);
            break;
        case PRNG_TYPE_PARK_MILLER:
            result = ParkMiller_Skip(&(self->state.u64), n);
            break;
        case PRNG_TYPE_SCHRAGE:
            // TODO I could probably just substitute the PM skip.
            break;
    }
    return result;
}

// *****************************************************************************

void PRNG_Shuffle(void *array, uint32_t n, size_t s, uint32_t seed)
{
    uint8_t tmp[s];
    uint8_t *arrayPtr = array;
    uint32_t schrageState = seed;

    if(schrageState == 0)
        schrageState++;

    if(n == 0)
        n++;

    for(uint32_t i = n - 1; i > 0; i--)
    {
        // Pick a random index from 0 to i
        uint32_t j = Schrage_Next(&schrageState) % (i + 1); // TODO should I replace this with the "modulo bias removal method" like above?

        // Swap arr[i] with the element at random index
        memcpy(tmp, arrayPtr + j * s, s);
        memcpy(arrayPtr + j * s, arrayPtr + i * s, s);
        memcpy(arrayPtr + i * s, tmp, s);
    }
}

// *****************************************************************************

uint32_t LCGBig_Next(uint64_t *state)
{
    /* This version will use a power of two for the modulus for speed with
    the lower bits removed. Similar to C rand, but with 32-bit result. 
    Multiplier a will be chosen from L'Ecuyer research paper. Increment c 
    will need to be odd. Try with c = 1. */

    /* TODO Possible output values should be in the range 0 to 2^32-1. But 
    output is not full-cycle. Need to verify. */

    /* X_n+1 = (a * X_n + c) % m */
    *state = (LCG_BIG_A * (*state) + LCG_BIG_C) & LCG_BIG_MASK;
    return (uint32_t)(*state >> 30ULL); // bits[62:31]
}

// *****************************************************************************

uint16_t LCGSmall_Next(uint32_t *state)
{
    /* This version is similar to C rand. I will use a power of two for 
    the modulus for speed with the lower bits removed. Multiplier a will be 
    chosen from L'Ecuyer research paper. Increment c will need to be odd. 
    Try with c = 1. */

    /* TODO Possible output values should be in the range 0 to 2^16-1. But 
    output is not full-cycle. Need to verify. */

    /* X_n+1 = (a * X_n + c) % m */
    *state = (LCG_SMALL_A * (*state) + LCG_SMALL_C) & LCG_SMALL_MASK;
    return (uint16_t)(*state >> 15ULL); // bits[30:15]
}

// *****************************************************************************

uint32_t LCGBig_Skip(uint64_t *state, int64_t n)
{
    /* Given the seed, compute the nth term by skipping ahead logarithmically. 
    This algorithm will complete in O(log2(n)) operations instead of O(n).

    Knuth, Art of Computer Programming 3.2.1 Equation (6):
    X_n+k = (a^k * X_n + c(a^k - 1) / (a - 1)) % m 
    multiplier: a^k % m 
    increment: (c(a^k -1) / (a - 1)) % m

    Brown, Random Number Generation With Arbitrary Strides 1994:
    Used in random number generation for "Monte Carlo" calculations. The pseudo 
    code below is basically just a method to compute the formula that I cited 
    above by Donald Knuth. Once A and C are computed, the same basic formula 
    "X_n+k = (A * X_n + C) % m" for LCG's is used for the final value.
    pseudo code to compute A:
    A = 1, h = a, i = k + 2^m % 2^m 
    while(i > 0) {
        if( i = odd)
            A = (A * h) % 2^m 
        h = (h^2) % 2^m
        i = floor(i / 2) }
    pseudo code to compute C:
    C = 0, f = c, h = a, i = (k + 2^m) % 2^m 
    while(i > 0) {
        if( i = odd)
            C = (C * h + f) % 2^m
        f = (f * (h + 1)) % 2^m
        h = (h^2) % 2^m
        i = floor(i / 2) } */

    /* Compute i (number of times to skip ahead). If i is negative, add the 
    period until it is positive. Skipping backwards is the same as skipping 
    forwards that many times. */
    int64_t i = n;
    while(i < 0)
        i += LCG_BIG_M;
    i = i & LCG_BIG_MASK;

    uint64_t A = 1, h = LCG_BIG_A, C = 0, f = LCG_BIG_C;
#if DEBUG_PRINT
    uint32_t loopCount = 0;
#endif
    /* Now compute A and C. Both methods are combined into a single loop. */
    for(; i > 0LL; i >>= 1)
    {
        if(i & 1LL)
        {
            A = (A * h) & LCG_BIG_MASK;
            C = (C * h + f) & LCG_BIG_MASK;
        }
        f = (f * h + f) & LCG_BIG_MASK;
        h = (h * h) & LCG_BIG_MASK;
#if DEBUG_PRINT
        loopCount++;
#endif
    }

#if DEBUG_PRINT
    printf("Number of iterations: %llu\n", loopCount);
#endif
    *state = (A * (*state) + C) & LCG_BIG_MASK;
    return (uint32_t)(*state >> 30ULL);
}

// *****************************************************************************

uint16_t LCGSmall_Skip(uint32_t *state, int32_t n)
{
    /* This is the exact same as the big LCG skip ahead function above, just
    with smaller variables. I know lots of people hate C rand() with a passion. 
    But if the basic rand() is good enough for your use case, then this will 
    give you the opportunity to use the logarithmic skip ahead with it also. 
    It still uses the double width product, but it might use a little less 
    overhead depending on your compiler.

    X_n+1 = (A * X_n + C) % m
    X_n+k = (a^k * X_n + c(a^k - 1) / (a - 1)) % m 
    multiplier: a^k % m 
    increment: (c(a^k -1) / (a - 1)) % m */

    /* Compute i (number of times to skip ahead). If i is negative, add the 
    period until it is positive. Skipping backwards is the same as skipping 
    forwards that many times. */
    int32_t i = n;
    while(i < 0)
        i += LCG_SMALL_M;
    i = i & LCG_SMALL_MASK;

    uint32_t A = 1, h = LCG_SMALL_A, C = 0, f = LCG_SMALL_C;
#if DEBUG_PRINT
    uint32_t loopCount = 0;
#endif
    /* Now compute A and C. Both methods are combined into a single loop. */
    for(; i > 0LL; i >>= 1)
    {
        if(i & 1LL)
        {
            A = (A * h) & LCG_SMALL_MASK;
            C = (C * h + f) & LCG_SMALL_MASK;
        }
        f = (f * h + f) & LCG_SMALL_MASK;
        h = (h * h) & LCG_SMALL_MASK;
#if DEBUG_PRINT
        loopCount++;
#endif
    }

#if DEBUG_PRINT
    printf("Number of iterations: %llu\n", loopCount);
#endif
    *state = (A * (*state) + C) & LCG_SMALL_MASK;
    return (uint32_t)(*state >> 15ULL);
}

// *****************************************************************************

uint32_t ParkMiller_Next(uint64_t *state)
{
    /* TODO Add more notes
    This version will be a full-cycle PRNG with a modulus of a prime 
    number and c = 0. I believe the output values should be in the range of 
    1 to m - 1. */

    /* X_n+1 = (a * X_n) % m */
    *state = (PM_BIG_A * (*state)) % PM_BIG_M;
    return *state;
}

// *****************************************************************************

uint32_t ParkMillerBigger_Next(uint64_t *state)
{
    /* TODO Test. Add more notes.
    This version uses a 64-bit double width product */

    /* X_n+1 = (a * X_n) % m */
    *state = (PM_BIGGER_A * (*state)) % PM_BIGGER_M;
    return (uint32_t)(*state >> 31ULL);
}

// *****************************************************************************

uint32_t ParkMiller_Skip(uint64_t *state, int64_t n)
{
    /* This is the exact same as the LCG skip ahead formula, except that this
    time I don't calculate C. And since m is a prime number and not a power of 
    two, I can't reduce the modulo operation.

    X_n+1 = (A * X_n + C) % m
    X_n+k = (a^k * X_n + c(a^k - 1) / (a - 1)) % m 
    multiplier: a^k % m 
    increment: (c(a^k -1) / (a - 1)) % m */

    /* Compute i (number of times to skip ahead). If i is negative, add the 
    period until it is positive. Skipping backwards is the same as skipping 
    forwards that many times. */
    int64_t i = n;
    while(i < 0)
        i += PM_BIG_M;
    i = i % PM_BIG_M;

    uint64_t A = 1, h = PM_BIG_A;
#if DEBUG_PRINT
    uint32_t loopCount = 0;
#endif
    /* Now compute A */
    for(; i > 0LL; i >>= 1)
    {
        if(i & 1LL)
        {
            A = (A * h) % PM_BIG_M;
        }
        h = (h * h) % PM_BIG_M;
#if DEBUG_PRINT
        loopCount++;
#endif
    }

#if DEBUG_PRINT
    printf("Number of iterations: %llu\n", loopCount);
#endif
    *state = (A * (*state)) % PM_BIG_M;
    return *state;
}

// *****************************************************************************

uint32_t Schrage_Next(uint32_t *state)
{
    int32_t result;
    uint32_t X = *state;
    int32_t X_Div_Q, X_Mod_Q;
    /* Schrage's method is a version of a Park Miller that avoids the need to 
    use a 64-bit variable to store the product of a * x. For any integer "m" 
    and "a > 0" there exists unique integers "q" (quotient) and "r" (remainder) 
    such that "m = a * q + r" and "0 <= r < m". 
    
    "q = m / a" (integer division) and "r = m % a". We compute the product 
    a * x by the approximation: a * x = a(x % q) - r[x / q] (integer division).
    Then take the result and perform % m to it. This modulo m is further 
    simplified: if a * x = a(x % q) - r[x / q] is negative, m is added to it. */

    /* ax % m = a(x % q) - r[x / q] % m 
    For X_Mod_Q, since we already have x / q, x % q can easily be done without 
    doing any actual modulo division. */
    X_Div_Q = X / SCH_Q;
    X_Mod_Q = X - X_Div_Q * SCH_Q;
    result = SCH_A * X_Mod_Q - SCH_R * X_Div_Q;
    if(result < 0)
        result += SCH_M;

    return *state = (uint32_t)result;
}

/*
 End of File
 */