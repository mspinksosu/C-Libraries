/***************************************************************************//**
 * @brief Map Function Implementation (Lookup Table)
 * 
 * @file MF_LookupTable.c
 * 
 * @author Matthew Spinks
 * 
 * @date 12/20/21  Original creation
 * 
 * @details
 *      A library that implements the MF_LookupTable functions, which conform 
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

#include "MF_LookupTable.h"
#include <stdbool.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/*  Declare an interface struct and initialize its members the our local 
    functions. Typecasting is necessary. When a new sub class object is 
    created, we will set its interface member equal to this table. */
static MFInterface FunctionTable = {
    .Compute = (int32_t (*)(void *, int32_t))MF_LookupTable_Compute,
};

static bool shiftInputSet = false;

// ***** Static Function Prototypes ********************************************

/* Put static function prototypes here */


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void MF_LookupTable_Create(MF_LookupTable *self, MapFunction *base, uint8_t *arrayLUT, uint8_t numPoints)
{
    self->super = base;
    self->lookUpTable = arrayLUT;
    self->numPoints = numPoints;
    
    if(!shiftInputSet)
        self->shiftInputRightNBits = 0;

    /*  Call the base class constructor */
    MF_Create(base, self, &FunctionTable);
}

// *****************************************************************************

void MF_LookupTable_SetRightShiftInput(MF_LookupTable *self, uint8_t shiftInputRightNBits)
{
    self->shiftInputRightNBits = shiftInputRightNBits;
    shiftInputSet = true;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

int32_t MF_LookupTable_Compute(MF_LookupTable *self, int32_t input)
{
    input = (input >> self->shiftInputRightNBits);
    
    if(input > self->numPoints - 1)
    { 
        input = self->numPoints - 1;
    }
    else if(input < 0)
    {
        input = 0;
    }
    return (self->lookUpTable[input]);
}

/*
 End of File
 */