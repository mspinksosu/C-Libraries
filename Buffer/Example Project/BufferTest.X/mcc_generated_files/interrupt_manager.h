/*******************************************************************************
 * Title:
 * 
 * Author: Matthew Spinks
 * 
 * File: interrupt_manager.h
 * 
 * Description:
 * 
 * ****************************************************************************/

#ifndef INTERRUPT_MANAGER_H
#define INTERRUPT_MANAGER_H

#define INTERRUPT_GlobalInterruptEnable() (INTCONbits.GIE = 1)
#define INTERRUPT_GlobalInterruptDisable() (INTCONbits.GIE = 0)
#define INTERRUPT_PeripheralInterruptEnable() (INTCONbits.PEIE = 1)
#define INTERRUPT_PeripheralInterruptDisable() (INTCONbits.PEIE = 0)

void interrupt INTERRUPT_InterruptManager(void);

void Set_EUSART_Transmit_ISR(void (*Function)(void));
void Set_EUSART_Receive_ISR(void (*Function)(void));

#endif  // INTERRUPT_MANAGER_H
/**
 End of File
*/