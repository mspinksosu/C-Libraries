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

#ifndef I2CMANAGER_H
#define I2CMANAGER_H

#include "TargetDevice.h"
#include "II2C.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

typedef enum I2CManagerTransferErrorTag
{
    I2CMANAGER_TRANSFER_ERROR_NONE = 0,
    I2CMANAGER_TRANSFER_ERROR_UNKOWN,
    I2CMANAGER_TRANSFER_ERROR_BUS_COLLISION,
    I2CMANAGER_TRANSFER_ERROR_START,
    I2CMANAGER_TRANSFER_ERROR_ADDRESS,
    I2CMANAGER_TRANSFER_ERROR_WRITE,
    I2CMANAGER_TRANSFER_ERROR_READ,
    I2CMANAGER_TRANSFER_ERROR_FLAGRANT,
} I2CManagerTransferError;

/* @todo My updated TargetDevice transfer status now covers most of this type. 
I may remove this entirely, or break it into parts. */
typedef struct I2CManagerTransferStatusTag
{
    I2CManagerTransferError error;
    bool isReadType;
    uint8_t *ptrArray;
    uint16_t sizeOfArray;
    uint16_t numBytesTransferred;
} I2CManagerTransferStatus;

typedef struct I2CManager_NodeTag I2CManager_Node;

struct I2CManager_NodeTag
{
    I2CManager_Node *next;
    TargetDevice *i2cDevice;
    uint8_t targetAddress7Bit; // 7-bit address, right justified 
};

typedef enum I2CManagerStateTag
{
    I2CMANAGER_STATE_IDLE = 0,
    I2CMANAGER_STATE_TRANSFER_IN_PROGRESS,
} I2CManagerState;

typedef enum I2CSignalTag
{
    I2CMANAGER_SIG_ENTER = 1,
    I2CMANAGER_SIG_EXIT,
    I2CMANAGER_SIG_BEGIN_TRANSFER,
    I2CMANAGER_SIG_BUS_IDLE_EVENT,
    I2CMANAGER_SIG_ACK_RECEIVED,
    I2CMANAGER_SIG_NACK_RECEIVED,
    I2CMANAGER_SIG_DATA_RECEIVED,
    I2CMANAGER_SIG_SEND_STOP,
    I2CMANAGER_SIG_SEND_RESTART,
    I2CMANAGER_SIG_RETRY_TIMER_EXPIRED,
    I2CMANAGER_SIG_RETRY_LIMIT_REACHED,
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

typedef struct I2CEventTag I2CEvent; // forward declaration

typedef struct I2CManagerTag I2CManager; // forward declaration

/* function pointer type for the state machine functions */
typedef void (*I2CFSMState)(I2CManager *self, const I2CEvent *e);

/* callback function pointer. The context pointer will point to the device that 
was being processed at the time. */
typedef void (*I2CManagerCallback)(I2CManagerTransferError error, I2CManager *context, TargetDevice *targetContext);

struct I2CEventTag
{
    I2CSignal sig;
    I2CFSMState callingState;
};

struct I2CManagerTag
{
    I2C *peripheral;
    I2CManager_Node *endOfList; // circular linked list
    I2CManager_Node *currentNode;

    DataTransfer currentDataTransfer;

    bool currentDataTransferFinished;
    I2CManagerTransferError currentDataTransferError;
    uint16_t writeCount;
    uint16_t readCount;
    I2CManagerTransferStatus finishedTransferReport;

    I2CFSMState fsmState;
    I2CTimer fsmTimer;
    uint8_t fsmRepeatCount;
    uint8_t fsmRepeatLimit;
    uint32_t fsmLongTimeoutPeriod;
    uint32_t fsmShortTimeoutPeriod;

    I2CManagerStatusBits statusBits;
    I2CManagerState managerState;
    I2CState peripheralState;

    I2CManagerCallback transferErrorCallback;

    /* The SCL pin is currently used for the bus clear function. 
    The SDA pin is for future use if needed. */
    void (*SetSDAPinDirection)(bool setOutput);
    void (*SetSCLPinDirection)(bool setOutput);
    void (*SetSDAPinLevel)(bool setHigh);
    void (*SetSCLPinLevel)(bool setHigh);
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

void I2CManager_AddDevice(I2CManager *self, I2CManager_Node *device, TargetDevice *target, uint8_t targetAddress7Bit);

void I2CManager_RemoveDevice(I2CManager *self, I2CManager_Node *device);

void I2CManager_Process(I2CManager *self);

void I2CManager_Enable(I2CManager *self);

void I2CManager_Disable(I2CManager *self);

bool I2CManager_IsBusy(I2CManager *self);

I2CManagerState I2CManager_GetState(I2CManager *self);

void I2CManager_GetCurrentDevice(I2CManager *self, TargetDevice *retDevice);

uint8_t I2CManager_GetCurrentDeviceAddress(I2CManager *self);

/* @todo decide how I want to implement I2C manager transfer status - MS */
/* @todo should I add a get transfer finished function, or use status? - MS */
void I2CManager_GetDataTransferStatus(I2CManager *self, I2CManagerTransferStatus *retTransferStatus);

I2CManagerTransferError I2CManager_GetTransferError(I2CManager *self);

/* @todo add more callbacks for transfer finished etc. */
void I2CManager_SetTransferErrorCallback(I2CManager *self, I2CManagerCallback Function);

void I2CManager_BusClear(I2CManager *self);

void I2CManager_SetSDAPinDirectionFunc(I2CManager *self, void(*Function)(bool setOutput));

void I2CManager_SetSCLPinDirectionFunc(I2CManager *self, void(*Function)(bool setOutput));

void I2CManager_SetSDAPinLevelFunc(I2CManager *self, void(*Function)(bool setHigh));

void I2CManager_SetSCLPinLevelFunc(I2CManager *self, void(*Function)(bool setHigh));

#endif /* I2CMANAGER_H */
