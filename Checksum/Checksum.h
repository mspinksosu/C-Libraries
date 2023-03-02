/***************************************************************************//**
 * @brief Checksum Library Header File
 * 
 * @author Matthew Spinks
 * 
 * @date 2/25/23   Original creation
 * 
 * @file Checksum.h
 * 
 * @details
 *      I decided to make a simple file to hold some different checksum 
 * routines, since it's something that I use a lot.
 * 
 ******************************************************************************/

#ifndef CHECKSUM_H
#define CHECKSUM_H

#include <stdint.h>
#include <stdbool.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Function Prototypes *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

uint8_t Checksum_TwosComp8Bit(const uint8_t *array, uint16_t length);

uint16_t Checksum_TwosComp16Bit(const uint8_t *array, uint16_t length);

uint8_t Checksum_OnesComp8Bit(const uint8_t *array, uint16_t length);

uint16_t Checksum_OnesComp16Bit(const uint8_t *array, uint16_t length);

#endif  /* CHECKSUM_H */