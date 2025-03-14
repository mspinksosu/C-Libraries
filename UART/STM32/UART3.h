/***************************************************************************//**
 * @brief UART Implementation Header (Non-Processor Specific)
 * 
 * @file UART3.h
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 12/2/14   Original creation
 * @date 2/4/22    Modified
 * @date 3/5/22    Changed to use function table and match new interface
 * @date 6/12/22   Changed compute baud rate function
 * @date 7/31/22   Added handler for pending transmit interrupt
 * 
 * @details
 *      A header for a UART peripheral that implements the IUART interface. 
 * Each function here must be implemented. The external interface variable, 
 * FunctionTable is declared and defined in the .c file for your implementation 
 * as well. From there, you will set each one of its pointers to one of the 
 * functions listed here.
 * 
 * Whichever file does the initialization for this UART will need to include 
 * this header file to gain access to the function table. The implementation 
 * can vary across different processors. All that matters is that the functions
 * listed in the table point to the correct functions for your implementation.
 * 
 * @see IUART.h for a description of what each function should do.
 * 
 * @section license License
 * SPDX-FileCopyrightText: © 2014 Matthew Spinks
 * SPDX-License-Identifier: Zlib
 * 
 * This software is released under the Zlib license. You are free alter and
 * redistribute it, but you must not misrepresent the origin of the software.
 * This notice may not be removed. <http://www.zlib.net/zlib_license.html>
 * 
 ******************************************************************************/

#ifndef UART3_H
#define UART3_H

#include "IUART.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/* Declare and define this variable in your implementation's .c file */
extern UARTInterface UART3_FunctionTable;


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/* See IUART.h for a description of what each function should do. */

uint32_t UART3_ComputeBRGValue(uint32_t desiredBaudRate, uint32_t pclkInHz);

void UART3_Init(UARTInitType *params);

void UART3_ReceivedDataEvent(void);

uint8_t UART3_GetReceivedByte(void);

bool UART3_IsReceiveRegisterFull(void);

bool UART3_IsReceiveUsingInterrupts(void);

void UART3_ReceiveEnable(void);

void UART3_ReceiveDisable(void);

void UART3_TransmitRegisterEmptyEvent(void);

void UART3_TransmitByte(uint8_t dataToSend);

bool UART3_IsTransmitRegisterEmpty(void);

bool UART3_IsTransmitFinished(void);

bool UART3_IsTransmitUsingInterrupts(void);

void UART3_TransmitEnable(void);

void UART3_TransmitDisable(void);

void UART3_PendingEventHandler(void);

void UART3_SetTransmitRegisterEmptyCallback(void (*Function)(void));

void UART3_SetReceivedDataCallback(void (*Function)(uint8_t (*CallToGetData)(void)));

void UART3_SetIsCTSPinLowFunc(bool (*Function)(void));

void UART3_SetRTSPinFunc(void (*Function)(bool setPinHigh));

#endif  /* UART3_H */
