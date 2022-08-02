/***************************************************************************//**
 * @brief UART 2 Implementation Header (Non-Processor Specific)
 * 
 * @author Matthew Spinks
 * 
 * @date 12/2/14   Original creation
 * @date 2/4/22    Modified
 * @date 3/5/22    Changed to use function table and match new interface
 * @date 6/12/22   Changed compute baud rate function
 * 
 * @file UART2.h
 * 
 * @details
 *      TODO
 * 
 ******************************************************************************/

#ifndef UART2_H
#define UART2_H

#include "IUART.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/* Class specific variables */
extern UARTInterface UART2_FunctionTable;

/** 
 * Description of struct
 * 
 * member1      description of variable member1
 * 
 */

// ***** Function Prototypes ***************************************************


// ----- Interface Functions ---------------------------------------------------

uint32_t UART2_ComputeBRGValue(uint32_t desiredBaudRate, uint32_t pclkInHz);

void UART2_Init(UARTInitType *params);

void UART2_ReceivedDataEvent(void);

uint8_t UART2_GetReceivedByte(void);

bool UART2_IsReceiveRegisterFull(void);

void UART2_ReceiveEnable(void);

void UART2_ReceiveDisable(void);

void UART2_TransmitFinishedEvent(void);

void UART2_TransmitByte(uint8_t dataToSend);

bool UART2_IsTransmitRegisterEmpty(void);

void UART2_TransmitEnable(void);

void UART2_TransmitDisable(void);

void UART1_PendingEventHandler(void); // TODO experiment

void UART2_SetTransmitFinishedCallback(void (*Function)(void));

void UART2_SetReceivedDataCallback(void (*Function)(uint8_t (*CallToGetData)(void)));

void UART2_SetIsCTSPinLowFunc(bool (*Function)(void));

void UART2_SetRTSPinFunc(void (*Function)(bool setPinHigh));

#endif  /* UART2_H */
