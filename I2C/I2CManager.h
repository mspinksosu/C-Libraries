/***************************************************************************//**
 * @brief I2C Manager Header (Non-Processor Specific)
 * 
 * @file I2CManager.h
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 2/27/25   Original creation
 * @date 4/25/25   Refactored
 * 
 * @details
 *      @todo details
 *      @todo Add more details about data transfer object buffer used in the 
 * I2C target device as well. Also, should I make a version that does not 
 * use the DT object?
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

#include "II2CTarget.h"
#include "II2C.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

typedef enum I2CManagerTransferStateTag // @todo transfer state. Haven't decided if I want this or not yet
{
    I2C_MANAGER_TRANSFER_STATE_UNKNOWN = 0,
    I2C_MANAGER_TRANSFER_STATE_READY,
    I2C_MANAGER_TRANSFER_STATE_IN_PROGRESS,
    I2C_MANAGER_TRANSFER_STATE_FINISHED,
    I2C_MANAGER_TRANSFER_STATE_ERROR,
} I2CManagerTransferState;

typedef enum I2CManagerTransferErrorTag
{
    I2C_MANAGER_TRANSFER_ERROR_NONE = 0,
    I2C_MANAGER_TRANSFER_ERROR_UNKOWN,
    I2C_MANAGER_TRANSFER_ERROR_TX,
    I2C_MANAGER_TRANSFER_ERROR_RX,
    // add more as needed
} I2CManagerTransferError;

/* @todo not sure if I want to include state in status report */
typedef struct I2CManagerTransferStatusTag
{
    I2CManagerTransferError error;
    // I2CTransferState state;
    bool isReadType;
    uint8_t *ptrArray;
    uint16_t sizeOfArray;
    uint16_t numOfBytesTransferred;
} I2CManagerTransferStatus;

typedef struct I2CManager_NodeTag I2CManager_Node;

struct I2CManager_NodeTag
{
    I2CManager_Node *next;
    I2CTarget *i2cDevice;
};

/* @todo decide if I actually want an I2C manager error state
or just make it an event a go back to idle - MS */
typedef enum I2CManagerStateTag
{
    I2C_MANAGER_STATE_IDLE = 0,
    I2C_MANAGER_STATE_TRANSFER_IN_PROGRESS,
    I2C_MANAGER_STATE_ERROR, // @todo I2C manager errors
} I2CManagerState;

typedef enum I2CSignalTag
{
    I2C_MANAGER_SIG_ENTER = 1,
    I2C_MANAGER_SIG_EXIT,
    I2C_MANAGER_SIG_BEGIN_TRANSFER,
    I2C_MANAGER_SIG_BUS_IDLE_EVENT,
    I2C_MANAGER_SIG_ACK_RECEIVED,
    I2C_MANAGER_SIG_NACK_RECEIVED,
    I2C_MANAGER_SIG_DATA_RECEIVED,
    I2C_MANAGER_SIG_SEND_STOP,
    I2C_MANAGER_SIG_SEND_RESTART,
    I2C_MANAGER_SIG_TIMEOUT,
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

} I2CEvent;

typedef struct I2CManagerTag I2CManager; // forward declaration

// This is the function pointer type for the state machine functions
typedef void (*I2CFSMState)(I2CManager *self, I2CEvent *e);

struct I2CManagerTag
{
    I2C *peripheral;
    I2CManager_Node *endOfList; // circular linked list
    I2CManager_Node *currentNode;

    struct {
        bool isReadType;
        uint8_t *ptrDataArray;
        uint16_t length;
    } currentDataTransfer;

    bool currentDataTransferFinished;
    uint16_t writeCount;
    uint16_t readCount;
    I2CManagerTransferStatus finishedTransferReport;

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
 * // @todo description of struct members
 * 
 */

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Function Prototypes *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/* @todo finish Doxygen */

void I2CManager_Init(I2CManager *self, I2C *peripheral, uint32_t tickRateNs);

void I2CManager_AddDevice(I2CManager *self, I2CManager_Node *device, I2CTarget *target);

void I2CManager_RemoveDevice(I2CManager *self, I2CManager_Node *device);

void I2CManager_Process(I2CManager *self);

void I2CManager_Enable(I2CManager *self);

void I2CManager_Disable(I2CManager *self);

bool I2CManager_IsBusy(I2CManager *self);

I2CManagerState I2CManager_GetState(I2CManager *self);

void I2CManager_GetCurrentDevice(I2CManager *self, I2CTarget *retDevice);

/* @todo decide how I want to implement I2C manager transfer status - MS */
void I2CManager_GetDataTransferStatus(I2CManager *self, I2CManagerTransferStatus *retTransferStatus);

#endif /* I2C_MANAGER_H */
