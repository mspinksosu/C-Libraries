/***************************************************************************//**
 * @brief Murphy's Modified Bresenham Line Algorithm
 * 
 * @author Matthew Spinks
 * 
 * @date 5/3/23    Original creation
 * 
 * @file ThickLine.h
 * 
 * @details
 *      TODO
 * 
 ******************************************************************************/

#ifndef THICK_LINE_H
#define THICK_LINE_H

#include <stdint.h>
#include <stdbool.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/* Class specific variables */

/** 
 * Description of struct
 * 
 * member1      description of variable member1
 * 
 */

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Function Prototypes *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void Murphy_DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, 
    uint8_t width, uint16_t rgb565Color);

#endif  /* THICK_LINE_H */