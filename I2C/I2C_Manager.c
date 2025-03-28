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


// ***** Global Variables ******************************************************

static bool I2CManagerEnabled; // @todo enable/disable

// @todo local function pointers
// void (*I2C1_TransmitFinishedCallback)(void);
// void (*I2C1_ReceiveInterruptCallback)(void);

// states
static void I2CManager_FsmIdle(I2CEvent *e);
static void I2CManager_FsmStart(I2CEvent *e);
static void I2CManager_FsmWriteAddress(I2CEvent *e);
static void I2CManager_FsmWriteData(I2CEvent *e);
static void I2CManager_FsmStop(I2CEvent *e);
static void I2CManager_FsmRestart(I2CEvent *e);
static void I2CManager_FsmReadData(I2CEvent *e);

// ***** Static Function Prototypes ********************************************

static void I2CManager_DevicePush(I2CSlave_Node *self, I2CSlave_Node *endOfList);


// ----- stuff from PIC32 code. @todo clean up ---------------------------------

static void I2CTimer_Start(void);
static void I2CTimer_Stop(void);
//static bool IsBusIdle(void);
static I2CManagerStatusBits I2CManager_GetStatusBits(void);

// @todo Fsm function prototypes from old I2C.h. Currently rewriting I2C.h.
void I2CManager_FsmInit(uint16_t tickRateInNs, uint16_t timeoutInUs);
void I2CManager_Process(I2CManager *self);
void I2CManager_MasterWrite(I2CManager *self, I2CSlave_Node *slave, uint8_t *writeData, uint8_t numBytes, bool repeatedStart);
void I2CManager_MasterRead(I2CManager *self, I2CSlave_Node *slave, uint8_t *readData, uint8_t numBytes);
bool I2CManager_IsIdle(void);
void I2CManager_GetData(uint8_t *numBytesWritten, uint8_t *numBytesRead, I2CSlave_Node *context);

// *****************************************************************************

void I2CManager_Create(I2CManager *self, I2C *peripheral)
{
    self->peripheral = peripheral;
    self->endOfList = NULL;
    self->device = NULL;
}

// *****************************************************************************

