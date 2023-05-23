/***************************************************************************//**
 * @brief Map Function Implementation Header (Piecewise)
 * 
 * @file MF_Piecewise.h
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 12/22/21  Original creation
 * 
 * @details
 *      // TODO details
 * 
 * @section example_code Example Code
 *      
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

#ifndef MF_PIECEWISE_H
#define MF_PIECEWISE_H

#include "IMapFunction.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

typedef struct coordinateTag
{
    uint16_t xInput;
    uint16_t yOutput;
} coordinate;

typedef struct MF_PiecewiseTag
{
    MapFunction *super;
    coordinate *coordinates;
    uint16_t numCoordinates;
} MF_Piecewise;

/** // TODO
 * super        the base class we are inheriting from
 * 
 * lookUpTable  pointer to the array that you are going to use
 * 
 * numPoints    number of entries in the lookup table
 * 
 * shiftInputRightNBits  shift the input n bits to the right to scale it to 
 *                       match the lookup table size
 */

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Connects the sub class and base class object then calls MF_Create
 * 
 * @param self  pointer to the LUT object you are using
 * 
 * @param base  pointer to the base class object used for function calls
 * 
 * @param coordinateArray  pointer to an array of coordinates
 * 
 * @param numPoints  number of points or coordinates in the array
 */
void MF_Piecewise_Create(MF_Piecewise *self, MapFunction *base, 
    coordinate *coordinateArray, uint8_t numPoints);

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Compute the output of the curve using the equation
 * 
 * @param self  pointer to the object you are using
 * 
 * @param input   input to map the function
 * 
 * @return int32_t  output of the map function
 */
int32_t MF_Piecewise_Compute(MF_Piecewise *self, int32_t input);

#endif	/* MF_PIECEWISE_H */