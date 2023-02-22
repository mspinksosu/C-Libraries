/***************************************************************************//**
 * @brief EEPROM Library Implementation (PIC18)
 * 
 * @author Matthew Spinks
 * 
 * @date 12/2/14   Original creation
 * @date 2/4/22    Modified
 * 
 * @file Foo_MCU1.c
 * 
 * @details
 *      TODO
 * 
 ******************************************************************************/

#include "EEPROM.h"

/* Include processor specific header files here. If you are using XC-8, it 
should have your chip selection defined in your project settings. Including
xc.h should do the trick. If not, processor header files can be found in the
install path of your compiler: "Install_Path\xc8\v2.20\pic\include\proc\""   */
#include <xc.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************


// ***** Static Function Prototypes ********************************************

/* Put static function prototypes here */


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

    while(EECON1bits.WR == 1);

    GIE = gieBit;
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

/*
 End of File
 */