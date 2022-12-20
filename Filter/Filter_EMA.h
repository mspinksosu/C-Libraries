/***************************************************************************//**
 * @brief Filter Library Implementation Header (EMA Filter)
 * 
 * @author Matthew Spinks
 * 
 * @date 12/2/14   Original creation
 * @date 2/4/22    Modified
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
 */
void Filter_EMA_Create(Filter_EMA *self, Filter *base);

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
uint16_t Filter_EMA_ComputeU16(Filter *self, uint16_t input);

#endif  /* FILTER_EMA_H */