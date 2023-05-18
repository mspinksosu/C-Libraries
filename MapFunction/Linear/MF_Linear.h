/***************************************************************************//**
 * @brief Map Function Implementation Header (Lookup Table)
 * 
 * @file MF_Linear.h
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 12/19/21  Original creation
 * 
 * @details
 *      A library that implements IMapFunction interface. This implementation 
 * uses a method known as linear interpolation to map one range of value to 
 * another. There are five parameters. The input, the old range of values, 
 * and the new range of values. The function uses integer division, so your
 * output will be truncated. This usually isn't a problem for simple range
 * conversions. If you need something faster, consider using the MF_LookupTable
 * library. The main advantage of this library is that it could save a little
 * bit of memory if your lookup table is very large.
 * 
 * @section example_code Example Code
 *      MapFunction Map;
 *      MF_Linear linearMap;
 *      MF_Linear_Create(&linearMap, &Map);
 *      MF_Linear_SetRange(&linearMap, oldMin, oldMax, newMin, newMax);
 *      output = MF_Compute(&Map, input);
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

#ifndef MF_LINEAR_H
#define MF_LINEAR_H

#include "IMapFunction.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

typedef struct MF_LinearTag
{
    MapFunction *super;
    uint32_t oldMin;
    uint32_t oldMax;
    uint32_t newMin;
    uint32_t newMax;
} MF_Linear;

/** 
 * Description of struct members
 * 
 * super  The base class we are inheriting from
 * 
 * oldMin  the minimum value of the range you are converting from
 * 
 * oldMax  the maximum value of the range you are converting from
 * 
 * newMin  the minimum value of the range you are converting to
 * 
 * newMax  the maximum value of the range you are converting to
 */

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Connects the sub class and base class object then calls MF_Create
 * 
 * @param self  pointer to the linear map object you are using
 * 
 * @param base  pointer to the base class object used for function calls
 */
void MF_Linear_Create(MF_Linear *self, MapFunction *base);

/***************************************************************************//**
 * @brief Set the parameters of the linear map function
 * 
 * @param self  pointer to the linear map object you are using
 * @param oldMin  the minimum value of the range you are converting from
 * @param oldMax  the maximum value of the range you are converting from
 * @param newMin  the minimum value of the range you are converting to
 * @param newMax  the maximum value of the range you are converting to
 */
void MF_Linear_SetRange(MF_Linear *self, uint32_t oldMin, uint32_t oldMax, uint32_t newMin, uint32_t newMax);

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Compute the output of the linear map given an input
 * 
 * @param self  pointer to the linear map object you are using
 * 
 * @param input  input to the map function
 * 
 * @return int32_t  output of the map function
 */
int32_t MF_Linear_Compute(MF_Linear *self, int32_t input);

#endif	/* MF_LINEAR_H */