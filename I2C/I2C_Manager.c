/***************************************************************************//**
 * @brief I2C Manager (Non-Processor Specific)
 * 
 * @file I2C_Manager.c
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

#include "I2C_Manager.h"
#include <stddef.h> // needed for NULL

// ***** Defines ***************************************************************

#define CircularIncrement(i, size) i == (size - 1) ? 0 : i + 1

// ***** Global Variables ******************************************************

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
    bool generateRepeatedStart; // at the end of transfer
    bool repeatedStartSent;     // repeated start has been performed
    bool masterRead;            // go into read state after sending address
} I2CEvent;

static bool I2CManagerEnabled; // @todo enable/disable

// @todo local function pointers
// void (*I2C1_TransmitFinishedCallback)(void);
// void (*I2C1_ReceiveInterruptCallback)(void);

// states
static void I2CManager_FsmIdle(I2CManager *self, I2CEvent *e);
static void I2CManager_FsmStart(I2CManager *self, I2CEvent *e);
static void I2CManager_FsmWriteAddress(I2CManager *self, I2CEvent *e);
static void I2CManager_FsmWriteData(I2CManager *self, I2CEvent *e);
static void I2CManager_FsmStop(I2CManager *self, I2CEvent *e);
static void I2CManager_FsmRestart(I2CManager *self, I2CEvent *e);
static void I2CManager_FsmReadData(I2CManager *self, I2CEvent *e);

// ***** Static Function Prototypes ********************************************

static void I2CManager_DevicePush(I2CSlave *self, I2CSlave *endOfList);
static void I2CManager_FsmInit(I2CManager *self, uint16_t tickRateInNs, uint16_t timeoutInUs);

// *****************************************************************************

void I2CManager_Create(I2CManager *self, I2C *peripheral)
{
    self->peripheral = peripheral;
    self->endOfList = NULL;
    self->device = NULL;
}

// *****************************************************************************

void I2CManager_AddSlave(I2CManager *self, I2CSlave *slave)
{
    if(self->endOfList == NULL)
    {
        /* Begin with a new list */
        self->endOfList = slave;

        /* Since the list only contains one entry, the "next" pointer will
        also point to itself */
        self->endOfList->next = self->endOfList;
    }
    else
    {
        I2CManager_DevicePush(slave, self->endOfList);
    }
    self->device = self->endOfList->next; // reset the index
}

// *****************************************************************************

