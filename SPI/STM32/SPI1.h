/***************************************************************************//**
 * @brief SPI Implementation Header (Non-Processor Specific)
 * 
 * @file SPI1.h
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 9/9/22    Original creation
 * 
 * @details
 *      // TODO details
 * 
 * @section license License
 * SPDX-FileCopyrightText: © 2022 Matthew Spinks
 * SPDX-License-Identifier: Zlib
 * 
 * This software is released under the Zlib license. You are free alter and
 * redistribute it, but you must not misrepresent the origin of the software.
 * This notice may not be removed. <http://www.zlib.net/zlib_license.html>
 * 
 ******************************************************************************/

#ifndef SPI1_H
#define SPI1_H

#include "ISPI.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/* Declare and define this variable in your implementation's .c file */
extern SPIInterface SPI1_FunctionTable;


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

void SPI1_Init(SPIInitType *params);

void SPI1_Enable(void);

void SPI1_Disable(void);

void SPI1_ReceivedDataEvent(void);

uint8_t SPI1_GetReceivedByte(void);

bool SPI1_IsReceiveRegisterFull(void);

void SPI1_TransmitRegisterEmptyEvent(void);

void SPI1_TransmitByte(uint8_t dataToSend);

bool SPI1_IsTransmitRegisterEmpty(void);

bool SPI1_IsTransmitFinished(void);

SPIStatusBits SPI1_GetStatus(void);

void SPI1_PendingEventHandler(void);

void SPI1_SetTransmitRegisterEmptyCallback(void (*Function)(void));

void SPI1_SetReceivedDataCallback(void (*Function)(uint8_t (*CallToGetData)(void)));

void SPI1_SetSSPinFunc(void (*Function)(bool setPinHigh));

#endif  /* SPI1_H */