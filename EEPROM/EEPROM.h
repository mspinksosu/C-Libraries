/***************************************************************************//**
 * @brief EEPROM Library Implementation Header (Non-Processor Specific)
 * 
 * @author Matthew Spinks
 * 
 * @date 2/22/23   Original creation
 * 
 * @file EEPROM.h
 * 
 * @details
 *      This is for a built-in EEPROM, not a serial EEPROM.
 * 
 ******************************************************************************/

#ifndef EEPROM__H
#define EEPROM__H

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

// TODO add write and read data functions
void EEPROM_WriteData(uint16_t address, uint8_t *data, uint8_t size);

#endif  /* EEPROM__H */