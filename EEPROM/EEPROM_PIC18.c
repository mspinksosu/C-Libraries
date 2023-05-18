/***************************************************************************//**
 * @brief EEPROM Library Implementation (PIC18)
 * 
 * @file EEPROM_PIC18.c
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 2/22/23   Original creation
 * 
 * @details
 *      If you are using XC-8, it should have your chip selection defined in 
 * your project settings. Including xc.h should do the trick. If not, processor 
 * header files can be found in the install path of your compiler: 
 * "Install_Path/xc8/v2.20/pic/include/proc/"
 * Example: #include <proc/pic18lf4585.h>
 * 
 *  @section license License
 * SPDX-FileCopyrightText: © 2023 Matthew Spinks
 * SPDX-License-Identifier: Zlib
 * 
 * This software is released under the Zlib license. You are free alter and
 * redistribute it, but you must not misrepresent the origin of the software.
 * This notice may not be removed. <http://www.zlib.net/zlib_license.html>
 * 
 ******************************************************************************/

#include "EEPROM.h"

/* Include processor specific header files here. */
//#include <xc.h>
#include </proc/pic18lf4585.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************


// ***** Static Function Prototypes ********************************************

/* Put static function prototypes here */


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void EEPROM_WriteByte(uint16_t address, uint8_t data)
{
    EECON1bits.CFGS = 0;  // access the program memory/EEPROM
    EECON1bits.EEPGD = 0; // access the EEPROM

    EEADRH = address >> 8;
    EEADR = address;
    EEDATA = data;

    EECON1bits.WREN = 1;    // enable EEPROM write
    uint8_t gieBit = GIE;   // store global interrupt status
    GIE = 0;                // disable interrupts

    /* EEPROM write sequence */
    EECON2 = 0x55;
    EECON2 = 0xAA;
    EECON1bits.WR = 1;
    
    GIE = gieBit;           // restore interrupts

    while(EECON1bits.WR == 1);

    EECON1bits.WREN = 0;
    EECON1bits.EEPGD = 1;
}

// *****************************************************************************

uint8_t EEPROM_ReadByte(uint16_t address)
{
    EECON1bits.CFGS = 0;  // access the program memory/EEPROM
    EECON1bits.EEPGD = 0; // access the EEPROM

    EEADRH = address >> 8;
    EEADR = address;

    /* Initiate read. Data is available on the next instruction cycle */
    EECON1bits.RD = 1;
    
    EECON1bits.EEPGD = 1;

    return EEDATA;
}

// *****************************************************************************

void EEPROM_WriteData(uint16_t address, uint8_t *data, uint8_t size)
{
    if((address + size) > 0x400)
        return;
    
    EECON1bits.CFGS = 0;  // access the program memory/EEPROM
    EECON1bits.EEPGD = 0; // access the EEPROM

    EECON1bits.WREN = 1;    // enable EEPROM write
    uint8_t gieBit = GIE;   // store global interrupt status
    GIE = 0;                // disable interrupts

    /* Initiate EEPROM write sequence then wait for WR bit to clear. This 
    sequence must be done for every byte. */
    while(size > 0)
    {
        EEADRH = address >> 8;
        EEADR = address;
        EEDATA = *data;

        EECON2 = 0x55;
        EECON2 = 0xAA;
        EECON1bits.WR = 1;

        while(EECON1bits.WR == 1);
        address++;
        data++;
        size--;
    }

    GIE = gieBit;
    EECON1bits.WREN = 0;
    EECON1bits.EEPGD = 1;
}

// *****************************************************************************

void EEPROM_ReadData(uint16_t address, uint8_t *retData, uint8_t size)
{
    if((address + size) > 0x400)
        return;

    EECON1bits.CFGS = 0;  // access the program memory/EEPROM
    EECON1bits.EEPGD = 0; // access the EEPROM

    /* Initiate read */
    while(size > 0)
    {
        EEADRH = address >> 8;
        EEADR = address;
        EECON1bits.RD = 1;
        while(EECON1bits.RD == 1);
        *retData = EEDATA;
        address++;
        retData++;
        size--;
    }

    EECON1bits.EEPGD = 1;
}

/*
 End of File
 */