/***************************************************************************//**
 * @brief EEPROM Library Implementation Header (Non-Processor Specific)
 * 
 * @file EEPROM.h
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 2/22/23   Original creation
 * 
 * @details
 *      This is for a built-in EEPROM, not a serial EEPROM.
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

#ifndef EEPROM_H
#define EEPROM_H

#include <stdint.h>

/* Include processor specific header files here if needed */


// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Write a byte to the EEPROM
 * 
 * @param address  the address in the EEPROM
 * 
 * @param data  byte to be written
 */
void EEPROM_WriteByte(uint16_t address, uint8_t data);

/***************************************************************************//**
 * @brief Read a byte from the EEPROM
 * 
 * @param address  the address in the EEPROM
 * 
 * @return uint8_t  the value at that location
 */
uint8_t EEPROM_ReadByte(uint16_t address);

/***************************************************************************//**
 * @brief Write multiple bytes to the EEPROM
 * 
 * This is a blocking function
 * 
 * @param address  the address in the EEPROM
 * 
 * @param data  pointer to the data to be written
 * 
 * @param size  the size of the data to be written
 */
void EEPROM_WriteData(uint16_t address, uint8_t *data, uint8_t size);

/***************************************************************************//**
 * @brief Read multiple bytes from the EEPROM
 * 
 * This is a blocking function
 * 
 * @param address  the address in the EEPROM
 * 
 * @param retData  pointer to the destination of the data read
 * 
 * @param size  the number of bytes to read
 */
void EEPROM_ReadData(uint16_t address, uint8_t *retData, uint8_t size);

#endif  /* EEPROM_H */