void I2CManager_AddSlave(I2CManager *self, I2CSlave_Node *slave, uint8_t *writeBuffer, uint8_t *readBuffer)
{
    slave->writeBuffer = writeBuffer;
    slave->readBuffer = readBuffer;
    slave->numBytesToRead = 0;
    slave->numBytesToSend = 0;
    slave->readCount = 0;
    slave->writeCount = 0;
    slave->transferFinished = false;

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

    I2CEvent action = {0};

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

    I2CManagerStatusBits currentStatus;
    currentStatus = I2CManager_GetStatusBits();

    if(self->statusBits.sendingStart && !currentStatus.sendingStart)
    {
        self->statusBits.sendingStart = 0;
        action.sig = I2C_SIG_BUS_IDLE_EVENT;
        I2CManager_Fsm.state(&action); // call the current state and pass the event
    }
    if(self->statusBits.sendingRestart && !currentStatus.sendingRestart)
    {
        self->statusBits.sendingRestart = 0;
        action.sig = I2C_SIG_BUS_IDLE_EVENT;
        I2CManager_Fsm.state(&action); // call the current state and pass the event
    }
    if(self->statusBits.sendingStop && !currentStatus.sendingStop)
    {
        self->statusBits.sendingStop = 0;
        action.sig = I2C_SIG_BUS_IDLE_EVENT;
        I2CManager_Fsm.state(&action); // call the current state and pass the event
    }
    if(self->statusBits.sendingAck && !currentStatus.sendingAck)
    {
        self->statusBits.sendingAck = 0;
        action.sig = I2C_SIG_BUS_IDLE_EVENT;
        I2CManager_Fsm.state(&action); // call the current state and pass the event
    }
    if(self->statusBits.transmitInProgress && !currentStatus.transmitInProgress)
    {
        // Be careful, with the PIC it will clear the transmit finished flag
        // before all 8 bits have been shifted out. So we must check for an 
        // empty transmit register as well before clearing our flag.
        if(!I2C1_IsTransmitRegisterFull())
        {
            self->statusBits.transmitInProgress = 0;
            action.sig = I2C_SIG_BUS_IDLE_EVENT;
            I2CManager_Fsm.state(&action); // call the current state and pass the event
        }
    }
    else if(self->statusBits.receiveInProgress && !currentStatus.receiveInProgress)
    {
        if(I2C1_IsReceivedDataAvailable())
        {
            self->statusBits.receiveInProgress = 0;
            action.sig = I2C_SIG_DATA_RECEIVED;
            I2CManager_Fsm.state(&action);
        }
    }
    else if(self->fsmTimer.flags.expired)
    {
        self->fsmTimer.flags.expired = 0;
        action.sig = I2C_SIG_TIMEOUT;
        // @todo This is temporary! I removed during troubleshooting with 
        // logic analyzer. I don't think the timeout value is long enough.
        // So I need to do some measurements first
        //I2CManager_Fsm.state(&action); 
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
    if(!I2C_IsEnabled())
        I2C_Enable(self->peripheral);
    
    I2CManagerEnabled = true;
}

// *****************************************************************************

void I2CManager_Disable(I2CManager *self)
{
    I2CManagerEnabled = false;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Local Functions *****************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

static void I2CManager_DevicePush(I2CSlave_Node *self, I2CSlave_Node *endOfList)
{
    /* Add the new entry to the beginning of the list. Make the "next" pointer
    point to the head */
    self->next = endOfList->next;
    /* Update the beginning of the list to point to the new beginning */
    endOfList->next = self;
}

// I2CSlave_Node_Create
// I2CSlave_Node_Init

// ----- @label experimental slave code ----------------------------------------

/* For now the slave will have its own fixed buffer of a small size. I might 
make it where the buffer is managed externally via pointer in the future.
Ideally the size of the buffer should be two at minimum to allow for a write 
followed by a read. Because of that I might keep it defined in the slave. */
bool I2CSlave_IsDataTransferFinished(I2CSlave *self)
{

}

// I2CSlave_DataTransferFinishedCallback

#define CircularIncrement(i, size) i == (size - 1) ? 0 : i + 1

// names: RequestDataTransfer? AddDataTransfer
void I2CSlave_WriteToDataTransferBuffer(I2CSlave *self, bool isReadRequest, uint8_t *writeData, uint16_t length)
{
    // check if slave is busy already?
    uint8_t tempHead = CircularIncrement(self->private.head, I2CSLAVE_DR_BUFFER_SIZE);

    if(tempHead != self->private.txTail)
    {
        // There is space in the buffer
        self->private.buffer[self->private.head].readTypeRequest = isReadRequest;
        self->private.buffer[self->private.head].data = writeData;
        self->private.buffer[self->private.head].length = length;
        self->private.head = tempHead;
        //self->private.bufferIsNotEmpty = true;
    }
}

uint8_t I2CSlave_ReadFromDataTransferBuffer(I2CSlave *self, I2CDataRequest *returnDataRequest)
{
    if(self->private.head != self->private.tail)
    {
        // The buffer is not empty
        // @follow-up test simple struct assignment first instead of memcpy just for better readability - MS
        *returnDataRequest = self->private.buffer[self->private.tail].readTypeRequest;
        self->private.tail = CircularIncrement(self->private.tail, I2CSLAVE_DR_BUFFER_SIZE);
        return 0; // no error
    }
    else
    {
        returnDataRequest->length = 0;
        return 1;
    }
}

uint8_t I2CSlave_GetDataTransferBufferCount(I2CSlave *self)
{
    int16_t count = self->private.head - self->private.tail;

    if(count < 0)
    {
        count += I2CSLAVE_DR_BUFFER_SIZE;
    }
    return count;
}

// ----- @label stuff from old state machine. Refactor -------------------------

// @remove old function after re-factoring
void I2CManager_MasterWrite(I2CManager *self, I2CSlave_Node *slave, uint8_t *writeData, uint8_t numBytes)
{
    if(I2CManager_Fsm.state != I2CManager_FsmIdle)
        return;

    slave->writeBuffer = writeData;
    slave->numBytesToSend = numBytes;
    slave->private.writeCount = 0;

    // create an event to give to the state machine
    I2CEvent event;
    event.private.masterRead = false;
    event.private.slaveAddress = slave->slaveAddress << 1;
    event.sig = I2C_SIG_BEGIN_TRANSFER;
    I2CManager_Fsm.state(&event); // call the current state and pass the event
}

void I2CManager_MasterRead(I2CManager *self, I2CSlave_Node *slave, uint8_t *readData, uint8_t numBytes)
{
    // @todo setup a lock to keep this function from being called if it's
    // already running. Decide if I want to make the return type a bool
    if(I2CManager_Fsm.state != I2CManager_FsmIdle)
        return;

    slave->readBuffer = readData;
    slave->numBytesToRead = numBytes;
    slave->private.readCount = 0;

    // create an event to give to the state machine
    I2CEvent event;
    event.private.masterRead = true;
    event.private.slaveAddress = ((slave->slaveAddress << 1) | 1);
    event.sig = I2C_SIG_BEGIN_TRANSFER;
    I2CManager_Fsm.state(&event); // call the current state and pass the event
}

bool I2CManager_IsIdle(void)
{
    if(I2CManager_Fsm.state == I2CManager_FsmIdle)
        return true;
    else
        return false;
}

// @todo refactor get data function - MS
void I2CManager_GetData(uint8_t *numBytesWritten, uint8_t *numBytesRead, I2CSlave_Node *context)
{
    // @todo decide if I want to use a separate static I2C object or just use 
    // the I2C slave object
    *numBytesWritten = ptrI2CSlave->private.writeCount;
    *numBytesRead = ptrI2CSlave->private.readCount;
    // context = ptrI2CSlave; // @todo get data
}

static void I2CTimer_Start(I2CManager *self)
{
    self->fsmTimer.flags.start = 1;
}

static void I2CTimer_Stop(I2CManager *self)
{
    self->fsmTimer.flags.active = 0;
}

// @todo keep status bits?
static I2CManagerStatusBits GetStatusBits(I2C *peripheral)
{
    I2CManagerStatusBits retValue;
    // retValue.sendingStart = I2C1CONbits.SEN;
    // retValue.sendingRestart = I2C1CONbits.RSEN;
    // retValue.sendingStop = I2C1CONbits.PEN;
    // retValue.sendingAck = I2C1CONbits.ACKEN;
    // retValue.receiveInProgress = I2C1CONbits.RCEN;
    // retValue.transmitInProgress = I2C1STATbits.TRSTAT;
    return retValue;
}

// ----- Old FSM code ----------------------------------------------------------

static void I2CManager_FsmInit(uint16_t tickRateInNs, uint16_t timeoutInUs)
{
    I2C1_Event.sig = 0;
    I2CManager_Fsm.state = I2CManager_FsmIdle;

    // @todo finish setting up timer using variables given
    self->fsmTimer.period = (uint16_t)TIMEOUT_PERIOD_COUNT;
}

static void I2CManager_FsmIdle(I2CEvent *e)
{
    switch(e->sig)
    {
        case I2C_SIG_BEGIN_TRANSFER:
            if(e->repeatedStart)
            {
                // skip the start and go straight to the address
                e->repeatedStart = false;
                I2C1_TransmitByte(e->slaveAddress);
                I2CTimer_Start();
                I2CManager_Fsm.state = I2CManager_FsmWriteAddress;
            }
            else
            {
                I2C1_Start();
                I2CTimer_Start();
                I2CManager_Fsm.state = I2CManager_FsmStart;
            }
            break;
    }
}

static void I2CManager_FsmStart(I2CEvent *e)
{
    switch(e->sig)
    {
        case I2C_SIG_BUS_IDLE_EVENT:
            I2CTimer_Stop();
            // send slave address
            I2C1_TransmitByte(e->slaveAddress);
            I2CTimer_Start();
            I2CManager_Fsm.state = I2CManager_FsmWriteAddress;
            break;
    }
}

static void I2CManager_FsmWriteAddress(I2CEvent *e)
{
    switch(e->sig)
    {
        case I2C_SIG_BUS_IDLE_EVENT:
            I2CTimer_Stop();
            if(I2C1_GetAckStatus())
            {
                // address acknowledged.
                if(e->private.masterRead)
                {
                    // prepare to receive byte
                    I2C1_ReceiveEnable();
                    I2CManager_Fsm.state = I2CManager_FsmReadData;
                }
                else
                {
                    // load first data byte
                    I2C1_TransmitByte(ptrI2CSlave->writeBuffer[ptrI2CSlave->private.writeCount]);
                    I2CManager_Fsm.state = I2CManager_FsmWriteData;
                }
                I2CTimer_Start();
            }
            break;
    }
}

static void I2CManager_FsmWriteData(I2CEvent *e)
{
    switch(e->sig)
    {
        case I2C_SIG_BUS_IDLE_EVENT:
            I2CTimer_Stop();
            if(I2C1_GetAckStatus())
            {
                // data acknowledged. Check if there are more bytes to send
                ptrI2CSlave->private.writeCount++;
                if(ptrI2CSlave->private.writeCount < ptrI2CSlave->numBytesToSend)
                {
                    I2C1_TransmitByte(ptrI2CSlave->writeBuffer[ptrI2CSlave->private.writeCount]);
                    I2CTimer_Start();
                    // stay in this state and continue sending bytes
                }
                else
                {
                    // We are finished sending bytes. Check to see if we should
                    // generate a repeated start
                    if(e->private.generateRepeatedStart)
                    {
                        I2C1_Restart(); // send repeated start command
                        I2CTimer_Start();
                        I2CManager_Fsm.state = I2CManager_FsmRestart;
                    }
                    else
                    {
                        I2C1_Stop();
                        I2CTimer_Start();
                        I2CManager_Fsm.state = I2CManager_FsmStop;
                    }
                }
            }
            break;
    }
}

static void I2CManager_FsmStop(I2CEvent *e)
{
    switch(e->sig)
    {
        case I2C_SIG_BUS_IDLE_EVENT:
            I2CTimer_Stop();
            // Stop is finished.
            I2CManager_Fsm.state = I2CManager_FsmIdle;
            break;
    }
}

static void I2CManager_FsmRestart(I2CEvent *e)
{
    switch(e->sig)
    {
        case I2C_SIG_BUS_IDLE_EVENT:
            I2CTimer_Stop();
            // restart is finished.
            e->private.generateRepeatedStart = false;
            e->private.repeatedStart = true;
            I2CManager_Fsm.state = I2CManager_FsmIdle;
            break;
    }
}

static void I2CManager_FsmReadData(I2CEvent *e)
{
     switch(e->sig)
    {
        case I2C_SIG_DATA_RECEIVED:
            I2CTimer_Stop();
            ptrI2CSlave->readBuffer[ptrI2CSlave->private.readCount++] = I2C1_GetReceivedByte();
            if(ptrI2CSlave->private.readCount < ptrI2CSlave->numBytesToRead)
            {
                I2C1_SendAck(true);
            }
            else
            {
                I2C1_SendAck(false); // send NACK on last byte
            }
            I2CTimer_Start();
            break;
        case I2C_SIG_BUS_IDLE_EVENT:
            I2CTimer_Stop();
            // prepare to receive byte
            if(ptrI2CSlave->private.readCount < ptrI2CSlave->numBytesToRead)
            {
                I2C1_ReceiveEnable();
                I2CTimer_Start();
            }
            else
            {
                I2C1_Stop();
                I2CTimer_Start();
                I2CManager_Fsm.state = I2CManager_FsmStop;
            }
            break;
    }
}

/*
 End of File
 */
