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

#define I2CMANAGER_TIMEOUT_PERIOD_US 500 // desired timeout period in us
#define I2CMANAGER_REPEAT_LIMIT 5

/* @note The size of the data request buffer should be one more than the 
amount that you would like the slave to be able to hold at once. I would 
suggest a minimum size of 3. That is enough to hold one write data request, 
followed by one read data request. - MS */
#define I2CSLAVE_DR_BUFFER_SIZE 3 // @todo change name to DT?

// ***** Global Variables ******************************************************

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

typedef enum I2CTransferErrorTag
{
    I2C_TRANSFER_ERROR_NONE = 0,
    I2C_TRANSFER_ERROR_UNKOWN,
    I2C_TRANSFER_ERROR_TX,
    I2C_TRANSFER_ERROR_RX,
    // add more as needed
} I2CTransferError;

typedef struct I2CDataTransferTag // might move this to II2C.h
{
    I2CTransferType transferType;
    uint8_t *data;
    uint16_t length;
} I2CDataTransfer;

typedef struct I2CDataTransferStatusTag
{
    I2CTransferError error;
    I2CTransferState state;
    I2CTransferType transferType;
    uint8_t *ptrArray;
    uint16_t sizeOfArray;
    uint16_t numOfBytesTransferred;
} I2CDataTransferStatus;

typedef struct I2CSlaveTag I2CSlave; // forward declaration

// @todo decided if I want to keep the old callback function pointers
/* callback function pointer. The context is so that you can know which of
your I2C devices initiated the callback. */
// typedef void (*I2CSlaveCallbackFunc)(I2CSlave *i2cSlaveContext);

/* @todo decide if I want to make a base class or not */
struct I2CSlaveTag
{
    // I2CSlave *super; // include the base class first
    I2CSlave *next;
    uint8_t slaveAddress7Bit; // 7-bit address, right justified
    struct
    {
        I2CDataTransfer buffer[I2CSLAVE_DR_BUFFER_SIZE];
        uint16_t head;
        uint16_t tail;
        bool transferFinished;
    } private;
    I2CDataTransferStatus finishedTransferReport;
    // @todo transfer finished callback function pointer or transmit and isTransmitReady
};

typedef struct I2CTimerTag
{
    uint32_t period;
    uint32_t count;
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
            unsigned repeatedStartSent  :1; // repeated start has been performed
            unsigned                    :1;
        };
        uint8_t all;
    };
} I2CManagerStatusBits;

// @todo decided if I want to use a manager state or keep using old status bits
typedef enum I2CManagerStateTag
{
    I2C_MANAGER_STATE_IDLE = 0,
    I2C_MANAGER_STATE_TRANSFER_IN_PROGRESS,
    I2C_MANAGER_STATE_ERROR, // @todo add errors etc.
} I2CManagerState;

typedef enum I2CSignalTag
{
    I2C_SIG_ENTER = 1,
    I2C_SIG_BEGIN_TRANSFER,
    I2C_SIG_BUS_IDLE_EVENT,
    I2C_SIG_ACK_RECEIVED,
    I2C_SIG_NACK_RECEIVED,
    I2C_SIG_DATA_RECEIVED,
    I2C_SIG_TIMEOUT,
    I2C_SIG_SEND_STOP,
    I2C_SIG_SEND_RESTART,
} I2CSignal;

typedef struct I2CEventTag
{
    I2CSignal sig;
    uint8_t slaveAddressPlusRW; // 7-bit address + R/W bit
    bool masterRead;            // go into read state after sending address
} I2CEvent;

typedef struct I2CManagerTag I2CManager; // forward declaration

// This is the function pointer type for the state machine functions
typedef void (*I2CFSMState)(I2CManager *self, I2CEvent *e);

struct I2CManagerTag
{
    I2C *peripheral;
    I2CSlave *endOfList; // circular linked list
    I2CSlave *currentDevice;
    I2CDataTransfer currentDataTransfer;
    bool currentDataTransferFinished;
    uint16_t writeCount;
    uint16_t readCount;

    I2CFSMState fsmState;
    uint8_t fsmRepeatCount;
    I2CTimer fsmTimer;

    I2CManagerStatusBits statusBits; // @todo might replace this with state
    I2CManagerState managerState; // @todo might use slave state instead
    I2CState peripheralState;
};

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

void I2CManager_Init(I2CManager *self, I2C *peripheral, uint32_t tickRateUs);

void I2CManager_AddSlave(I2CManager *self, I2CSlave *slave);

void I2CManager_Process(I2CManager *self);

// @todo do I want a separate I2CManager_Tick function? Or just run the timers at the same speed as Process?

void I2CManager_Enable(I2CManager *self);

void I2CManager_Disable(I2CManager *self);

bool I2CManager_IsIdle(I2CManager *self); // @todo change to IsBusy to match I2C function

void I2CManager_GetState(I2CManager *self); // @todo get state

void I2CManager_GetCurrentDevice(I2CManager *self, I2CSlave *retDevice); // @todo get current device

// slave functions

void I2CSlave_Init(I2CSlave *self, uint8_t slaveAddress);

bool I2CSlave_IsReadyForDataTransfer(I2CSlave *self);

// @todo Do I want to add a GetDataTransferBufferCount?
// If so, also add a get buffer size for future. Makes manager more flexible in performing repeated starts versus start then stop

void I2CSlave_DataTransfer(I2CSlave *self, I2CTransferType writeOrRead, uint8_t *data, uint16_t length);

bool I2CSlave_IsDataTransferFinished(I2CSlave *self);

// @todo get slave state?

void I2CSlave_GetDataTransferStatus(I2CSlave *self, I2CDataTransferStatus *retTransferStatus);

// I2CSlave_DataTransferFinishedCallback
// I2CSlave_WriteTransferFinishedCallback
// I2CSlave_ReadTransferFinishedCallback

#endif /* I2C_MANAGER_H */