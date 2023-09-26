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

/* Option for the Park Miller. True = multiplier a and modulo m will both be 
64-bit numbers. False = multiplier a and modulo m will both be 32-bit numbers. 
Setting it to false uses the same implementation as C++ minstd_rand. */
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

/* TODO Add get random number from a to b. Work on modulo bias */

#endif  /* PRNG_H */