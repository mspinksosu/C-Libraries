/***************************************************************************//**
 * @brief SPI 1 Implementation Header (Non-Processor Specific)
 * 
 * @file SPI3.h
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 9/9/22    Original creation
 * 
 * @details
 *      // TODO details
 * 
 * @section license License
 * SPDX-FileCopyrightText: © 2023 Matthew Spinks
 * SPDX-License-Identifier: Zlib
 * 
 * This software is released under the Zlib license. You are free alter and
 * redistribute it, but you must not misrepresent the origin of the software.
 * This notice may not be removed. <http://www.zlib.net/zlib_license.html>
 * 
 ******************************************************************************/

#ifndef SPI3_H
#define SPI3_H

#include "ISPI.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/* Declare and define this variable in your implementation's .c file */
extern SPIInterface SPI3_FunctionTable;


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

void SPI3_Init(SPIInitType *params);

void SPI3_Enable(void);

void SPI3_Disable(void);

void SPI3_ReceivedDataEvent(void);

uint8_t SPI3_GetReceivedByte(void);

bool SPI3_IsReceiveRegisterFull(void);

void SPI3_TransmitRegisterEmptyEvent(void);

void SPI3_TransmitByte(uint8_t dataToSend);

bool SPI3_IsTransmitRegisterEmpty(void);

bool SPI3_IsTransmitFinished(void);

SPIStatusBits SPI3_GetStatus(void);

void SPI3_PendingEventHandler(void);

void SPI3_SetTransmitRegisterEmptyCallback(void (*Function)(void));

void SPI3_SetReceivedDataCallback(void (*Function)(uint8_t (*CallToGetData)(void)));

void SPI3_SetSSPinFunc(void (*Function)(bool setPinHigh));

#endif  /* SPI3_H */