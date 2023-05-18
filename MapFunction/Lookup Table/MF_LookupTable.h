/***************************************************************************//**
 * @brief Map Function Implementation Header (Lookup Table)
 * 
 * @file MF_LookupTable.h
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 12/20/21  Original creation
 * 
 * @details
 *      Implements the base class MapFunction. This implementation uses the
 * a simple lookup table method for computing a curve. The lookup table is an
 * array of bytes that I have precomputed. In order to use it, simply take the 
 * input which is typically an adc reading or something similar and go to that 
 * index in the array.
 * 
 * I like to make my lookup tables small powers of two, usually between 64 and 
 * 128 values. This way I can easily scale my adc reading down by shifting it 
 * to the right. For example, if my adc reading is a 16-bit unsigned int and 
 * my lookup table has 128 values, I will need to shift my input right 9 times 
 * to match my lookup table. I've included an optional parameter called
 * shiftInputRightNBits to do this for me so I can pass the input directly to 
 * the function.
 * 
 * @section example_code Example Code
 *      LUTArray[32] = { ... };
 *      MapFunction myCurve;
 *      MF_LookupTable myLUT;
 *      MF_LookupTable_Create(&myLUT, &myCurve, &LUTArray, sizeof(LUTArray));
 *      output = MF_Compute(&Curve, adcValue);   
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

#ifndef MF_LOOKUP_H
#define MF_LOOKUP_H

#include "IMapFunction.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

typedef struct MF_LookupTableTag
{
    MapFunction *super;
    uint8_t *lookUpTable;
    uint8_t numPoints;
    uint8_t shiftInputRightNBits;
} MF_LookupTable;

/** 
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
 * @param arrayLUT  pointer to the actual lookup table
 * 
 * @param numPoints  number of entries in the lookup table
 */
void MF_LookupTable_Create(MF_LookupTable *self, MapFunction *base, uint8_t *arrayLUT, uint8_t numPoints);

/***************************************************************************//**
 * @brief  Set the value to shift the input to match your lookup table array
 * 
 * Most adc readings are between 8 and 16 bits in length. Whereas, a lookup 
 * table could be much smaller. Shifting the input is a quick way to scale it
 * to match your table. I'll handle the bit shift for you, so you don't have to
 * add it before your function call. This works great for lookup tables where 
 * the number of entries are a power of two. If you don't have the proper 
 * amount, I will just stop at the maximum value.
 * 
 * @param self  pointer to the LUT object you are using
 * 
 * @param shiftInputRightNBits  shift input right. default is 0
 */
void MF_LookupTable_SetRightShiftInput(MF_LookupTable *self, uint8_t shiftInputRightNBits);

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Compute the output of the curve using the lookup table
 * 
 * The input will restricted to 0 to numPoints - 1.
 * 
 * @param self  pointer to the LUT object you are using
 * 
 * @param input   input to map the function
 * 
 * @return int32_t  output of the map function
 */
int32_t MF_LookupTable_Compute(MF_LookupTable *self, int32_t input);

#endif	/* MF_LOOKUP_H */