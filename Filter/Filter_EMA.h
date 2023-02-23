/***************************************************************************//**
 * @brief Filter Library Implementation Header (EMA Filter)
 * 
 * @author Matthew Spinks
 * 
 * @date 12/18/22  Original creation
 * 
 * @file Filter_EMA.h
 * 
 * @details
 *      TODO
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