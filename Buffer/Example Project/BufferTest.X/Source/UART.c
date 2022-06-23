/*******************************************************************************
 * Title: Basic UART
 * 
 * Author: Matthew Spinks
 * 
 * File: UART.c
 * 
 * Description:
 * 
 * ****************************************************************************/

#include <xc.h>
#include "UART.h"

// ***** Defines ***************************************************************

#define RX_REG  RC1REG  // the receive register
#define TX_REG  TX1REG  // the the transmit register

// ***** Function Prototypes ***************************************************


// ***** Global Variables ******************************************************

// local function pointers
void (*UARTTransmitFinishedCallback)(void);
void (*UARTReceiveInterruptCallback)(void);

// *****************************************************************************

void UARTInit(void)
{
    // I'm going to let the Code Configurator handle the initialization
    // this time.
    
//    // disable interrupts before changing states
//    PIE3bits.RCIE = 0;
//    PIE3bits.TXIE = 0;
//
//    // Set the EUSART module to the options selected in the user interface.
//
//    // ABDOVF no_overflow; SCKP Non-Inverted; BRG16 16bit_generator; WUE disabled; ABDEN disabled; 
//    BAUD1CON = 0x08;
//
//    // SPEN enabled; RX9 8-bit; CREN enabled; ADDEN disabled; SREN disabled; 
//    RC1STA = 0x90;
//
//    // TX9 8-bit; TX9D 0; SENDB sync_break_complete; TXEN enabled; SYNC asynchronous; BRGH hi_speed; CSRC slave; 
//    TX1STA = 0x24;
//
//    // Baud Rate = 115200; SP1BRGL 34; 
//    SP1BRGL = 0x22;
//
//    // Baud Rate = 115200; SP1BRGH 0; 
//    SP1BRGH = 0x00;
//
//    // enable receive interrupt
//    PIE3bits.RCIE = 1;
}

// ----- UART Transmit  --------------------------------------------------------

//void UARTTransmitFinished()
//{
//    if(UARTTransmitFinishedCallback)
//    {
//        UARTTransmitFinishedCallback();
//    }
//}

void UARTTransmitChar(uint8_t data)
{
    // For this function I will be making use of interrupts to transmit my data.
    // Writing to TX_REG will clear the TXIF flag after one instruction cycle
    TX_REG = data;
    
    // TODO make function call
    PIE3bits.TXIE = 1; // enable transmit interrupt
}

void UARTTransmitEnable()
{
    PIE3bits.TXIE = 1;
}

void UARTTransmitDisable()
{
    PIE3bits.TXIE = 0;
}

// ----- UART Receive ----------------------------------------------------------

// The receive interrupt and the actual receive char are two separate 
// functions. This makes things a more flexible when interfacing to some sort
// of interrupt manager.

//void UARTReceiveInterrupt()
//{
//    if(1 == RC1STAbits.OERR)
//    {
//        // EUSART error - restart
//        RC1STAbits.CREN = 0;
//        RC1STAbits.CREN = 1;
//    }
//    
//    if(UARTReceiveInterruptCallback)
//    {
//        UARTReceiveInterruptCallback();
//    }
//}

uint8_t UARTGetReceivedChar()
{
    return RX_REG;
}

void UARTReceiveEnable()
{
    PIE3bits.RCIE = 1;
}

void UARTReceiveDisable()
{
    PIE3bits.RCIE = 0;
}

// ----- Set UART Transmit -----------------------------------------------------

void SetUARTTransmitFinishedCallback(void (*Function)(void))
{
    UARTTransmitFinishedCallback = Function;
}

// ----- Set UART Receive ------------------------------------------------------

void SetUARTReceiveInterruptCallback(void (*Function)(void))
{
    UARTReceiveInterruptCallback = Function;
}
/**
 End of File
*/