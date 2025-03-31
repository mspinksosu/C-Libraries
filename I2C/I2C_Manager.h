/***************************************************************************//**
 * @brief I2C Manager Header (Non-Processor Specific)
 * 
 * @file I2C_Manager.h
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 2/27/25   Original creation
 * 
 * @details
 *      // @todo details
 * 
 * @section license License
 * SPDX-FileCopyrightText: © 2025 Matthew Spinks
 * SPDX-License-Identifier: Zlib
 * 
 * This software is released under the Zlib license. You are free alter and
 * redistribute it, but you must not misrepresent the origin of the software.
 * This notice may not be removed. <http://www.zlib.net/zlib_license.html>
 * 
 ******************************************************************************/

#ifndef I2C_MANAGER_H
#define I2C_MANAGER_H

#include "II2C.h"

// ***** Defines ***************************************************************

/* Desired timeout time in ms */
/* @todo original library used a dedicated hardware timer. Might change TIMEOUT_PERIOD
to just use REPEAT_SEND instead */
#define I2C_TIMEOUT_PERIOD_MS 1
#define I2C_REPEAT_SEND_MS 10

/* How many times to retry a command before giving up.
An error flag will be set afterwards */
#define I2C_REPEAT_LIMIT 5

#define I2CSLAVE_DR_BUFFER_SIZE 2

// ***** Global Variables ******************************************************

// typedef struct I2CSlaveTag I2CSlave; // forward declaration

// @todo decided if I want to keep the old callback function pointers
/* callback function pointer. The context is so that you can know which of
your I2C devices initiated the callback. */
// typedef void (*I2CSlaveCallbackFunc)(I2CSlave *i2cSlaveContext);

typedef enum I2CTransferTypeTag
{
    I2C_TRANSFER_TYPE_WRITE = 0,
    I2C_TRANSFER_TYPE_READ
} I2CTransferType;

typedef enum I2CTransferStateTag
{
    I2C_TRANSFER_STATE_UNKNOWN = 0,
    I2C_TRANSFER_STATE_IDLE,
    I2C_TRANSFER_STATE_BUSY,
    I2C_TRANSFER_STATE_ERROR,
    // add more as needed
} I2CTransferState;

typedef enum I2CTransferStateTag
{
    I2C_TRANSFER_ERROR_UNKNOWN = 0,
    I2C_TRANSFER_ERROR_TX,
    I2C_TRANSFER_ERROR_RX,
    // add more as needed
} I2CTransferError;

typedef struct I2CDataRequestTag
{
    I2CTransferType transferType;
    uint8_t *data;
    uint16_t length;
} I2CDataRequest;

typedef struct I2CDataTransferStatusTag
{
    I2CTransferError error;
    I2CTransferState state;
    I2CTransferType transferType;
    uint8_t *ptrArray;
    uint16_t sizeOfArray;
    uint16_t numOfBytesTransferred;
} I2CDataTransferStatus;

// typedef struct I2CSlaveTag // Decide if I want to use a single I2CSlave class or not.
// {
//     void *instance;
//     uint8_t slaveAddress; // 7-bit address, right justified

//     /* add IsTransmitByteReady function pointer?, TransmitByte function 
//     pointer similar to wireless module library? */
// } I2CSlave;

typedef struct I2CSlaveTag I2CSlave; // forward declaration

struct I2CSlaveTag
{
    // I2CSlave *super; // include the base class first
    I2CSlave *next;
    uint8_t slaveAddress;
    struct
    {
        I2CDataRequest buffer[I2CSLAVE_DR_BUFFER_SIZE];
        uint16_t head;
        uint16_t tail;
        uint16_t writeCount;
        uint16_t readCount;
        bool transferFinished;
    } private;
    // @todo transfer finished callback function pointer or transmit and isTransmitReady
};

typedef struct I2CTimerTag
{
    uint16_t period;
    uint16_t count;
    union {
        struct {
            unsigned start   :1;
            unsigned active  :1;
            unsigned expired :1;
            unsigned         :5;
        };
        uint8_t all;
    } flags;
} I2CTimer;

typedef struct I2CManagerStatusBitsTag
{
    union {
        struct {
            unsigned sendingStart       :1;
            unsigned sendingRestart     :1;
            unsigned sendingStop        :1;
            unsigned sendingAck         :1;
            unsigned receiveInProgress  :1;
            unsigned transmitInProgress :1;
            unsigned                    :2;
        };
        uint8_t all;
    };
} I2CManagerStatusBits;

// This is the function pointer type for the state machine functions
typedef void (*I2CState)(I2CEvent *e);

typedef struct I2CManagerTag
{
    I2C *peripheral;
    I2CSlave *endOfList; // circular linked list
    I2CSlave *device; // current device being processed
    I2CState state;
    I2CTimer fsmTimer;
    I2CTimer waitTimer;
    I2CManagerStatusBits statusBits;
} I2CManager;

/**
 * Description of struct members:
 * // TODO description
 * 
 */

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Function Prototypes *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/* @todo finish Doxygen */

void I2CManager_Create(I2CManager *self, I2C *peripheral);

void I2CManager_AddSlave(I2CManager *self, I2CSlave *slave);

void I2CManager_Process(I2CManager *self);

void I2CManager_Enable(I2CManager *self);

void I2CManager_Disable(I2CManager *self);

bool I2CManager_IsIdle(I2CManager *self);

void I2CManager_GetState(I2CManager *self);

// old I2C Manager functions from PIC32 @remove or re-factor
void I2CManager_MasterWrite(I2CManager *self, I2CSlave *slave, uint8_t *writeData, uint8_t numBytes);
void I2CManager_MasterRead(I2CManager *self, I2CSlave *slave, uint8_t *readData, uint8_t numBytes);
void I2CManager_GetData(uint8_t *numBytesWritten, uint8_t *numBytesRead, I2CSlave *context);

// slave functions // @todo might move slave functions to a new file

void I2CSlave_Init(I2CSlave *self, uint8_t slaveAddress);

bool I2CSlave_IsReadyForDataTransfer(I2CSlave *self);

void I2CSlave_DataTransfer(I2CSlave *self, I2CTransferType writeOrRead, uint8_t *data, uint16_t length);

bool I2CSlave_IsDataTransferFinished(I2CSlave *self);

void I2CSlave_GetDataTransferStatus(I2CSlave *self, I2CDataTransferStatus *retTransferStatus);

// @todo add generic get data function? similar to wmod GetRxPacket?

// I2CSlave_DataTransferFinishedCallback
// I2CSlave_WriteTransferFinishedCallback
// I2CSlave_ReadTransferFinishedCallback

#endif /* I2C_MANAGER_H */