/** 
 * This is to implement a curve using a simple lookup table. This file is for
 * the MF_LookupTable.c For this lookup table, I will convert the adc output 
 * to smaller bit value. This lookup table uses an array of bytes. The output 
 * will be a value from 0 to 255. I have precalculated the values in excel to 
 * provide a curve.
 * 
 * Even though the code for my implementation is rather simple, using the 
 * interface will allow the user to use any implementation at will without 
 * changing the calling function. This provides much more flexibility.
 * 
 * This is an example of a preprocessor macro. The intended use is to declare
 * an array and set it equal to MF_LUT_CURVE_1. Be careful not to add a 
 * semicolon to the end of this array. If you do, you will have two semicolons
 * in your code and possibly a very cryptic compiler error. You can have 
 * multiple different lookup tables in this file if you like.
 * 
 * Example usage:
 *      uint8_t LUTArray_Curve_1[MF_LUT_CURVE_1_SIZE] = MF_LUT_CURVE_1;
 *      MF_LookupTable_Init_WithBitShift(... , ... , &LUTArray_Curve_1, MF_LUT_CURVE_1_SIZE, MF_LUT_CURVE_1_SHIFT_RIGHT);
 * 
 * - Matt Spinks */

#ifndef MF_LUT_CURVE_H
#define MF_LUT_CURVE_H

#include "MF_LookupTable.h"

#define MF_LUT_CURVE_1_SIZE         128
#define MF_LUT_CURVE_1_SHIFT_RIGHT    9 // convert from 16 bit value to 7 bit

// Leave off the semicolon!
#define MF_LUT_CURVE_1 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,2,2,2,2,2,3,3,3,4,4,4,5,5,6,6,7,7,8,9,9,10,11,11,12,13,14,15,16,17,18,19,20,21,22,23,24,26,27,28,30,31,33,34,36,37,39,41,43,45,46,48,50,53,55,57,59,61,64,66,69,71,74,76,79,82,85,88,91,94,97,100,103,107,110,114,117,121,124,128,132,136,140,144,148,153,157,161,166,170,175,180,184,189,194,199,205,210,215,221,226,232,237,243,249,255}

#endif /* MF_LUT_CURVE_H */