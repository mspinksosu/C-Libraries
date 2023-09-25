/***************************************************************************//**
 * @brief PRNG Library
 * 
 * @file PRNG.c
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

#include "PRNG.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************


// ***** Static Functions Prototypes *******************************************


// *****************************************************************************

void PRNG_LCGSeed(LCG *self, uint32_t seed)
{
    if(seed == 0)
        seed = LCG_DEFAULT_SEED;

    self->state = seed;
    self->isSeeded = true;
}

// *****************************************************************************

uint32_t PRNG_LCGNext(LCG *self)
{
    uint64_t result = 0;

    /* TODO This version will use a power of two for the modulus for speed with
    the lower bits removed. Similar to C rand, but with 32-bit result. 
    Multiplier a will be chosen from L'Ecuyer research paper. Increment c 
    will need to be odd. Try with c = 1. */

    /* X_n+1 = (a * X_n + c) % m */
    if(self->isSeeded == false)
    {
        self->state = LCG_DEFAULT_SEED;
        self->isSeeded = true;
    }

    result = (LCG_A * self->state + LCG_C) & LCG_MASK;
    self->state = (uint32_t)(result >> 32ULL);
    return self->state;
}

// *****************************************************************************

uint32_t PRNG_LCGSkipAhead(LCG *self, uint32_t skip)
{
    return 0;
}

// *****************************************************************************

void PRNG_ParkMillerSeed(ParkMiller *self, uint32_t seed)
{
    /* TODO I'm pretty sure the seed of the Park Miller can't change because it 
    says that X0 must be co-prime to m. */
    if(seed == 0)
        seed = PM_DEFAULT_SEED;

    self->state = seed;
    self->isSeeded = true;
}

// *****************************************************************************

uint32_t PRNG_ParkMillerNext(ParkMiller *self)
{
    uint64_t result = 0;

    /* TODO This version will be a full-cycle PRNG with a modulus of a prime 
    number and c = 0. */

    /* X_n+1 = (a * X_n) % m */
    if(self->isSeeded == false)
    {
        self->state = PM_DEFAULT_SEED;
        self->isSeeded = true;
    }

    result = (LCG_A * self->state) & LCG_MASK;
    self->state = (uint32_t)(result);
    return self->state;
}

// *****************************************************************************

uint32_t PRNG_ParkMillerSkipAhead(ParkMiller *self, uint32_t skip)
{
    return 0;
}

/*
 End of File
 */