/***************************************************************************//**
 * @brief UART 1 Implementation Header (Non-Processor Specific)
 * 
 * @author Matthew Spinks
 * 
 * @date 12/2/14   Original creation
 * @date 2/4/22    Modified
 * @date 3/5/22    Changed to use function table and match new interface
 * @date 6/12/22   Changed compute baud rate function
 * @date 7/31/22   Added handler for pending transmit interrupt
 * 
 * @file UART1.h
 * 
 * @details
 *      A header for a UART peripheral that implements the IUART interface. 
 * Each function here must be implemented. The external interface variable, 
 * FunctionTable is declared and defined in the .c file for your implementation 
 * as well. From there, you will set each one of its pointers to one of the 
 * functions listed here.
 * 
 * Whichever file does the initalization for this UART will need to include 
 * this header file to gain access to the function table. The implementation 
 * can vary across different processors. All that matters is that the functions
 * listed in the table point to the correct functions for your implementation.
 * 
 * See IUART.h for a description of what each function should do.
 * 
 ******************************************************************************/

#ifndef UART1_H
#define UART1_H

#include "IUART.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/* Declare and define this variable in your implementation's .c file */
extern UARTInterface UART1_FunctionTable;


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

uint32_t UART1_ComputeBRGValue(uint32_t desiredBaudRate, uint32_t pclkInHz);

void UART1_Init(UARTInitType *params);

void UART1_ReceivedDataEvent(void);

uint8_t UART1_GetReceivedByte(void);

bool UART1_IsReceiveRegisterFull(void);

void UART1_ReceiveEnable(void);

void UART1_ReceiveDisable(void);

void UART1_TransmitFinishedEvent(void);

void UART1_TransmitByte(uint8_t dataToSend);

bool UART1_IsTransmitRegisterEmpty(void);

void UART1_TransmitEnable(void);

void UART1_TransmitDisable(void);

void UART1_PendingEventHandler(void);

void UART1_SetTransmitFinishedCallback(void (*Function)(void));

void UART1_SetReceivedDataCallback(void (*Function)(uint8_t (*CallToGetData)(void)));

void UART1_SetIsCTSPinLowFunc(bool (*Function)(void));

void UART1_SetRTSPinFunc(void (*Function)(bool setPinHigh));

#endif  /* UART1_H */
