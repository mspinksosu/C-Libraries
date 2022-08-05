/***************************************************************************//**
 * @brief Map Function Implementation (Lookup Table)
 * 
 * @author Matthew Spinks
 * 
 * @date December 24, 2021  Original creation
 * 
 * @file MF_LookupTable.c
 * 
 * @details
 *      Implements the base class MapFunction. This implementation uses a 
 * simple lookup table method for computing a map function. The lookup table is
 * an array of values that I have precomputed using an excel sheet. The output
 * of my lookup table will be a value from 0 to 255. The array itself
 * will use bytes. In order to comply with the interface though, the function
 * will return an int32.
 * 
 * Example Code:
 *      MapFunction Curve;
 *      MF_LUT LUTCurve;
 *      MF_LUT_Init(&LUTCurve, &Curve, &LUTArray, ...);
 *      output = MF_Compute(&Curve, adcValue);   
 * 
 ******************************************************************************/

#include "MF_LookupTable.h"

// ***** Defines ***************************************************************


// ***** Static Function Prototypes ********************************************


// ***** Global Variables ******************************************************

/*  Declare an interface struct and initialize its members the our local 
    functions. Typecasting is necessary. When a new sub class object is 
    created, we will set its interface member equal to this table. */
static MF_Interface FunctionTable = {
    .Compute = (int32_t (*)(void *, int32_t))MF_LookupTable_Compute,
};

/***************************************************************************//**
 * @brief Initializes a Lookup Table object.
 * 
 * @param self  pointer to the LUT object you are using
 * 
 * @param base  pointer to the base class object used for function calls
 * 
 * @param arrayLUT  pointer to the actual lookup table
 * 
 * @param numPoints  number of entries in the lookup table
 */
void MF_LookupTable_Init(MF_LookupTable *self, MapFunction *base, uint8_t *arrayLUT, uint8_t numPoints)
{
    MF_LookupTable_Init_WithBitShift(self, base, arrayLUT, numPoints, 0);
}

/***************************************************************************//**
 * @brief Initializes a Lookup Table object with a shift right option.
 * 
 * The shift right option is for shifting an adc value to the right in order to
 * quickly adjust the input to the size of the lookup table. Putting it here
 * just allows me to use the same function call in the main code without any
 * other changes to the input being passed in. It's only really useful if your
 * lookup table has a 2^n entries
 * 
 * @param self  pointer to the LUT object you are using
 * 
 * @param base  pointer to the base class object used for function calls
 * 
 * @param arrayLUT  pointer to the actual lookup table
 * 
 * @param numPoints  number of entries in the lookup table
 * 
 * @param shiftInputRightNBits  Shift the compute function's input right n bits
 */
void MF_LookupTable_Init_WithBitShift(MF_LookupTable *self, MapFunction *base, uint8_t *arrayLUT, uint8_t numPoints, uint8_t shiftInputRightNBits)
{
    self->super = base;
    self->lookUpTable = arrayLUT;
    self->numPoints = numPoints;

    /*  Most adc readings are between 8 and 16 bits in length. Whereas, my 
        lookup tables are usually 7 or 8 bits in length. I'll handle the bit
        shift for you, so you don't have to add it before your function call */
    self->shiftInputRightNBits = shiftInputRightNBits;

    /*  Call the base class constructor */
    MF_Create(base, self, &FunctionTable);
}

/***************************************************************************//**
 * @brief Compute the output of the trigger map given an input
 * 
 * @param self  pointer to the Trigger LUT object you are using
 * 
 * @param input   input to the map function
 * 
 * @return int32_t  output of the map function
 */
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