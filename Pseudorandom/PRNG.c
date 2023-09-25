/***************************************************************************//**
 * @brief Blank C File Template
 * 
 * @file blank.c
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

#include "PRNG.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************


// ***** Static Functions Prototypes *******************************************


// *****************************************************************************

void PRNG_LCGSeed(LCG *self, uint32_t seed)
{

}

// *****************************************************************************

uint32_t PRNG_LCGNext(LCG *self)
{
    /* TODO This version will use a power of two for the modulus for speed with
    the lower bits removed. Similar to C rand, but with 32-bit result. 
    Multiplier a will be chosen from L'Ecuyer research paper. Increment c 
    will need to be odd. Try with c = 1. */
}

// *****************************************************************************

uint32_t PRNG_LCGSkipAhead(LCG *self, uint32_t skip)
{

}

// *****************************************************************************

void PRNG_ParkMillerSeed(ParkMiller *self, uint32_t seed)
{

}

uint32_t PRNG_ParkMillerNext(ParkMiller *self)
{
    /* TODO This version will be a full-cycle PRNG with a modulus of a prime 
    number and c = 0. */
}

uint32_t PRNG_ParkMillerSkipAhead(ParkMiller *self, uint32_t skip)
{

}

/*
 End of File
 */