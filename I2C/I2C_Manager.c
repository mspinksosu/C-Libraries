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
    uint8_t slaveAddressPlusRW; // 7-bit address + R/W bit
    bool generateRepeatedStart; // at the end of transfer
    bool repeatedStartSent;     // repeated start has been performed
    bool masterRead;            // go into read state after sending address
} I2CEvent;

static bool I2CManagerEnabled; // @todo enable/disable

// @todo function pointers
// void (*I2C_TransmitFinishedCallback)(void);
// void (*I2C_ReceiveInterruptCallback)(void);

// states
static void I2CManager_FsmIdle(I2CManager *self, I2CEvent *e);
static void I2CManager_FsmStart(I2CManager *self, I2CEvent *e); // @todo add enter event
static void I2CManager_FsmWriteAddress(I2CManager *self, I2CEvent *e);
static void I2CManager_FsmWriteData(I2CManager *self, I2CEvent *e);
static void I2CManager_FsmStop(I2CManager *self, I2CEvent *e);
static void I2CManager_FsmRestart(I2CManager *self, I2CEvent *e);
static void I2CManager_FsmReadData(I2CManager *self, I2CEvent *e);

// ***** Static Function Prototypes ********************************************

static void I2CManager_DevicePush(I2CSlave *self, I2CSlave *endOfList);
static uint8_t I2CSlave_ReadFromDataTransferBuffer(I2CSlave *self, I2CDataTransfer *returnDataTransfer);
static uint8_t I2CSlave_GetDataTransferBufferCount(I2CSlave *self);
static void I2CSlave_DataTransferFinished(I2CSlave *self, I2CDataTransferStatus *report);
static void I2CManager_DataTransfer(I2CManager *self, I2CSlave *slave, I2CDataTransfer *data);
static void I2CManager_GetFinishedTransferReport(I2CManager *self, I2CDataTransferStatus *retReport);
static void I2CManager_FsmInit(I2CManager *self, uint16_t tickRateInNs, uint16_t timeoutInUs);

// *****************************************************************************

void I2CManager_Init(I2CManager *self, I2C *peripheral)
{
    self->peripheral = peripheral;
    self->endOfList = NULL;
    self->currentDevice = NULL;
    self->currentDataTransfer.length = 0;
    self->writeCount = 0;
    self->readCount = 0;
    // @todo init state and error variables

    I2CManager_FsmInit(self, 0, 0); // @todo FSM init
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
    self->currentDevice = self->endOfList->next; // reset the index
}

// *****************************************************************************

