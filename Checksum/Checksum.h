/***************************************************************************//**
 * @brief Checksum Library Header File
 * 
 * @file Checksum.h
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 2/25/23   Original creation
 * 
 * @details
 *      I decided to make a simple file to hold some different checksum 
 * routines, since it's something that I use a lot.
 * 
 * @section license License
 * SPDX-FileCopyrightText: © 2023 Matthew Spinks
 * SPDX-License-Identifier: Zlib
 * 
 * This software is released under the Zlib license. You are free alter and
 * redistribute it, but you must not misrepresent the origin of the software.
 * This notice may not be removed. <http://www.zlib.net/zlib_license.html>
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