/***************************************************************************//**
 * @brief Map Function Implementation Header (Lookup Table)
 * 
 * @author Matthew Spinks
 * 
 * @date December 24, 2021  Original creation
 * 
 * @file MF_LookupTable.h
 * 
 * @details
 *      Implements the base class MapFunction. This implementation uses the
 * a simple lookup table method for computing a trigger map.
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
 * super        The base class we are inheriting from
 * 
 * lookUpTable  A pointer to the array that you are going to use
 * 
 * numPoints   Number of entries in the lookup table
 * 
 * shiftInputRightNBits     I like to make my look up tables small powers of
 *      two. Usually between 64 and 256 values if possible. What I typically
 *      do is shift the input of my adc reading to alter the range of the input
 *      to match my table
 */

// ***** Function Prototypes ***************************************************

void MF_LookupTable_Init(MF_LookupTable *self, MapFunction *base, uint8_t *arrayLUT, uint8_t numPoints);

void MF_LookupTable_Init_WithBitShift(MF_LookupTable *self, MapFunction *base, uint8_t *arrayLUT, uint8_t numPoints, uint8_t shiftInputRightNBits);

// ----- Interface Functions ---------------------------------------------------

int32_t MF_LookupTable_Compute(MF_LookupTable *self, int32_t input);

#endif	/* MF_LOOKUP_H */