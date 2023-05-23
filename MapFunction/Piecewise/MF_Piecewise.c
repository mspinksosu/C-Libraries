/***************************************************************************//**
 * @brief Map Function Implementation (Lookup Table)
 * 
 * @file MF_Piecewise.c
 * 
 * @author Matthew Spinks
 * 
 * @date 12/22/21  Original creation
 * 
 * @details
 *      A library that implements the MF_Piecewise functions, which conform 
 * to the IMapFunction interface. 
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

#include "MF_Piecewise.h"
#include <stdbool.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/*  Declare an interface struct and initialize its members the our local 
    functions. Typecasting is necessary. When a new sub class object is 
    created, we will set its interface member equal to this table. */
static MFInterface FunctionTable = {
    .Compute = (int32_t (*)(void *, int32_t))MF_Piecewise_Compute,
};

// ***** Static Function Prototypes ********************************************

/* Put static function prototypes here */


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void MF_Piecewise_Create(MF_Piecewise *self, MapFunction *base, 
    coordinate *coordinateArray, uint8_t numPoints)
{
    self->super = base;
    self->coordinates = coordinateArray;
    self->numCoordinates = numPoints;

    /*  Call the base class constructor */
    MF_Create(base, self, &FunctionTable);
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

int32_t MF_Piecewise_Compute(MF_Piecewise *self, int32_t input)
{
    int32_t i = 0, output = 0, n = self->numCoordinates;

    /* The x values should be ordered from smallest to largest */
    for(; i < n - 1; i++)
    {
        if(input <= self->coordinates[i].yOutput)
            break;
    }

    if(i == 0)
    {
        output = self->coordinates[0].yOutput;
    }
    else if(i == n - 1)
    {
        output = self->coordinates[n-1].yOutput;
    }
    else if(self->coordinates[i+1].xInput != self->coordinates[i].xInput)
    {
        output = (int32_t)((input - self->coordinates[i].xInput) * 
            (self->coordinates[i+1].yOutput - self->coordinates[i].yOutput) / 
            (self->coordinates[i+1].xInput - self->coordinates[i].xInput) + 
            self->coordinates[i].yOutput);
    }
    else
    {
        /* Overlapping x coordinates causes a divide by zero. */
        output = self->coordinates[i+1].yOutput;
    }

    return output;
}

/*
 End of File
 */