/***************************************************************************//**
 * @brief UART 1 Implementation Header (Non-Processor Specific)
 * 
 * @author Matthew Spinks
 * 
 * @date 12/2/14   Original creation
 * @date 2/4/22    Modified
 * @date 3/5/22    Changed to use function table and match new interface
 * @date 6/12/22   Changed compute baud rate function
 * @date 6/25/22   Changed received callback function prototype
 * @date 7/31/22   Added handler for pending transmit interrupt
 * 
 * @file UART1.h
 * 
 * @details
 *      TODO
 * 
 ******************************************************************************/

#ifndef UART1_H
#define UART1_H

#include "IUART.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

extern UARTInterface UART1_FunctionTable;

/** 
 * Description of struct
 * 
 * member1      description of variable member1
 * 
 */

// ***** Function Prototypes ***************************************************


// ----- Interface Functions ---------------------------------------------------

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

void UART1_PendingEventHandler(void); // TODO experiment

void UART1_SetTransmitFinishedCallback(void (*Function)(void));

void UART1_SetReceivedDataCallback(void (*Function)(uint8_t (*CallToGetData)(void)));

void UART1_SetIsCTSPinLowFunc(bool (*Function)(void));

void UART1_SetRTSPinFunc(void (*Function)(bool setPinHigh));

#endif  /* UART1_H */
