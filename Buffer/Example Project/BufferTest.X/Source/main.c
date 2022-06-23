/*******************************************************************************
 * Title: Buffer Test
 * 
 * Author: Matthew Spinks
 * 
 * File: main.c
 * 
 * Description:
 *      A project to experiment with creating a ring buffer interface.
 *      This project also uses a UART interface.
 * 
 * ****************************************************************************/

#include "../mcc_generated_files/mcc.h"
#include "UART.h"
#include "Buffer.h"

// ***** Defines ***************************************************************

#define RX_BUFF_SIZE    32
#define TX_BUFF_SIZE    32

// ***** Function Prototypes ***************************************************

void TransmitChar(uint8_t);

// ***** Global Variables ******************************************************

// Create two arrays to hold the data
uint8_t rxArray[RX_BUFF_SIZE];
uint8_t txArray[TX_BUFF_SIZE];

// Create two buffers objects
Buffer rxBuffer;
Buffer txBuffer;

// *****************************************************************************

void main(void)
{
    // initialize the device
    SYSTEM_Initialize();

    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();

// ----- Initialize ------------------------------------------------------------
        
    uint8_t receivedData;
    
    // Set function pointers for interrupt_manager -> UART Interface
    Set_EUSART_Receive_ISR(UARTReceiveInterrupt);
    Set_EUSART_Transmit_ISR(UARTTransmitFinished);
    
    /* This time around I'm going to implement the receive interrupt directly
     * here in main. In the past, I used a callback function and handled it
     *  inside a related c file. In either case, whichever file the receive
     * interrupt function is in must interface with both the UART and the 
     * buffer. If it is in a different c file, create function pointers to
     * get the data from the interrupt to the buffer and vice versa. */
    
    // Call the normal init function which has the default overwrite 
    // set to false.
    Buffer_Init(&rxBuffer, rxArray, RX_BUFF_SIZE);
    Buffer_Init(&txBuffer, txArray, TX_BUFF_SIZE);
    
    while (1)
    {
// ----- Main Loop -------------------------------------------------------------
        
        // Test loop back
        if(Buffer_IsNotEmpty(&rxBuffer))
        {
            receivedData = Buffer_ReadChar(&rxBuffer);
            
            // loop back
            TransmitChar(receivedData);
        }
        
    } // end main loop
    
} // end main

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** UART Interface Functions ********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

// ----- UART Receive Interrupt ------------------------------------------------
    
void UARTReceiveInterrupt()
{
    uint8_t receivedChar = UARTGetReceivedChar();
    Buffer_WriteChar(&rxBuffer, receivedChar);
}

// ----- UART Transmit Interrupt -----------------------------------------------

void UARTTransmitFinished()
{
    uint8_t dataToSend;
    
    // Is there more data in the TX buffer to send?
    if(Buffer_IsNotEmpty(&txBuffer))
    {
        dataToSend = Buffer_ReadChar(&txBuffer);
        UARTTransmitChar(dataToSend); // Place in the tx register
    }
    else
    {
        UARTTransmitDisable(); // Disable the transmit interrupt
    }
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Local Functions *****************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

// I only have one UART right now. I may eventually expand the UART 
// interface to be like the buffer interface.

void TransmitChar(uint8_t dataToSend)
{
    // This is just a helper function to automate the process of placing 
    // data in the buffer and preparing it to be sent.
    
    // Wait for space in the buffer. Since my transmit is interrupt driven
    // using an empty while-loop will work just fine. If it is not interrupt 
    // driven, then you should include code in the wait loop that will transmit 
    // one byte over the UART if the buffer is full.
    UARTTransmitEnable();
    
    while(Buffer_IsFull(&txBuffer));
    
    UARTTransmitDisable();
    
    Buffer_WriteChar(&txBuffer, dataToSend);
    
    UARTTransmitEnable(); // Enable the transmit interrupt
}
/**
 End of File
*/