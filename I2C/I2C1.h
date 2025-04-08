/***************************************************************************//**
 * @brief I2C Implementation Header (Non-Processor Specific)
 * 
 * @file IC21.h
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 10/2/16   Original Creation
 * @date 2/21/22   Added Doxygen
 * @date 3/24/25   Changed to use function table and match new interface
 * 
 * @details
 *      @todo details
 * 
 * @see II2C.h for a description of what each function should do.
 * 
 * @section license License
 * SPDX-FileCopyrightText: © 2016 Matthew Spinks
 * SPDX-License-Identifier: Zlib
 * 
 * This software is released under the Zlib license. You are free alter and
 * redistribute it, but you must not misrepresent the origin of the software.
 * This notice may not be removed. <http://www.zlib.net/zlib_license.html>
 * 
 ******************************************************************************/

#ifndef I2C1_H
#define I2C1_H

#include "II2C.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/* Declare and define this variable in your implementation's .c file */
extern I2CInterface I2C1_FunctionTable;


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/* See II2C.h for a description of what each function should do. */

uint32_t I2C1_ComputeBRGValue(uint32_t desiredBaudRate, uint32_t pclkInHz);

void I2C1_Init(I2CInitType *params);

void I2C1_Enable(void);

void I2C1_Disable(void);

bool I2C1_IsEnabled(void);

void I2C1_ReceivedDataEvent(void);

uint8_t I2C1_GetReceivedByte(void);

bool I2C1_IsReceiveRegisterFull(void);

bool I2C1_IsReceiveUsingInterrupts(void);

void I2C1_ReceiveByte(void);

void I2C1_ReceiveByteCancel(void);

void I2C1_TransmitRegisterEmptyEvent(void);

void I2C1_TransmitByte(uint8_t dataToSend);

bool I2C1_IsTransmitRegisterEmpty(void);

bool I2C1_IsTransmitFinished(void);

bool I2C1_IsTransmitUsingInterrupts(void);

void I2C1_PendingEventHandler(void);
void I2C1_SetTransmitRegisterEmptyCallback(void (*Function)(void));
void I2C1_SetReceivedDataCallback(void (*Function)(uint8_t (*CallToGetData)(void)));

void I2C1_Start(void);

void I2C1_Stop(void);

void I2C1_Restart(void);

void I2C1_SendAck(bool ackOrNack);

bool I2C1_IsBusy(void);

I2CState I2C1_GetState(void);

bool I2C1_GetAckSlaveStatus(void);

#endif /* I2C1_H */
