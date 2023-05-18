/***************************************************************************//**
 * @brief Map Function Implementation (Linear Interpolation)
 * 
 * @file MF_Linear.c
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 12/19/21  Original creation
 * 
 * @details
 *      A library that implements the MF_Linear functions, which conform to 
 * the IMapFunction interface.
 * 
 * @section license License
 * SPDX-FileCopyrightText: © 2021 Matthew Spinks
 * SPDX-License-Identifier: Zlib
 * 
 * This software is released under the Zlib license. You are free alter and
 * redistribute it, but you must not misrepresent the origin of the software.
 * This notice may not be removed. <http://www.zlib.net/zlib_license.html>
 * 
 ******************************************************************************/

#include "MF_Linear.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/*  Declare an interface struct and initialize its members the our local 
    functions. Typecasting is necessary. When a new sub class object is 
    created, we will set its interface member equal to this table. */
static MFInterface FunctionTable = {
    .Compute = (int32_t (*)(void *, int32_t))MF_Linear_Compute,
};

// ***** Static Function Prototypes ********************************************

/* Put static function prototypes here */


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void MF_Linear_Create(MF_Linear *self, MapFunction *base)
{
    self->super = base;

    /*  Call the base class constructor */
    MF_Create(base, self, &FunctionTable);
}

// *****************************************************************************

void MF_Linear_SetRange(MF_Linear *self, uint32_t oldMin, uint32_t oldMax, uint32_t newMin, uint32_t newMax)
{
    self->oldMin = oldMin;
    self->oldMax = oldMax;
    self->newMin = newMin;
    self->newMax = newMax;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

int32_t MF_Linear_Compute(MF_Linear *self, int32_t input)
{
    int32_t output = (input - self->oldMin) * (self->newMax - self->newMin) / 
                     (self->oldMax - self->oldMin) + self->newMin;

    return output;
}

/*
 End of File
 */