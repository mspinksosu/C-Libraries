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
 *      // TODO details
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

// ***** Global Variables ******************************************************

// ---- New state machine stuff ------------------------------------------------

typedef enum I2CSignalTag
{
    I2C_SIG_BEGIN_TRANSFER = 1,
    I2C_SIG_BUS_IDLE_EVENT,
    I2C_SIG_ACK_RECEIVED,
    I2C_SIG_DATA_RECEIVED,
    I2C_SIG_TIMEOUT,
} I2CSignal;

typedef struct I2CEventTag
{
    I2CSignal sig;
    uint8_t slaveAddress;

    // @todo update extra info for events
    bool generateRepeatedStart; // at the end of transfer
    bool repeatedStart;         // repeated start has been performed
    bool masterRead;            // go into read state after sending address
} I2CEvent;


// -----------------------------------------------------------------------------

// @todo I2C device slave state enum maybe not needed for the FSM
// @todo or replace with new state type for FSM
typedef enum I2CSlaveStateTag
{
    I2C_STATE_IDLE = 0,
    I2C_STATE_RQ_START,
    I2C_STATE_SEND_BYTE,
    I2C_STATE_RECEIVE_BYTE
} I2CSlaveState;

typedef struct I2CSlaveTag I2CSlave;

// @todo decided if I want to keep the old callback function pointers
/* callback function pointer. The context is so that you can know which of
your I2C devices initiated the callback. */
// typedef void (*I2CObjectCallbackFunc)(I2CObject *i2cObjectContext);

struct I2CSlaveTag
{
    I2CSlave *next;
    I2CManager *manager;
    uint8_t slaveAddress; // 7-bit address, right justified
    uint8_t *writeBuffer;
    uint8_t *readBuffer;
    uint16_t numBytesToSend;
    uint16_t numBytesToRead;
    uint16_t writeCount; // @todo might go back to my old method of making a private struct
    uint16_t readCount;
    I2CSlaveState state;
    bool transferFinished;

    // @todo decided if I want to keep the old callback function pointers
    // I2CObjectCallbackFunc transmitFinishedCallback;
    // I2CObjectCallbackFunc receivedFinishedCallback;
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

// @todo move this to .c file?
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

typedef struct I2CManagerTag
{
    I2C *peripheral;
    I2CSlave *endOfList; // circular linked list
    I2CSlave *device;
    bool busy;
    // @todo is the busy flag needed?
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

/* TODO finish Doxygen */

void I2C_Manager_Create(I2CManager *self, I2C *peripheral);

void I2C_Manager_AddSlave(I2CManager *self, I2CSlave *slave, uint8_t *writeBuffer, uint8_t *readBuffer);

bool I2C_Manager_IsDeviceBusy(I2CSlave *self);

void I2C_Manager_BeginTransfer(I2CSlave *self, uint16_t numBytesToSend, uint16_t numBytesToRead);

bool I2C_Manager_IsTransferFinished(I2CSlave *self);

void I2C_Manager_Process(I2CManager *self);

void I2C_Manager_Enable(I2CManager *self);

void I2C_Manager_Disable(I2CManager *self);

void I2C_Manager_GetState(I2CManager *self); // @todo return state

#endif  /* I2C_MANAGER_H */