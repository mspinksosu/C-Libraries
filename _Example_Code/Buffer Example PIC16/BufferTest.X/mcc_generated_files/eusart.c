
#include "eusart.h"

void EUSART_Initialize(void)
{
    // disable interrupts before changing states
    PIE3bits.RCIE = 0;
    PIE3bits.TXIE = 0;

    // Set the EUSART module to the options selected in the user interface.

    // ABDOVF no_overflow; SCKP Non-Inverted; BRG16 16bit_generator; WUE disabled; ABDEN disabled; 
    BAUD1CON = 0x08;

    // SPEN enabled; RX9 8-bit; CREN enabled; ADDEN disabled; SREN disabled; 
    RC1STA = 0x90;

    // TX9 8-bit; TX9D 0; SENDB sync_break_complete; TXEN enabled; SYNC asynchronous; BRGH hi_speed; CSRC slave; 
    TX1STA = 0x24;

    // Baud Rate = 115200; SP1BRGL 34; 
    SP1BRGL = 0x22;

    // Baud Rate = 115200; SP1BRGH 0; 
    SP1BRGH = 0x00;

    // enable receive interrupt
    PIE3bits.RCIE = 1;
}

/**
  End of File
*/