void I2CManager_Process(I2CManager *self)
{
    // @todo refactor old PIC32 code

    I2CEvent event = {0};

// ----- Check for Active Timers -----------------------------------------------

    if(self->fsmTimer.flags.start)
    {
        self->fsmTimer.flags.start = 0;
        // self->fsmTimer.count = TIMEOUT_PERIOD_COUNT; // @todo add retry count
        // self->fsmTimer.retryCount = RETRY_COUNT;
        self->fsmTimer.flags.active = 1;
    }

// ----- Update Active Timers --------------------------------------------------

    if(self->fsmTimer.flags.active)
    {
        self->fsmTimer.count--;
        if(self->fsmTimer.count == 0)
        {
            self->fsmTimer.retryCount--;
            if(self->fsmTimer.retryCount == 0)
            {
                self->fsmTimer.flags.active = 0;
                self->fsmTimer.flags.expired = 1;
                self->fsmTimer.flags.retryFinished = 1;
            }
        }
    }

// ----- Check for I2C Events --------------------------------------------------

    if(self->statusBits.sendingStart && !(I2C_GetStartStatus(self->peripheral)))
    {
        self->statusBits.sendingStart = 0;
        event.sig = I2C_SIG_BUS_IDLE_EVENT;
        self->fsmState(&event); // call the current state and pass the event
    }
    if(self->statusBits.sendingRestart && !(I2C_GetStartStatus(self->peripheral)))
    {
        self->statusBits.sendingRestart = 0;
        event.sig = I2C_SIG_BUS_IDLE_EVENT;
        self->fsmState(&event);
    }
    if(self->statusBits.sendingStop && !(I2C_GetStartStatus(self->peripheral)))
    {
        self->statusBits.sendingStop = 0;
        event.sig = I2C_SIG_BUS_IDLE_EVENT;
        self->fsmState(&event);
    }
    if(self->statusBits.sendingAck && !(I2C_GetSendAckStatus(self->peripheral)))
    {
        self->statusBits.sendingAck = 0;
        event.sig = I2C_SIG_BUS_IDLE_EVENT;
        self->fsmState(&event);
    }
    if(self->statusBits.transmitInProgress && I2C_IsTransmitFinished(self->peripheral))
    {
        /* @follow-up In my previous PIC32 library, I had a note to check that 
        the transmit register was completely empty before sending the bus idle 
        event. That was because that PIC would set the flag before the data had 
        finished shifting out. It's possible I was accidentally using the TBF 
        flag instead of the TRSTAT in the original code. It looks like checking 
        the TRSTAT bit should work fine. Need to check on a logic analyzer*/
        if(I2C_IsTransmitRegisterEmpty(self->peripheral))
        {
            self->statusBits.transmitInProgress = 0;
            event.sig = I2C_SIG_BUS_IDLE_EVENT;
            self->fsmState(&event);
        }
    }
    else if(self->statusBits.receiveInProgress && !(I2C_GetReceiveEnableStatus(self->peripheral)))
    {
        if(I2C_IsReceiveRegisterFull())
        {
            self->statusBits.receiveInProgress = 0;
            event.sig = I2C_SIG_DATA_RECEIVED;
            self->fsmState(&event);
        }
    }
    else if(self->fsmTimer.flags.expired)
    {
        self->fsmTimer.flags.expired = 0;
        event.sig = I2C_SIG_TIMEOUT;
        // @todo This is temporary! I removed during troubleshooting with 
        // logic analyzer. I don't think the timeout value is long enough.
        // So I need to do some measurements first
        //self->fsmState(&event); 
    }
    else if(self->fsmTimer.flags.retryFinished)
    {
        // We've sent the event to retry the command multiple times and it
        // has failed
        self->fsmTimer.flags.retryFinished = 0;
        // @todo stop everything
    }
}

// *****************************************************************************

void I2CManager_Enable(I2CManager *self)
{
    if(!I2C_IsEnabled(self->peripheral))
        I2C_Enable(self->peripheral);

    I2CManagerEnabled = true;
}

// *****************************************************************************

void I2CManager_Disable(I2CManager *self)
{
    I2CManagerEnabled = false;
}

// *****************************************************************************

bool I2CManager_IsIdle(I2CManager *self)
{
    if(self->fsmState == I2CManager_FsmIdle)
        return true;
    else
        return false;
}

// *****************************************************************************

void I2CManager_GetState(I2CManager *self)
{
    // @todo manager make enum for states
}

// *****************************************************************************

void I2CSlave_Init(I2CSlave *self, uint8_t slaveAddress)
{
    self->slaveAddress = slaveAddress;
    self->private.head = 0;
    self->private.tail = 0;
    self->private.writeCount = 0;
    self->private.readCount = 0;
    self->private.transferFinished = false;
}

bool I2CSlave_IsReadyForDataTransfer(I2CSlave *self)
{
    // check busy state or buffer full
}

void I2CSlave_DataTransfer(I2CSlave *self, I2CTransferType writeOrRead, uint8_t *data, uint16_t length)
{
    // check if slave is busy already

    uint8_t tempHead = CircularIncrement(self->private.head, I2CSLAVE_DR_BUFFER_SIZE);

    if(tempHead != self->private.txTail)
    {
        // There is space in the buffer
        self->private.buffer[self->private.head].transferType = writeOrRead;
        self->private.buffer[self->private.head].data = data;
        self->private.buffer[self->private.head].length = length;
        self->private.head = tempHead;
        //self->private.bufferIsNotEmpty = true;
    }
}

// *****************************************************************************

bool I2CSlave_IsDataTransferFinished(I2CSlave *self)
{
    return self->private.transferFinished; // @todo remember to clear transferFinished when transmit register is written to. Similar to UART TIF flag
}

// *****************************************************************************

void I2CSlave_GetDataTransferStatus(I2CSlave *self, I2CDataTransferStatus *retTransferStatus)
{

}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Local Functions *****************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

static void I2CManager_DevicePush(I2CSlave *self, I2CSlave *endOfList)
{
    /* Add the new entry to the beginning of the list. Make the "next" pointer
    point to the head */
    self->next = endOfList->next;
    /* Update the beginning of the list to point to the new beginning */
    endOfList->next = self;
}

