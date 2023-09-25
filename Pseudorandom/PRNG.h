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
 *      TODO
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

/* modulus m = 2^63 */
#define LCG_MASK            ((1ULL << 63) - 1ULL)
#define LCG_A               3249286849523012805ULL
#define LCG_C               1ULL
/* m and c must be relatively prime, so c = 1 is common chosen */
#define LCG_DEFAULT_SEED    1UL

#define PM_M                ((1ULL << 63) - 25ULL)
#define PM_A                6458928179451363983ULL
#define PM_DEFAULT_SEED     1UL

// ***** Global Variables ******************************************************

/* Class specific variables */
typedef struct LCGTag
{
    uint32_t state;
    bool isSeeded;
} LCG;

typedef struct ParkMillerTag
{
    uint32_t state;
    bool isSeeded;
} ParkMiller;

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

void PRNG_srand(uint32_t seed);

uint32_t PRNG_randU32(void);

uint16_t PRNG_randU16(void);


void PRNG_LCGSeed(LCG *self, uint32_t seed);

uint32_t PRNG_LCGNext(LCG *self);

uint32_t PRNG_LCGSkipAhead(LCG *self, uint32_t skip);


void PRNG_ParkMillerSeed(ParkMiller *self, uint32_t seed);

uint32_t PRNG_ParkMillerNext(ParkMiller *self);

uint32_t PRNG_ParkMillerSkipAhead(ParkMiller *self, uint32_t skip);

/* TODO Add get random number from a to b. Work on modulo bias */

#endif  /* PRNG_H */