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

/* @note The size of the data transfer buffer should be one more than the 
amount that you would like the slave to be able to hold at once. I would 
suggest a minimum size of 3. That is enough to hold one write data request, 
followed by one read data request. - MS */
#define I2CSLAVE_DT_BUFFER_SIZE 3

// ***** Global Variables ******************************************************

typedef enum I2CTransferTypeTag
{
    I2C_TRANSFER_TYPE_WRITE = 0,
    I2C_TRANSFER_TYPE_READ
} I2CTransferType;

typedef enum I2CTransferStateTag // @todo transfer state. Haven't decided if I want this or not yet
{
    I2C_TRANSFER_STATE_UNKNOWN = 0,
    I2C_TRANSFER_STATE_READY,
    I2C_TRANSFER_STATE_IN_PROGRESS,
    I2C_TRANSFER_STATE_FINISHED,
    I2C_TRANSFER_STATE_ERROR,
} I2CTransferState;

typedef enum I2CTransferErrorTag
{
    I2C_TRANSFER_ERROR_NONE = 0,
    I2C_TRANSFER_ERROR_UNKOWN,
    I2C_TRANSFER_ERROR_TX,
    I2C_TRANSFER_ERROR_RX,
    // add more as needed
} I2CTransferError;

typedef enum I2CSlaveStateTag
{
    I2C_SLAVE_STATE_IDLE = 0,
    I2C_SLAVE_STATE_TRANSFER_IN_PROGRESS,
    I2C_SLAVE_STATE_ERROR,
} I2CSlaveState;

typedef struct I2CDataTransferTag // might move this to II2C.h
{
    I2CTransferType transferType;
    uint8_t *data;
    uint16_t length;
} I2CDataTransfer;

/* @todo not sure if I want to include state in status report */
typedef struct I2CDataTransferStatusTag
{
    I2CTransferError error;
    // I2CTransferState state;
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
    I2CSlaveState state;

    struct
    {
        I2CDataTransfer buffer[I2CSLAVE_DT_BUFFER_SIZE];
        uint16_t head;
        uint16_t tail;
        uint16_t count;
        bool transferStartedEventFlag;
        bool transferFinishedEventFlag;
    } private;

    I2CDataTransferStatus finishedTransferReport;
};

// @todo decided if I want to use a manager state or keep status bits
typedef enum I2CManagerStateTag
{
    I2C_MANAGER_STATE_IDLE = 0,
    I2C_MANAGER_STATE_TRANSFER_IN_PROGRESS,
    I2C_MANAGER_STATE_ERROR, // @todo add errors etc.
} I2CManagerState;

typedef enum I2CSignalTag
{
    I2C_SIG_ENTER = 1,
    I2C_SIG_EXIT,
    I2C_SIG_BEGIN_TRANSFER,
    I2C_SIG_BUS_IDLE_EVENT,
    I2C_SIG_ACK_RECEIVED,
    I2C_SIG_NACK_RECEIVED,
    I2C_SIG_DATA_RECEIVED,
    I2C_SIG_SEND_STOP,
    I2C_SIG_SEND_RESTART,
    I2C_SIG_TIMEOUT,
    I2C_SIG_BUS_COLLISION, // @todo bus collision, etc.
} I2CSignal;

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

typedef struct I2CEventTag
{
    I2CSignal sig;
    // uint8_t slaveAddressPlusRW; // 7-bit address + R/W bit // @remove later
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
    I2CTimer fsmTimer;
    uint8_t fsmRepeatCount;
    uint8_t fsmRepeatLimit;
    I2CFSMState fsmTimerStateEnterCallback;
    uint32_t fsmLongTimeoutPeriod;
    uint32_t fsmShortTimeoutPeriod; // @todo add short timer for start and stop?

    I2CManagerStatusBits statusBits;
    I2CManagerState managerState;
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

void I2CManager_Init(I2CManager *self, I2C *peripheral, uint32_t tickRateNs);

void I2CManager_AddSlave(I2CManager *self, I2CSlave *slave);

void I2CManager_Tick(I2CManager *self);

void I2CManager_Enable(I2CManager *self);

void I2CManager_Disable(I2CManager *self);

bool I2CManager_IsBusy(I2CManager *self);

I2CManagerState I2CManager_GetState(I2CManager *self);

void I2CManager_GetCurrentDevice(I2CManager *self, I2CSlave *retDevice);

/* slave functions */

void I2CSlave_Init(I2CSlave *self, uint8_t slaveAddress);

uint8_t I2CSlave_GetDataTransferBufferCount(I2CSlave *self);

bool I2CSlave_IsDataTransferBufferFull(I2CSlave *self);

bool I2CSlave_IsDataTransferBufferNotEmpty(I2CSlave *self);

uint8_t I2CSlave_GetDataTransferBufferSize(I2CSlave *self); // for future use with external buffer - MS

void I2CSlave_WriteToDataTransferBuffer(I2CSlave *self, I2CTransferType writeOrRead, uint8_t *data, uint16_t length);

uint8_t I2CSlave_ReadFromDataTransferBuffer(I2CSlave *self, I2CDataTransfer *returnDataTransfer);

bool I2CSlave_GetDataTransferStartedEvent(I2CSlave *self);

void I2CSlave_ClearDataTransferStartedEventFlag(I2CSlave *self);

bool I2CSlave_GetDataTransferFinishedEvent(I2CSlave *self);

void I2CSlave_ClearDataTransferFinishedEventFlag(I2CSlave *self);

void I2CSlave_GetDataTransferStatus(I2CSlave *self, I2CDataTransferStatus *retTransferStatus); // @todo transfer status

I2CSlaveState I2CSlave_GetState(I2CSlave *self);

// I2CSlave_DataTransferFinishedCallback
// I2CSlave_WriteTransferFinishedCallback
// I2CSlave_ReadTransferFinishedCallback

#endif /* I2C_MANAGER_H */