// *****************************************************************************

static uint8_t I2CSlave_ReadFromDataTransferBuffer(I2CSlave *self, I2CDataRequest *returnDataRequest)
{
    if(self->private.head != self->private.tail)
    {
        // The buffer is not empty
        // @follow-up test simple struct assignment first instead of memcpy just for better readability - MS
        *returnDataRequest = self->private.buffer[self->private.tail];
        self->private.tail = CircularIncrement(self->private.tail, I2CSLAVE_DR_BUFFER_SIZE);
        return 0; // no error
    }
    else
    {
        returnDataRequest->length = 0;
        return 1;
    }
}

// *****************************************************************************

static uint8_t I2CSlave_GetDataTransferBufferCount(I2CSlave *self)
{
    int16_t count = self->private.head - self->private.tail;

    if(count < 0)
    {
        count += I2CSLAVE_DR_BUFFER_SIZE;
    }
    return count;
}

// *****************************************************************************

// @remove old function after re-factoring
void I2CManager_MasterWrite(I2CManager *self, I2CSlave *slave, uint8_t *writeData, uint8_t numBytes)
{
    if(self->fsmState != I2CManager_FsmIdle)
        return;

    slave->writeBuffer = writeData;
    slave->numBytesToSend = numBytes;
    slave->private.writeCount = 0;

    // create an event to give to the state machine
    I2CEvent event;
    event.private.masterRead = false;
    event.private.slaveAddress = slave->slaveAddress << 1;
    event.sig = I2C_SIG_BEGIN_TRANSFER;
    self->fsmState(&event); // call the current state and pass the event
}

// *****************************************************************************

void I2CManager_MasterRead(I2CManager *self, I2CSlave *slave, uint8_t *readData, uint8_t numBytes)
{
    // @todo setup a lock to keep this function from being called if it's
    // already running. Decide if I want to make the return type a bool
    if(self->fsmState != I2CManager_FsmIdle)
        return;

    slave->readBuffer = readData;
    slave->numBytesToRead = numBytes;
    slave->private.readCount = 0;

    // create an event to give to the state machine
    I2CEvent event;
    event.private.masterRead = true;
    event.private.slaveAddress = ((slave->slaveAddress << 1) | 1);
    event.sig = I2C_SIG_BEGIN_TRANSFER;
    self->fsmState(&event); // call the current state and pass the event
}

// *****************************************************************************
// @todo refactor get data function - MS
void I2CManager_GetData(I2CManager *self, uint8_t *numBytesWritten, uint8_t *numBytesRead, I2CSlave *context)
{
    *numBytesWritten = ptrI2CSlave->private.writeCount;
    *numBytesRead = ptrI2CSlave->private.readCount;
    // context = ptrI2CSlave; // @todo get data
}

// *****************************************************************************
// @label old PIC32 FSM code

static void I2CManager_FsmInit(I2CManager *self, uint16_t tickRateInNs, uint16_t timeoutInUs)
{
    self->fsmState = I2CManager_FsmIdle;

    // @todo finish setting up timer using variables given
    self->fsmTimer.period = (uint16_t)TIMEOUT_PERIOD_COUNT;
}

// *****************************************************************************

static void I2CManager_FsmIdle(I2CManager *self, I2CEvent *e)
{
    switch(e->sig)
    {
        case I2C_SIG_BEGIN_TRANSFER:
            if(e->repeatedStartSent)
            {
                // skip the start and go straight to the address
                e->repeatedStartSent = false;
                I2C_TransmitByte(self->peripheral, e->slaveAddress);
                self->fsmTimer.flags.start = 1;
                self->fsmState = I2CManager_FsmWriteAddress;
            }
            else
            {
                I2C1_Start();
                self->fsmTimer.flags.start = 1;
                self->fsmState = I2CManager_FsmStart;
            }
            break;
    }
}

// *****************************************************************************

static void I2CManager_FsmStart(I2CManager *self, I2CEvent *e)
{
    switch(e->sig)
    {
        case I2C_SIG_BUS_IDLE_EVENT:
            self->fsmTimer.flags.active = 0;
            // send slave address
            I2C1_TransmitByte(e->slaveAddress);
            self->fsmTimer.flags.start = 1;
            self->fsmState = I2CManager_FsmWriteAddress;
            break;
    }
}

