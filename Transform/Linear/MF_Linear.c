/***************************************************************************//**
 * @brief Map Function Implementation (Linear Interpolation)
 * 
 * @author Matthew Spinks
 * 
 * @date February 21, 2022  Original creation
 * 
 * @file MF_Linear.c
 * 
 * @details
 *      Implements the base class MapFunction. This implementation uses a 
 * a method known as linear interpolation to map one range of value to another.
 * There are five parameters. The input, the old range of values, and the new
 * range of values. This function uses integer division. If you need a faster
 * method, consider using a lookup table. The advantage to using this function 
 * over a lookup table is potentially saving some memory if your lookup table
 * is really large.
 * 
 * Example Code:
 *      MapFunction Map;
 *      MF_Linear linearMap;
 *      MF_Linear_Create(&linearMap, &Map, oldMin, oldMax, newMin, newMax);
 *      output = MF_Compute(&Map, input);   
 * 
 ******************************************************************************/

#include "MF_Linear.h"

// ***** Defines ***************************************************************


// ***** Static Function Prototypes ********************************************


// ***** Global Variables ******************************************************

/*  Declare an interface struct and initialize its members the our local 
    functions. Typecasting is necessary. When a new sub class object is 
    created, we will set its interface member equal to this table. */
static MF_Interface FunctionTable = {
    .Compute = (int32_t (*)(void *, int32_t))MF_Linear_Compute,
};

/***************************************************************************//**
 * @brief Creates a Linear Map Function object and then calls the base class
 *        create function
 * 
 * @param self  pointer to the map object you are using
 * 
 * @param base  pointer to the base class object used for function calls
 * 
 * @param oldMin 
 * 
 * @param oldMax 
 * 
 * @param newMin 
 * 
 * @param newMax 
 */
void MF_Linear_Create(MF_Linear *self, MapFunction *base, uint32_t oldMin, uint32_t oldMax, uint32_t newMin, uint32_t newMax)
{
    self->super = base;
    self->oldMin = oldMin;
    self->oldMax = oldMax;
    self->newMin = newMin;
    self->newMax = newMax;

    /*  Call the base class constructor */
    MF_Create(base, self, &FunctionTable);
}

/***************************************************************************//**
 * @brief Compute the output of the linear map given an input
 * 
 * @param self  pointer to the map object you are using
 * 
 * @param input  input to the map function
 * 
 * @return int32_t  output of the map function
 */
int32_t MF_Linear_Compute(MF_Linear *self, int32_t input)
{
   uint32_t output;

   output = (input - self->oldMin) * (self->newMax - self->newMin) / 
            (self->oldMax - self->oldMin) + self->newMin;

    return output;
}

/*
 End of File
 */