void I2CManager_Process(I2CManager *self)
{
    I2CEvent event = {0};

    /* Check if we need to start a timer */
    if(self->fsmTimer.flags.start)
    {
        self->fsmTimer.flags.start = 0;
        self->fsmTimer.count = self->fsmTimer.period;
        self->fsmRepeatCount = 0;
        self->fsmTimer.flags.active = 1;
    }

    /* Check active timers */
    if(self->fsmTimer.flags.active)
    {
        self->fsmTimer.count--;
        if(self->fsmTimer.count == 0)
        {
            self->fsmTimer.flags.expired = 1;
            self->fsmTimer.flags.active = 0;
        }
    }

// ----- Check for I2C Events --------------------------------------------------

    I2CState currentPeripheralState = I2C_GetState(self->peripheral);

    /* @todo Decide if I want to keep the old status bits code or replace it 
    with just the state. I could keep the status bits and make sure the events 
    are explicit. Or just make a simple check if the previous state was not 
    equal to the current state. */

    if(self->peripheralState != I2C_STATE_BUS_IDLE && currentPeripheralState == I2C_STATE_BUS_IDLE)
    {
        if(self->statusBits.sendingStart)
        {
            self->statusBits.sendingStart = 0;
            event.sig = I2C_SIG_BUS_IDLE_EVENT;
            self->fsmState(&event); // call the current state and pass the event
        }
        if(self->statusBits.sendingRestart)
        {
            self->statusBits.sendingRestart = 0;
            event.sig = I2C_SIG_BUS_IDLE_EVENT;
            self->fsmState(&event);
        }
        if(self->statusBits.sendingStop)
        {
            self->statusBits.sendingStop = 0;
            event.sig = I2C_SIG_BUS_IDLE_EVENT;
            self->fsmState(&event);
        }
        if(self->statusBits.sendingAck)
        {
            self->statusBits.sendingAck = 0;
            event.sig = I2C_SIG_BUS_IDLE_EVENT;
            self->fsmState(&event);
        }
    }

    if(self->statusBits.transmitInProgress && currentPeripheralState == I2C_STATE_BUS_IDLE && 
        self->peripheralState == I2C_STATE_MASTER_TRANSMITTING)
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
    else if(self->statusBits.receiveInProgress && currentPeripheralState == I2C_STATE_BUS_IDLE && 
        self->peripheralState == I2C_STATE_SENDING_START)
    {
        if(I2C_IsReceiveRegisterFull(self->peripheral))
        {
            self->statusBits.receiveInProgress = 0;
            event.sig = I2C_SIG_DATA_RECEIVED;
            self->fsmState(&event);
        }
    }

    if(self->fsmTimer.flags.expired)
    {
        self->fsmTimer.flags.expired = 0;
        self->fsmRepeatCount++;
        event.sig = I2C_SIG_TIMEOUT;
        // @todo add error
        if(self->fsmRepeatCount > I2CMANAGER_REPEAT_LIMIT)
        {
            self->fsmRepeatCount = 0;
            self->fsmTimer.flags.start = 0;
            self->fsmTimer.flags.active = 0;
            self->fsmTimer.flags.expired = 0;
            // @todo stop, change signal, go back to idle state. Set error status
        }
        /* @follow-up old code had this commented out. I had written that the 
        timeout value was not long enough. - MS */
        //self->fsmState(&event); 
    }

    self->peripheralState = currentPeripheralState;

// ----- Process slave list ----------------------------------------------------

    // @todo go through list and process each slave's data requests
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

void I2CSlave_Init(I2CSlave *self, uint8_t slaveAddress7Bit)
{
    self->slaveAddress7Bit = slaveAddress7Bit;
    self->private.head = 0;
    self->private.tail = 0;
    self->private.transferFinished = false;
}

// *****************************************************************************

bool I2CSlave_IsReadyForDataTransfer(I2CSlave *self)
{
    if(I2CSlave_GetDataTransferBufferCount(self) > 0)
        return true;
    else
        return false;
}

// *****************************************************************************

void I2CSlave_DataTransfer(I2CSlave *self, I2CTransferType writeOrRead, uint8_t *data, uint16_t length)
{
    // @todo check if slave is busy already

    uint8_t tempHead = CircularIncrement(self->private.head, I2CSLAVE_DR_BUFFER_SIZE);

    if(tempHead != self->private.tail)
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
    return self->private.transferFinished;
}

// *****************************************************************************

/* @todo not sure how I want to implement this yet. Should the status reflect 
on going changes during the data transfer, such as the current state? Or should 
I just use it as a end of data transfer report? - MS */
void I2CSlave_GetDataTransferStatus(I2CSlave *self, I2CDataTransferStatus *retTransferStatus)
{
    /* @todo should the FSM write the state to the current slave device? - MS */
    retTransferStatus->state = I2C_TRANSFER_STATE_UNKNOWN; // @todo I2C states
    retTransferStatus->error = I2C_TRANSFER_ERROR_NONE; // @todo I2C error codes
    
    /* @note cannot get current transfer type without linking to I2C manager... 
    Maybe have the manager write to the report? - MS */
    retTransferStatus->transferType = self->finishedTransferReport.transferType;
    retTransferStatus->ptrArray = self->finishedTransferReport.ptrArray;
    retTransferStatus->sizeOfArray = self->finishedTransferReport.sizeOfArray;
    retTransferStatus->numOfBytesTransferred = self->finishedTransferReport.numOfBytesTransferred;
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

static uint8_t I2CSlave_ReadFromDataTransferBuffer(I2CSlave *self, I2CDataTransfer *returnDataTransfer)
{
    if(self->private.head != self->private.tail)
    {
        /* The buffer is not empty. Get the data from the buffer to be 
        processed and clear the transfer finished flag */
        // @follow-up test simple struct assignment first instead of memcpy just for better readability - MS
        *returnDataTransfer = self->private.buffer[self->private.tail];
        self->private.tail = CircularIncrement(self->private.tail, I2CSLAVE_DR_BUFFER_SIZE);
        self->private.transferFinished = false;
        return 0; // no error
    }
    else
    {
        returnDataTransfer->length = 0;
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

static void I2CSlave_DataTransferFinished(I2CSlave *self, I2CDataTransferStatus *report)
{
    self->private.transferFinished = true;
    // @follow-up try a simple struct assignment first instead of memcpy just for better readability - MS
    self->finishedTransferReport = *report;
}

// *****************************************************************************

static void I2CManager_DataTransfer(I2CManager *self, I2CSlave *slave, I2CDataTransfer *data)
{
    if(self->fsmState != I2CManager_FsmIdle)
        return;

    // get the data
    // @follow-up try a simple struct assignment first instead of memcpy just for better readability - MS
    self->currentDataTransfer = *data;

    // create an event to give to the state machine
    I2CEvent event = {0};
    if(data->transferType == I2C_TRANSFER_TYPE_WRITE)
    {
        event.masterRead = false;
        event.slaveAddressPlusRW = slave->slaveAddress7Bit << 1;
        self->writeCount = 0;
    }
    else
    {
        event.masterRead = true;
        event.slaveAddressPlusRW = ((slave->slaveAddress7Bit << 1) | 1);
        self->readCount = 0;
    }
    event.sig = I2C_SIG_BEGIN_TRANSFER;
    self->fsmState(&event); // call the current state and pass the event
}

// *****************************************************************************

static void I2CManager_GetFinishedTransferReport(I2CManager *self, I2CDataTransferStatus *retReport)
{
    retReport->error = I2C_TRANSFER_ERROR_NONE; // @todo I2C error codes
    retReport->state = I2C_TRANSFER_STATE_IDLE;
    retReport->transferType = self->currentDataTransfer.transferType;
    retReport->ptrArray = self->currentDataTransfer.data;
    retReport->sizeOfArray = self->currentDataTransfer.length;
    if(retReport->transferType == I2C_TRANSFER_TYPE_WRITE)
        retReport->numOfBytesTransferred = self->writeCount;
    else
        retReport->numOfBytesTransferred = self->readCount;
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
                I2C_TransmitByte(self->peripheral, e->slaveAddressPlusRW);
                self->fsmTimer.flags.start = 1;
                self->fsmState = I2CManager_FsmWriteAddress;
            }
            else
            {
                I2C_Start(self->peripheral);
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
            I2C_TransmitByte(self->peripheral, e->slaveAddressPlusRW);
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
            if(I2C_GetAckStatus())
            {
                if(self->currentDataTransfer.transferType == I2C_TRANSFER_TYPE_READ)
                {
                    /* Prepare to receive byte */
                    I2C_ReceiveByte(self->peripheral);
                    self->fsmState = I2CManager_FsmReadData;
                }
                else
                {
                    /* Load first data byte */
                    I2C_TransmitByte(self->peripheral, self->currentDataTransfer.data[self->writeCount]);
                    self->fsmState = I2CManager_FsmWriteData;
                }
                self->fsmTimer.flags.start = 1;
            }
            break;
        // @todo add timeout event
    }
}

// *****************************************************************************

static void I2CManager_FsmWriteData(I2CManager *self, I2CEvent *e)
{
    switch(e->sig)
    {
        case I2C_SIG_BUS_IDLE_EVENT:
            if(self->currentDataTransfer.transferType == I2C_TRANSFER_TYPE_WRITE) // @follow-up is this extra check needed? - MS
            {
                if(I2C_GetAckStatus())
                {
                    self->fsmTimer.flags.active = 0;
                    /* Data acknowledged. Check if there are more bytes to send */
                    self->writeCount++;
                    if(self->writeCount < self->currentDataTransfer.length)
                    {
                        /* Stay in this state and continue sending bytes */
                        I2C_TransmitByte(self->peripheral, self->currentDataTransfer.data[self->writeCount]);
                        self->fsmTimer.flags.start = 1;
                    }
                    else
                    {
                        /* We are finished with the transfer. Check to see if there 
                        is another transfer ready and generate a repeated start. */
                        if(e->generateRepeatedStart) // @todo replace with slave buffer check
                        {
                            I2C_Restart(self->peripheral);
                            self->fsmTimer.flags.start = 1;
                            self->fsmState = I2CManager_FsmRestart;
                        }
                        else
                        {
                            I2C_Stop(self->peripheral);
                            self->fsmTimer.flags.start = 1;
                            self->fsmState = I2CManager_FsmStop;
                        }
                    }
                }
                else
                {
                    /* Resend */
                    self->fsmTimer.flags.active = 0;
                    if(self->writeCount < self->currentDataTransfer.length)
                    {
                        I2C_TransmitByte(self->peripheral, self->currentDataTransfer.data[self->writeCount]);
                        self->fsmTimer.flags.start = 1;
                    }
                }
            }
            break;
        // @todo add timeout event
    }
}

// *****************************************************************************

static void I2CManager_FsmReadData(I2CManager *self, I2CEvent *e)
{
    switch(e->sig)
    {
        case I2C_SIG_DATA_RECEIVED:
            if(self->currentDataTransfer.transferType == I2C_TRANSFER_TYPE_READ) // @follow-up is this extra check needed? - MS
            {
                self->fsmTimer.flags.active = 0;
                self->currentDataTransfer.data[self->readCount++] = I2C_GetReceivedByte(self->peripheral);
                if(self->readCount < self->currentDataTransfer.length)
                    I2C_SendAck(self->peripheral, true);
                else
                    I2C_SendAck(self->peripheral, false); // send NACK on last byte
                self->fsmTimer.flags.start = 1;
            }
            break;
        case I2C_SIG_BUS_IDLE_EVENT:
            self->fsmTimer.flags.active = 0;
            /* Send ack finished. Prepare to receive byte if there are more 
            bytes to read. Otherwise, end transfer. */
            if(self->readCount < self->currentDataTransfer.length)
            {
                I2C_ReceiveByte(self->peripheral);
                self->fsmTimer.flags.start = 1;
            }
            else
            {
                /* We are finished with the transfer. Check to see if there 
                is another transfer ready and generate a repeated start. */
                if(e->generateRepeatedStart) // @todo replace with slave buffer check
                {
                    I2C_Restart(self->peripheral);
                    self->fsmTimer.flags.start = 1;
                    self->fsmState = I2CManager_FsmRestart;
                }
                else
                {
                    I2C_Stop(self->peripheral);
                    self->fsmTimer.flags.start = 1;
                    self->fsmState = I2CManager_FsmStop;
                }
            }
            break;
        // @todo add timeout event
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
        // @todo add timeout event
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
            e->generateRepeatedStart = false;
            e->repeatedStartSent = true;
            self->fsmState = I2CManager_FsmIdle;
            break;
        // @todo add timeout event
    }
}

/*
 End of File
 */