// *****************************************************************************

static void I2CManager_FsmWriteAddress(I2CManager *self, I2CEvent *e)
{
    switch(e->sig)
    {
        case I2C_SIG_BUS_IDLE_EVENT:
            self->fsmTimer.flags.active = 0;
            if(I2C1_GetAckStatus())
            {
                // address acknowledged.
                if(e->private.masterRead)
                {
                    // prepare to receive byte
                    I2C1_ReceiveEnable();
                    self->fsmState = I2CManager_FsmReadData;
                }
                else
                {
                    // load first data byte
                    I2C1_TransmitByte(ptrI2CSlave->writeBuffer[ptrI2CSlave->private.writeCount]);
                    self->fsmState = I2CManager_FsmWriteData;
                }
                self->fsmTimer.flags.start = 1;
            }
            break;
    }
}

// *****************************************************************************

static void I2CManager_FsmWriteData(I2CManager *self, I2CEvent *e)
{
    switch(e->sig)
    {
        case I2C_SIG_BUS_IDLE_EVENT:
            self->fsmTimer.flags.active = 0;
            if(I2C1_GetAckStatus())
            {
                // data acknowledged. Check if there are more bytes to send
                ptrI2CSlave->private.writeCount++;
                if(ptrI2CSlave->private.writeCount < ptrI2CSlave->numBytesToSend)
                {
                    I2C1_TransmitByte(ptrI2CSlave->writeBuffer[ptrI2CSlave->private.writeCount]);
                    self->fsmTimer.flags.start = 1;
                    // stay in this state and continue sending bytes
                }
                else
                {
                    // We are finished sending bytes. Check to see if we should
                    // generate a repeated start
                    if(e->private.generateRepeatedStart)
                    {
                        I2C1_Restart(); // send repeated start command
                        self->fsmTimer.flags.start = 1;
                        self->fsmState = I2CManager_FsmRestart;
                    }
                    else
                    {
                        I2C1_Stop();
                        self->fsmTimer.flags.start = 1;
                        self->fsmState = I2CManager_FsmStop;
                    }
                }
            }
            break;
    }
}

// *****************************************************************************

static void I2CManager_FsmStop(I2CManager *self, I2CEvent *e)
{
    switch(e->sig)
    {
        case I2C_SIG_BUS_IDLE_EVENT:
            self->fsmTimer.flags.active = 0;
            // Stop is finished.
            self->fsmState = I2CManager_FsmIdle;
            break;
    }
}

// *****************************************************************************

static void I2CManager_FsmRestart(I2CManager *self, I2CEvent *e)
{
    switch(e->sig)
    {
        case I2C_SIG_BUS_IDLE_EVENT:
            self->fsmTimer.flags.active = 0;
            // restart is finished.
            e->private.generateRepeatedStart = false;
            e->private.repeatedStartSent = true;
            self->fsmState = I2CManager_FsmIdle;
            break;
    }
}

// *****************************************************************************

static void I2CManager_FsmReadData(I2CManager *self, I2CEvent *e)
{
     switch(e->sig)
    {
        case I2C_SIG_DATA_RECEIVED:
            self->fsmTimer.flags.active = 0;
            ptrI2CSlave->readBuffer[ptrI2CSlave->private.readCount++] = I2C1_GetReceivedByte();
            if(ptrI2CSlave->private.readCount < ptrI2CSlave->numBytesToRead)
            {
                I2C1_SendAck(true);
            }
            else
            {
                I2C1_SendAck(false); // send NACK on last byte
            }
            self->fsmTimer.flags.start = 1;
            break;
        case I2C_SIG_BUS_IDLE_EVENT:
            self->fsmTimer.flags.active = 0;
            // prepare to receive byte
            if(ptrI2CSlave->private.readCount < ptrI2CSlave->numBytesToRead)
            {
                I2C1_ReceiveEnable();
                self->fsmTimer.flags.start = 1;
            }
            else
            {
                I2C1_Stop();
                self->fsmTimer.flags.start = 1;
                self->fsmState = I2CManager_FsmStop;
            }
            break;
    }
}

/*
 End of File
 */
