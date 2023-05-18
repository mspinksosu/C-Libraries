/***************************************************************************//**
 * @brief I2C 1 Implementation Header (Non-Processor Specific)
 * 
 * @file IC21.h 
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 10/2/16   Original Creation
 * @date 2/21/22   Added Doxygen
 * 
 * @details
 *      TODO
 * 
 * @section license License
 * SPDX-FileCopyrightText: © 2016 Matthew Spinks
 * SPDX-License-Identifier: Zlib
 * 
 * This software is released under the Zlib license. You are free alter and
 * redistribute it, but you must not misrepresent the origin of the software.
 * This notice may not be removed. <http://www.zlib.net/zlib_license.html>
 * 
*******************************************************************************/

#ifndef I2C1_H
#define	I2C1_H

#include "I2C.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************


// ***** Function Prototypes ***************************************************


// ----- State Machine Functions -----------------------------------------------

void I2C1_FsmInit(uint16_t tickRateInNs, uint16_t timeoutInUs);
void I2C1_FsmProcess(void);
void I2C1_FsmMasterWrite(I2CObject *self, uint8_t *writeData, uint8_t numBytes, bool repeatedStart);
void I2C1_FsmMasterRead(I2CObject *self, uint8_t *readData, uint8_t numBytes);

bool I2C1_FsmIsIdle(void);
//bool I2C1_FsmIsTransferFinished(void); // TODO add check for any errors
void I2C1_FsmGetData(uint8_t *numBytesWritten, uint8_t *numBytesRead, I2CObject *context);

// ----- Peripheral Functions --------------------------------------------------

bool I2C1_InitWithFrequencies(float pbclkInMHz, uint16_t baudInKHz);
bool I2C1_InitWithBRGValue(uint32_t brgValue);

bool I2C1_IsBusy(void); // is the actual peripheral bus busy
void I2C1_Start(void);
void I2C1_Stop(void);
void I2C1_Restart(void);
void I2C1_SendAck(bool ack);
bool I2C1_GetAckStatus(void);

void I2C1_ReceiveEnable(void);
void I2C1_ReceiveDisable(void);
bool I2C1_IsReceivedDataAvailable(void);
uint8_t I2C1_GetReceivedByte(void);

void I2C1_TransmitEnable(void);
void I2C1_TransmitDisable(void);
void I2C1_TransmitByte(uint8_t dataToSend);
bool I2C1_IsTransmitRegisterFull(void);

void I2C1_ReceiveInterrupt(void);
void I2C1_TransmitFinishedInterrupt(void);

void I2C1_SetReceiveInterruptCallback(void (*Function)(void));
void I2C1_SetTransmitFinishedCallback(void (*Function)(void));

#endif	/* I2C1_H */
