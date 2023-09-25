/***************************************************************************//**
 * @brief Blank Library Header File
 * 
 * @file blank.h
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 12/2/14   Original creation
 * @date 2/4/22    Modified
 * 
 * @details
 *      TODO
 * 
 * @section license License
 * SPDX-FileCopyrightText: © 2019 Matthew Spinks
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
#define LCG_MASK    ((1ULL << 63) - 1ULL)
#define LCG_A       3249286849523012805ULL
#define LCG_C       1ULL

// ***** Global Variables ******************************************************

/* Class specific variables */
typedef struct LCGTag
{
    uint32_t state;
} LCG;

typedef struct ParkMillerTag
{
    uint32_t state;
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

void PRNG_LCGSeed(LCG *self, uint32_t seed);

uint32_t PRNG_LCGNext(LCG *self);

uint32_t PRNG_LCGSkipAhead(LCG *self, uint32_t skip);


void PRNG_ParkMillerSeed(ParkMiller *self, uint32_t seed);

uint32_t PRNG_ParkMillerNext(ParkMiller *self);

uint32_t PRNG_ParkMillerSkipAhead(ParkMiller *self, uint32_t skip);


#endif  /* PRNG_H */