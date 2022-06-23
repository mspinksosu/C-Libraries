/*******************************************************************************
 * Title:
 * 
 * Author: Matthew Spinks
 * 
 * File: interrupt_manager.c
 * 
 * Description:
 * 
 * ****************************************************************************/

#include "interrupt_manager.h"
#include "mcc.h"

void (*EUSART_Transmit_ISR)(void);
void (*EUSART_Receive_ISR)(void);

void interrupt INTERRUPT_InterruptManager (void)
{
   // interrupt handler
    if(PIE3bits.TXIE == 1 && PIR3bits.TXIF == 1)
    {
        EUSART_Transmit_ISR();
    }
    else if(PIE3bits.RCIE == 1 && PIR3bits.RCIF == 1)
    {
        EUSART_Receive_ISR();
    }
    else
    {
        //Unhandled Interrupt
    }
}

void Set_EUSART_Transmit_ISR(void (*Function)(void))
{
    EUSART_Transmit_ISR = Function;
}

void Set_EUSART_Receive_ISR(void (*Function)(void))
{
    EUSART_Receive_ISR = Function;
}

/**
 End of File
*/