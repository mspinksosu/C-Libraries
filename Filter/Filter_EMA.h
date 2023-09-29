/***************************************************************************//**
 * @brief Filter Library Implementation Header (EMA Filter)
 * 
 * @file Filter_EMA.h
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 12/18/22  Original creation
 * 
 * @details
 *      TODO
 * 
 * @section license License
 * SPDX-FileCopyrightText: © 2022 Matthew Spinks
 * SPDX-License-Identifier: Zlib
 * 
 * This software is released under the Zlib license. You are free alter and
 * redistribute it, but you must not misrepresent the origin of the software.
 * This notice may not be removed. <http://www.zlib.net/zlib_license.html>
 * 
 ******************************************************************************/

#ifndef FILTER_EMA_H
#define FILTER_EMA_H

#include "IFilter.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

typedef struct Filter_EMATag
{
    Filter *super;
    uint16_t alphaU16;
    uint16_t prevOutput;
} Filter_EMA;

/** 
 * Description of struct
 * 
 * member1      description of variable member1
 * 
 */

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Connects the sub class to the base class
 * 
 * Calls the base class Filter_Create function. Each sub class object must have 
 * a base class.
 * 
 * This filter is called an exponential moving average filter, which is 
 * supposed to mimic a RC filter and uses less memory than an SMA filter. 
 * There is value called alpha that goes from 0 to 1.0. Values closer to 0 
 * make the filter roll off earlier. A value of 0 allows no output at all. A
 * value of 1.0 applies no filtering.
 * 
 * @param self  pointer to the EMA Filter object you are using
 * 
 * @param base  pointer to the base class object used for function calls
 * 
 * @param alpha  "smoothing factor" 0.0 to 1.0. Alters the Q of the filter
 *               0 is no output and 1.0 is all-pass
 */
void Filter_EMA_Create(Filter_EMA *self, Filter *base, float alpha);

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Compute the output of the EMA filter with a given input
 * 
 * @param self  pointer to the EMA Filter you are using
 * 
 * @param input  input to the filter
 * 
 * @return uint16_t  output of the filter
 */
uint16_t Filter_EMA_ComputeU16(Filter_EMA *self, uint16_t input);

#endif  /* FILTER_EMA_H */