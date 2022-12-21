/***************************************************************************//**
 * @brief Filter Library Implementation Header (SMA Filter)
 * 
 * @author Matthew Spinks
 * 
 * @date 12/2/14   Original creation
 * @date 2/4/22    Modified
 * 
 * @file Filter_SMA.h
 * 
 * @details
 *      TODO
 * 
 ******************************************************************************/

#ifndef FILTER_SMA_H
#define FILTER_SMA_H

#include "IFilter.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

typedef struct Filter_SMATag
{
    Filter *super;
    uint16_t *buffer;
    uint32_t sum;
    uint8_t bufferLength;
    uint8_t index;
} Filter_SMA;

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
 * @param self  pointer to the SMA Filter object you are using
 * 
 * @param base  pointer to the base class object used for function calls
 * 
 * @param buffer  pointer to an array for storing samples
 * 
 * @param bufferLength  the length of the array
 */
void Filter_SMA_Create(Filter_SMA *self, Filter *base, uint16_t *buffer, uint8_t bufferLength);

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Compute the output of the SMA filter with a given input
 * 
 * @param self  pointer to the SMA Filter you are using
 * 
 * @param input  input to the filter
 * 
 * @return uint16_t  output of the filter
 */
uint16_t Filter_SMA_ComputeU16(Filter_SMA *self, uint16_t input);

#endif  /* FILTER_SMA_H */