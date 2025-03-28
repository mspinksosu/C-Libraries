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

// ***** Global Variables ******************************************************

// ---- New state machine stuff ------------------------------------------------

typedef enum I2CSignalTag
{
    I2C_SIG_BEGIN_TRANSFER = 1,
    I2C_SIG_BUS_IDLE_EVENT,
    I2C_SIG_ACK_RECEIVED,
    I2C_SIG_NACK_RECEIVED,
    I2C_SIG_DATA_RECEIVED,
    I2C_SIG_TIMEOUT,
} I2CSignal;

typedef struct I2CEventTag
{
    I2CSignal sig;
    uint8_t slaveAddress;

    // @todo refactor and update with extra info for events
    bool generateRepeatedStart; // at the end of transfer
    bool repeatedStart;         // repeated start has been performed
    bool masterRead;            // go into read state after sending address
} I2CEvent;


// -----------------------------------------------------------------------------

// typedef struct I2CSlaveTag I2CSlave; // forward declaration

// @todo decided if I want to keep the old callback function pointers
/* callback function pointer. The context is so that you can know which of
your I2C devices initiated the callback. */
// typedef void (*I2CSlaveCallbackFunc)(I2CSlave *i2cSlaveContext);

// experiment
typedef struct I2CDataRequestTag
{
    bool readTypeRequest; // true = read, false = write
    uint16_t length;
    uint8_t *data;
} I2CDataRequest;

// experiment
// typedef struct I2CManagerDataRequestTag
// {
//     I2CDataRequest *dataRequest;
//     I2CSlave *slave;
// } I2CManagerDataRequest;

typedef struct I2CSlaveTag
{
    void *instance;
    uint8_t slaveAddress; // 7-bit address, right justified

    struct
    {
        // uint8_t *txBuffer;
        // uint16_t txBufferSize;
        // Try using a fixed size for now
        I2CDataRequest txBuffer[2];
        uint16_t txHead;
        uint16_t txTail;

        // uint8_t *rxBuffer;
        // uint16_t rxBufferSize;
        I2CDataRequest rxBuffer[2];
        uint16_t rxHead;
        uint16_t rxTail;

        uint16_t writeCount;
        uint16_t readCount;
    } private;

    // @todo IsTransmitByteReady function pointer?, TransmitByte function pointer.
} I2CSlave;

typedef struct I2CSlave_NodeTag I2CSlave_Node; // forward declaration

struct I2CSlave_NodeTag
{
    I2CSlave *super; // include the base class first

    I2CSlave_Node *next;
    bool transferFinished;
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

// This is the function pointer type for the state machine functions
typedef void (*I2CState)(I2CEvent *e);

typedef struct I2CManagerTag
{
    I2C *peripheral;
    I2CSlave *endOfList; // circular linked list
    I2CSlave *device;
    I2CState state;
    I2CTimer fsmTimer;
    I2CTimer waitTimer;
    bool isBusBusy; // @todo is the busy flag needed?
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

/* TODO finish Doxygen */

// @todo change names to I2CManager_Create etc.?

void I2C_Manager_Create(I2CManager *self, I2C *peripheral);

// @todo add create sub class

void I2C_Manager_AddSlave(I2CManager *self, I2CSlave_Node *slave, uint8_t *writeBuffer, uint8_t *readBuffer);

bool I2C_Manager_IsDeviceBusy(I2CSlave_Node *self);

void I2C_Manager_BeginTransfer(I2CSlave_Node *self, uint16_t numBytesToSend, uint16_t numBytesToRead);

bool I2C_Manager_IsTransferFinished(I2CSlave_Node *self);

void I2C_Manager_Process(I2CManager *self);

void I2C_Manager_Enable(I2CManager *self);

void I2C_Manager_Disable(I2CManager *self);

void I2C_Manager_GetState(I2CManager *self); // @todo return state

#endif  /* I2C_MANAGER_H */