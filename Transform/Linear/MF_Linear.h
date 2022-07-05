/***************************************************************************//**
 * @brief Map Function Implementation Header (Lookup Table)
 * 
 * @author Matthew Spinks
 * 
 * @date February 21, 2022  Original creation
 * 
 * @file MF_Linear.h
 * 
 * @details
 *      Implements the base class MapFunction. This implementation uses a 
 * a method known as linear interpolation to map one range of value to another.
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
 * super        The base class we are inheriting from
 * 
 * oldMin TODO
 * 
 * oldMax
 * 
 * newMin
 * 
 * newMax
 * 
 */

// ***** Function Prototypes ***************************************************

void MF_Linear_Create(MF_Linear *self, MapFunction *base, uint32_t oldMin, uint32_t oldMax, uint32_t newMin, uint32_t newMax);

// ----- Interface Functions ---------------------------------------------------

int32_t MF_Linear_Compute(MF_Linear *self, int32_t input);

#endif	/* MF_LINEAR_H */