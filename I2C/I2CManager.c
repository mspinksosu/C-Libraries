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
 *      @todo details
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

#include "I2CManager.h"
#include <stddef.h> // needed for NULL

// ***** Defines ***************************************************************

/* desired timeout in ns */ // @todo use a shortened timer for start stop?
#define I2CMANAGER_LONG_TIMEOUT_PERIOD_NS  (500000UL) // 500 us
#define I2CMANAGER_SHORT_TIMEOUT_PERIOD_NS (100000UL) // 100 us
#define I2CMANAGER_DEFAULT_TICK_RATE_NS    50
#define I2CMANAGER_REPEAT_LIMIT            5

// ***** Global Variables ******************************************************

static bool I2CManagerEnabled; // @todo enable/disable

// @todo function pointers
// void (*I2C_TransmitFinishedCallback)(void);
// void (*I2C_ReceiveInterruptCallback)(void);

// states
static void I2CManager_FsmIdle(I2CManager *self, I2CEvent *e);
static void I2CManager_FsmStart(I2CManager *self, I2CEvent *e);
static void I2CManager_FsmWriteAddress(I2CManager *self, I2CEvent *e);
static void I2CManager_FsmWriteData(I2CManager *self, I2CEvent *e);
static void I2CManager_FsmStop(I2CManager *self, I2CEvent *e);
static void I2CManager_FsmRestart(I2CManager *self, I2CEvent *e);
static void I2CManager_FsmReadData(I2CManager *self, I2CEvent *e);

// ***** Static Function Prototypes ********************************************

static void I2CManager_DevicePush(I2CTarget_Node *self, I2CTarget_Node *endOfList);
static void I2CManager_BeginDataTransfer(I2CManager *self, DataTransfer *dtObject);
static void I2CManager_GenerateFinishedTransferReport(I2CManager *self, I2CDataTransferStatus *retReport);

// *****************************************************************************

void I2CManager_Init(I2CManager *self, I2C *peripheral, uint32_t tickRateNs)
{
    self->peripheral = peripheral;
    /* @follow-up Should init always clear the list by setting the pointers to NULL? This would require that 
    the person using the library makes sure that they call Init before AddSlave. If they accidentally 
    add all target devices, then call init, the manager list will be erased. - MS */
    // self->endOfList = NULL;
    // self->currentDevice = NULL;
    self->currentDataTransfer.length = 0;
    self->writeCount = 0;
    self->readCount = 0;
    // @todo init state and error variables

    if(tickRateNs == 0)
        tickRateNs = I2CMANAGER_DEFAULT_TICK_RATE_NS;

    self->fsmLongTimeoutPeriod = I2CMANAGER_LONG_TIMEOUT_PERIOD_NS / tickRateNs;
    self->fsmShortTimeoutPeriod = I2CMANAGER_SHORT_TIMEOUT_PERIOD_NS / tickRateNs;

    if(self->fsmLongTimeoutPeriod == 0)
        self->fsmLongTimeoutPeriod = 1;
    if(self->fsmShortTimeoutPeriod == 0)
        self->fsmShortTimeoutPeriod = 1;

    self->fsmState = I2CManager_FsmIdle;
    self->fsmTimerStateEnterCallback = I2CManager_FsmIdle;
}

// *****************************************************************************

void I2CManager_AddDevice(I2CManager *self, I2CTarget_Node *targetDevice)
{
    if(self->endOfList == NULL)
    {
        /* Begin with a new list */
        self->endOfList = targetDevice;

        /* Since the list only contains one entry, the "next" pointer will
        also point to itself */
        self->endOfList->next = self->endOfList;
    }
    else
    {
        I2CManager_DevicePush(targetDevice, self->endOfList);
    }
    self->currentDevice = self->endOfList->next; // reset the index
}

// *****************************************************************************

void I2CManager_Process(I2CManager *self)
{
    I2CEvent event = {0};
    DataTransfer tempDataTransfer = {0};
    I2CDataTransferStatus tempStatusReport = {0};

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

    /* Check for I2C events */
    I2CState currentPeripheralState = I2C_GetState(self->peripheral);
    /* @todo Decide if I want to keep the old status bits code or replace it 
    with just the state. I could keep the status bits and make sure the events 
    are explicit. Or just make a simple check if the previous state was not 
    equal to the current state. */
    if(self->statusBits.sendingStart && currentPeripheralState == I2C_STATE_BUS_IDLE)
    {
        self->statusBits.sendingStart = 0;
        event.sig = I2C_MANAGER_SIG_BUS_IDLE_EVENT;
        self->fsmState(self, &event); // call the current state and pass the event
    }
    if(self->statusBits.sendingRestart && currentPeripheralState == I2C_STATE_BUS_IDLE)
    {
        self->statusBits.sendingRestart = 0;
        event.sig = I2C_MANAGER_SIG_BUS_IDLE_EVENT;
        self->fsmState(self, &event);
    }
    if(self->statusBits.sendingStop && currentPeripheralState == I2C_STATE_BUS_IDLE)
    {
        self->statusBits.sendingStop = 0;
        event.sig = I2C_MANAGER_SIG_BUS_IDLE_EVENT;
        self->fsmState(self, &event);
    }
    if(self->statusBits.sendingAck && currentPeripheralState == I2C_STATE_BUS_IDLE)
    {
        self->statusBits.sendingAck = 0;
        event.sig = I2C_MANAGER_SIG_BUS_IDLE_EVENT;
        self->fsmState(self, &event);
    }

    if(self->statusBits.transmitInProgress && currentPeripheralState != I2C_STATE_MASTER_TRANSMITTING)
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
            event.sig = I2C_MANAGER_SIG_BUS_IDLE_EVENT;
            self->fsmState(self, &event);
        }
    }
    else if(self->statusBits.receiveInProgress && currentPeripheralState != I2C_STATE_MASTER_RECEIVING)
    {
        if(I2C_IsReceiveRegisterFull(self->peripheral))
        {
            self->statusBits.receiveInProgress = 0;
            event.sig = I2C_MANAGER_SIG_DATA_RECEIVED;
            self->fsmState(self, &event);
        }
    }

    if(self->fsmTimer.flags.expired)
    {
        self->fsmTimer.flags.expired = 0;
        self->fsmRepeatCount++;
        if(self->fsmRepeatCount <= self->fsmRepeatLimit)
        {
            /* @todo timeout retry event. Test with debugger. Decided if I just 
            want to always call the enter signal by default, or have the ability 
            to set a specific action. */
            // event.sig = I2C_MANAGER_SIG_ENTER;
            // self->fsmTimerStateEnterCallback(self, &event); // @debug testing timeout
        }
        else
        {
            self->fsmRepeatCount = 0;
            self->fsmTimer.flags.start = 0;
            self->fsmTimer.flags.active = 0;
            self->fsmTimer.flags.expired = 0;
            // event.sig = I2C_MANAGER_SIG_TIMEOUT;
            // self->fsmTimerStateEnterCallback(self, &event); // @debug testing timeout
        }
    }
    self->peripheralState = currentPeripheralState;

    // @debug testing recover from bus collision
    if(I2C_GetBusError(self->peripheral))
    {
        event.sig = I2C_MANAGER_SIG_SEND_STOP;
        self->fsmState(self, &event);
        self->managerState = I2C_MANAGER_STATE_IDLE;
        self->currentDevice->state = I2C_SLAVE_STATE_IDLE;
        self->currentDevice->private.transferStartedEventFlag = false;
        self->currentDevice->private.transferFinishedEventFlag = false;
        // @todo I2CManager. Send some sort of error to notify user
        /* @todo Tested purposely breaking the I2C bus then recovering it. 
        Take this code and turn it into a function and remove dependencies 
        on GPIO. - MS */
        I2C_Disable(self->peripheral);
        TRISDbits.TRISD10 = 0; // I2C1 SCL
        for(uint8_t i = 0; i < 10; i++)
        {
            uint16_t count = 1000;
            LATDbits.LATD10 = 0;
            while(count--);
            LATDbits.LATD10 = 1;
            count = 1000;
            while(count--);
        }
        I2C_Enable(self->peripheral);
    }

    /* Go through list and process each targets data requests. */
    if(self->currentDevice != NULL)
    {
        uint8_t transferError = 1;
        switch(self->managerState)
        {
            case I2C_MANAGER_STATE_IDLE:
                if(I2CSlave_GetDataTransferBufferCount(self->currentDevice) > 0 && 
                    self->fsmState == I2CManager_FsmIdle)
                {
                    transferError = I2CSlave_ReadDataTransfer(self->currentDevice, &tempDataTransfer);
                    if(transferError == 0)
                    {
                        I2CManager_BeginDataTransfer(self, &tempDataTransfer);
                        self->managerState = I2C_MANAGER_STATE_TRANSFER_IN_PROGRESS;
                        self->currentDevice->state = I2C_SLAVE_STATE_TRANSFER_IN_PROGRESS;
                        self->currentDevice->private.transferStartedEventFlag = true;
                    }
                }
                else
                {
                    self->currentDevice = self->currentDevice->next;
                }
                break;
            case I2C_MANAGER_STATE_TRANSFER_IN_PROGRESS:
                if(self->currentDataTransferFinished)
                {
                    /* Get the status of the current transfer and write it to 
                    the target device */
                    I2CManager_GenerateFinishedTransferReport(self, &tempStatusReport);
                    self->currentDevice->finishedTransferReport = tempStatusReport;
                    self->currentDevice->private.transferFinishedEventFlag = true;
                    /* Check if there is more data to transfer. If there is, 
                    send a repeated start event to the state machine. */
                    if(I2CSlave_GetDataTransferBufferCount(self->currentDevice) > 0)
                    {
                        event.sig = I2C_MANAGER_SIG_SEND_RESTART;
                        self->fsmState(self, &event);
                    }
                    else
                    {
                        /* Finished. Set the state of the current device, then 
                        go to the next device. */
                        event.sig = I2C_MANAGER_SIG_SEND_STOP;
                        self->fsmState(self, &event);
                        self->currentDevice->state = I2C_SLAVE_STATE_IDLE;
                        self->currentDevice = self->currentDevice->next;
                    }
                    self->managerState = I2C_MANAGER_STATE_IDLE;
                }
                break;
        }
    }
    else
    {
        self->currentDevice = self->currentDevice->next;
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

bool I2CManager_IsBusy(I2CManager *self)
{
    if(self->fsmState == I2CManager_FsmIdle)
        return false;
    else
        return true;
}

// *****************************************************************************

I2CManagerState I2CManager_GetState(I2CManager *self)
{
    return self->managerState;
}

// *****************************************************************************

void I2CManager_GetCurrentDevice(I2CManager *self, I2CTarget *retDevice)
{
    *retDevice = *(self->currentDevice->i2cDevice);
}

// *****************************************************************************

/* @todo not sure if/how I want to implement this yet. Should status reflect 
on going changes during the data transfer, such as the current number of bytes
transferred? Or should I just use it as a end of data transfer report? - MS */
void I2CManager_GetDataTransferStatus(I2CManager *self, I2CDataTransferStatus *retTransferStatus)
{
    retTransferStatus->error = I2C_MANAGER_TRANSFER_ERROR_NONE; // @todo I2C error codes
    retTransferStatus->transferType = self->currentDevice->finishedTransferReport.transferType;
    retTransferStatus->ptrArray = self->currentDevice->finishedTransferReport.ptrArray;
    retTransferStatus->sizeOfArray = self->currentDevice->finishedTransferReport.sizeOfArray;
    retTransferStatus->numOfBytesTransferred = self->currentDevice->finishedTransferReport.numOfBytesTransferred;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Local Functions *****************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

static void I2CManager_DevicePush(I2CTarget_Node *self, I2CTarget_Node *endOfList)
{
    /* Add the new entry to the beginning of the list. Make the "next" pointer
    point to the head */
    self->next = endOfList->next;
    /* Update the beginning of the list to point to the new beginning */
    endOfList->next = self;
}

// *****************************************************************************

static void I2CManager_BeginDataTransfer(I2CManager *self, DataTransfer *dtObject)
{
    if(self->fsmState != I2CManager_FsmIdle)
        return;

    /* Copy the data over to our temporary transfer buffer */
    self->currentDataTransfer = *dtObject;
    self->currentDataTransferFinished = false;
    I2CEvent event = {0}; // create an event to give to the state machine
    event.sig = I2C_MANAGER_SIG_BEGIN_TRANSFER;
    self->fsmState(self, &event); // call the current state and pass the event
}

// *****************************************************************************

static void I2CManager_GenerateFinishedTransferReport(I2CManager *self, I2CDataTransferStatus *retReport)
{
    retReport->error = I2C_MANAGER_TRANSFER_ERROR_NONE; // @todo I2C error codes

    retReport->transferType = self->currentDataTransfer.transferType;
    retReport->ptrArray = self->currentDataTransfer.ptrDataArray;
    retReport->sizeOfArray = self->currentDataTransfer.length;
    if(retReport->transferType == DATA_TRANSFER_TYPE_WRITE)
        retReport->numOfBytesTransferred = self->writeCount;
    else
        retReport->numOfBytesTransferred = self->readCount;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** States **************************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

static void I2CManager_FsmIdle(I2CManager *self, I2CEvent *e)
{
    switch(e->sig)
    {
        case I2C_MANAGER_SIG_BEGIN_TRANSFER:
            if(self->statusBits.repeatedStartSent)
            {
                /* Repeat start. Skip start and go straight to the address */
                self->statusBits.repeatedStartSent = 0;
                self->fsmState = I2CManager_FsmWriteAddress;
            }
            else
            {
                self->fsmState = I2CManager_FsmStart;
            }
            self->fsmRepeatCount = 0;
            self->fsmRepeatLimit = I2CMANAGER_REPEAT_LIMIT;
            e->sig = I2C_MANAGER_SIG_ENTER;
            self->fsmState(self, e);
            break;
        default:
            break;
    }
}

// *****************************************************************************

static void I2CManager_FsmStart(I2CManager *self, I2CEvent *e)
{
    switch(e->sig)
    {
        case I2C_MANAGER_SIG_ENTER:
            I2C_Start(self->peripheral);
            self->fsmTimer.period = self->fsmShortTimeoutPeriod;
            self->fsmTimer.flags.start = 1;
            self->fsmTimerStateEnterCallback = I2CManager_FsmStart;
            self->statusBits.sendingStart = 1;
            break;
        case I2C_MANAGER_SIG_EXIT:
            self->fsmTimer.flags.start = 0;
            self->fsmTimer.flags.active = 0;
            self->fsmTimer.flags.expired = 0;
            self->fsmRepeatCount = 0;
            break;
        case I2C_MANAGER_SIG_BUS_IDLE_EVENT:
            /* Transition to next state. Perform our exit action, 
            then perform entry action. */
            e->sig = I2C_MANAGER_SIG_EXIT;
            self->fsmState(self, e);
            e->sig = I2C_MANAGER_SIG_ENTER;
            self->fsmState = I2CManager_FsmWriteAddress;
            self->fsmState(self, e);
            break;
        case I2C_MANAGER_SIG_SEND_STOP:
            e->sig = I2C_MANAGER_SIG_EXIT;
            self->fsmState(self, e);
            e->sig = I2C_MANAGER_SIG_ENTER;
            self->fsmState = I2CManager_FsmStop;
            self->fsmState(self, e);
            break;
        case I2C_MANAGER_SIG_TIMEOUT:
            /* @todo set a error flag of some kind, then transition to 
            stop state */
            e->sig = I2C_MANAGER_SIG_EXIT;
            self->fsmState(self, e);
            e->sig = I2C_MANAGER_SIG_ENTER;
            self->fsmState = I2CManager_FsmStop;
            self->fsmState(self, e);
            break;
        default:
            break;
    }
}

// *****************************************************************************

static void I2CManager_FsmWriteAddress(I2CManager *self, I2CEvent *e)
{
    switch(e->sig)
    {
        case I2C_MANAGER_SIG_ENTER:
        {
            /* Prepare to write or read data */
            uint8_t targetAddressPlusRW = 0;
            if(self->currentDataTransfer.transferType == DATA_TRANSFER_TYPE_WRITE)
            {
                targetAddressPlusRW = (self->currentDevice->targetAddress7Bit) << 1;
                self->writeCount = 0;
            }
            else
            {
                targetAddressPlusRW = ((self->currentDevice->targetAddress7Bit << 1) | 1);
                self->readCount = 0;
            }
            I2C_TransmitByte(self->peripheral, targetAddressPlusRW);
            self->fsmTimerStateEnterCallback = I2CManager_FsmWriteAddress;
            self->fsmTimer.period = self->fsmLongTimeoutPeriod;
            self->fsmTimer.flags.start = 1;
            self->statusBits.transmitInProgress = 1;
            break;
        }
        case I2C_MANAGER_SIG_EXIT:
            self->fsmTimer.flags.start = 0;
            self->fsmTimer.flags.active = 0;
            self->fsmTimer.flags.expired = 0;
            self->fsmRepeatCount = 0;
            break;
        case I2C_MANAGER_SIG_BUS_IDLE_EVENT:
            if(I2C_GetAckSlaveStatus(self->peripheral))
            {
                /* Address acknowledged. Perform our exit action, 
                then transition to next state. */
                e->sig = I2C_MANAGER_SIG_EXIT;
                self->fsmState(self, e);
                if(self->currentDataTransfer.transferType == DATA_TRANSFER_TYPE_READ)
                {
                    /* Prepare to receive byte */
                    self->readCount = 0;
                    e->sig = I2C_MANAGER_SIG_ENTER;
                    self->fsmState = I2CManager_FsmReadData;
                    self->fsmState(self, e);
                }
                else
                {
                    /* Prepare to transmit the first byte */
                    self->writeCount = 0;
                    e->sig = I2C_MANAGER_SIG_ENTER;
                    self->fsmState = I2CManager_FsmWriteData;
                    self->fsmState(self, e);
                }
            }
            else
            {
                /* Force timer to callback immediately instead of waiting */
                self->fsmTimerStateEnterCallback = I2CManager_FsmWriteAddress;
                self->fsmTimer.flags.active = 0;
                self->fsmTimer.flags.expired = 1;
            }
            break;
        case I2C_MANAGER_SIG_SEND_STOP:
            e->sig = I2C_MANAGER_SIG_EXIT;
            self->fsmState(self, e);
            e->sig = I2C_MANAGER_SIG_ENTER;
            self->fsmState = I2CManager_FsmStop;
            self->fsmState(self, e);
            break;
        case I2C_MANAGER_SIG_TIMEOUT:
            /* @todo set a error flag of some kind, then transition to 
            stop state */
            e->sig = I2C_MANAGER_SIG_EXIT;
            self->fsmState(self, e);
            e->sig = I2C_MANAGER_SIG_ENTER;
            self->fsmState = I2CManager_FsmStop;
            self->fsmState(self, e);
            break;
        default:
            break;
    }
}

// *****************************************************************************

static void I2CManager_FsmWriteData(I2CManager *self, I2CEvent *e)
{
    switch(e->sig)
    {
        case I2C_MANAGER_SIG_ENTER:
            I2C_TransmitByte(self->peripheral, self->currentDataTransfer.ptrDataArray[self->writeCount]);
            self->fsmTimerStateEnterCallback = I2CManager_FsmWriteData;
            self->fsmTimer.period = self->fsmLongTimeoutPeriod;
            self->fsmTimer.flags.start = 1;
            self->statusBits.transmitInProgress = 1;
            break;
        case I2C_MANAGER_SIG_EXIT:
            self->fsmTimer.flags.start = 0;
            self->fsmTimer.flags.active = 0;
            self->fsmTimer.flags.expired = 0;
            self->fsmRepeatCount = 0;
            break;
        case I2C_MANAGER_SIG_BUS_IDLE_EVENT:
            if(self->currentDataTransfer.transferType == DATA_TRANSFER_TYPE_WRITE)
            {
                if(I2C_GetAckSlaveStatus(self->peripheral))
                {
                    /* Data acknowledged. Check if there are more bytes to send */
                    self->writeCount++;
                    if(self->writeCount < self->currentDataTransfer.length)
                    {
                        /* Stay in this state and continue sending bytes. 
                        Perform exit action and re-enter to send next byte. */
                        e->sig = I2C_MANAGER_SIG_EXIT;
                        self->fsmState(self, e);
                        e->sig = I2C_MANAGER_SIG_ENTER;
                        self->fsmState(self, e);
                    }
                    else
                    {
                        /* We are finished with the transfer. Set a flag to tell 
                        the manager that we are done. The manager will tell us 
                        if we need to send a stop or a restart. */
                        self->currentDataTransferFinished = true;
                        /* Have the timer automatically send a stop on timeout 
                        as a failsafe. */
                        e->sig = I2C_MANAGER_SIG_EXIT;
                        self->fsmState(self, e);
                        self->fsmTimerStateEnterCallback = I2CManager_FsmStop;
                        self->fsmTimer.period = self->fsmLongTimeoutPeriod;
                        self->fsmTimer.flags.start = 1;
                    }
                }
                else
                {
                    /* Force timer to callback immediately instead of waiting */
                    self->fsmTimerStateEnterCallback = I2CManager_FsmWriteData;
                    self->fsmTimer.flags.active = 0;
                    self->fsmTimer.flags.expired = 1;
                }
            }
            break;
        case I2C_MANAGER_SIG_SEND_STOP:
            e->sig = I2C_MANAGER_SIG_EXIT;
            self->fsmState(self, e);
            e->sig = I2C_MANAGER_SIG_ENTER;
            self->fsmState = I2CManager_FsmStop;
            self->fsmState(self, e);
            break;
        case I2C_MANAGER_SIG_SEND_RESTART:
            e->sig = I2C_MANAGER_SIG_EXIT;
            self->fsmState(self, e);
            e->sig = I2C_MANAGER_SIG_ENTER;
            self->fsmState = I2CManager_FsmRestart;
            self->fsmState(self, e);
            break;
        case I2C_MANAGER_SIG_TIMEOUT:
            /* @todo set a error flag of some kind, then transition to 
            stop state */
            e->sig = I2C_MANAGER_SIG_EXIT;
            self->fsmState(self, e);
            e->sig = I2C_MANAGER_SIG_ENTER;
            self->fsmState = I2CManager_FsmStop;
            self->fsmState(self, e);
            break;
        default:
            break;
    }
}

// *****************************************************************************

static void I2CManager_FsmReadData(I2CManager *self, I2CEvent *e)
{
    switch(e->sig)
    {
        case I2C_MANAGER_SIG_ENTER:
            I2C_ReceiveByte(self->peripheral);
            self->fsmTimerStateEnterCallback = I2CManager_FsmReadData;
            self->fsmTimer.period = self->fsmLongTimeoutPeriod;
            self->fsmTimer.flags.start = 1;
            self->statusBits.receiveInProgress = 1;
            break;
        case I2C_MANAGER_SIG_EXIT:
            self->fsmTimer.flags.start = 0;
            self->fsmTimer.flags.active = 0;
            self->fsmTimer.flags.expired = 0;
            self->fsmRepeatCount = 0;
            break;
        case I2C_MANAGER_SIG_DATA_RECEIVED:
            if(self->currentDataTransfer.transferType == DATA_TRANSFER_TYPE_READ) // @follow-up is this extra check needed? - MS
            {
                self->currentDataTransfer.ptrDataArray[self->readCount++] = I2C_GetReceivedByte(self->peripheral);
                if(self->readCount < self->currentDataTransfer.length)
                    I2C_SendAck(self->peripheral, true); // @todo change to FSM state send ack or send nack rather than call I2C directly? - MS
                else
                    I2C_SendAck(self->peripheral, false); // send NACK on last byte
                self->fsmTimer.period = self->fsmShortTimeoutPeriod;
                self->fsmTimer.flags.start = 1; // restart
                self->statusBits.sendingAck = 1;
            }
            break;
        case I2C_MANAGER_SIG_BUS_IDLE_EVENT:
            /* Send ack finished. Prepare to receive byte if there are more 
            bytes to read. Otherwise, end transfer. */
            if(self->readCount < self->currentDataTransfer.length)
            {
                /* Re-enter this state. Perform exit action and entry action */
                e->sig = I2C_MANAGER_SIG_EXIT;
                self->fsmState(self, e);
                e->sig = I2C_MANAGER_SIG_ENTER;
                self->fsmState(self, e);
            }
            else
            {
                /* We are finished with the transfer. Set a flag to tell the 
                manager process that we are done. The manager process will 
                tell us if we need to send a stop or a restart. */
                self->currentDataTransferFinished = true;
                /* Have the timer automatically send a stop on timeout 
                as a failsafe. */
                e->sig = I2C_MANAGER_SIG_EXIT;
                self->fsmState(self, e);
                self->fsmTimerStateEnterCallback = I2CManager_FsmStop;
                self->fsmTimer.period = self->fsmLongTimeoutPeriod;
                self->fsmTimer.flags.start = 1;
            }
            break;
        case I2C_MANAGER_SIG_SEND_STOP:
            e->sig = I2C_MANAGER_SIG_EXIT;
            self->fsmState(self, e);
            e->sig = I2C_MANAGER_SIG_ENTER;
            self->fsmState = I2CManager_FsmStop;
            self->fsmState(self, e);
            break;
        case I2C_MANAGER_SIG_SEND_RESTART:
            e->sig = I2C_MANAGER_SIG_EXIT;
            self->fsmState(self, e);
            e->sig = I2C_MANAGER_SIG_ENTER;
            self->fsmState = I2CManager_FsmRestart;
            self->fsmState(self, e);
            break;
        case I2C_MANAGER_SIG_TIMEOUT:
            /* @todo set a error flag of some kind, then transition to 
            stop state */
            e->sig = I2C_MANAGER_SIG_EXIT;
            self->fsmState(self, e);
            e->sig = I2C_MANAGER_SIG_ENTER;
            self->fsmState = I2CManager_FsmStop;
            self->fsmState(self, e);
            break;
        default:
            break;
    }
}

// *****************************************************************************

static void I2CManager_FsmStop(I2CManager *self, I2CEvent *e)
{
    switch(e->sig)
    {
        case I2C_MANAGER_SIG_ENTER:
            I2C_Stop(self->peripheral);
            self->fsmTimer.period = self->fsmShortTimeoutPeriod;
            self->fsmTimer.flags.start = 1;
            self->statusBits.repeatedStartSent = 0;
            self->statusBits.sendingStop = 1;
            break;
        case I2C_MANAGER_SIG_EXIT:
            self->fsmTimer.flags.start = 0;
            self->fsmTimer.flags.active = 0;
            self->fsmTimer.flags.expired = 0;
            self->fsmRepeatCount = 0;
            break;
        case I2C_MANAGER_SIG_BUS_IDLE_EVENT:
            /* Stop is finished. Transition back to idle state. Perform exit 
            action. No entry action. */
            e->sig = I2C_MANAGER_SIG_EXIT;
            self->fsmState(self, e);
            self->fsmTimer.flags.active = 0;
            self->fsmState = I2CManager_FsmIdle;
            break;
        case I2C_MANAGER_SIG_TIMEOUT:
            /* @todo Sending a stop failed somehow. Set a error flag of some 
            kind then transition to idle state */
            e->sig = I2C_MANAGER_SIG_ENTER;
            self->fsmState = I2CManager_FsmIdle;
            self->fsmState(self, e);
            break;
        case I2C_MANAGER_SIG_SEND_STOP:
            /* Something went wrong and the manager requested a stop. We are 
            already in stop, so perform one last stop action then transition
            back to idle. Perform exit action. No entry action. */
            I2C_Stop(self->peripheral);
            e->sig = I2C_MANAGER_SIG_EXIT;
            self->fsmState(self, e);
            self->fsmTimer.flags.active = 0;
            self->fsmState = I2CManager_FsmIdle;
            break;
        default:
            break;
    }
}

// *****************************************************************************

static void I2CManager_FsmRestart(I2CManager *self, I2CEvent *e)
{
    switch(e->sig)
    {
        case I2C_MANAGER_SIG_ENTER:
            I2C_Restart(self->peripheral);
            self->fsmTimer.period = self->fsmShortTimeoutPeriod;
            self->fsmTimer.flags.start = 1;
            self->statusBits.repeatedStartSent = 0;
            self->statusBits.sendingRestart = 1;
            break;
        case I2C_MANAGER_SIG_EXIT:
            self->fsmTimer.flags.start = 0;
            self->fsmTimer.flags.active = 0;
            self->fsmTimer.flags.expired = 0;
            self->fsmRepeatCount = 0;
            break;
        case I2C_MANAGER_SIG_BUS_IDLE_EVENT:
            /* Repeat start is finished */
            e->sig = I2C_MANAGER_SIG_EXIT;
            self->fsmState(self, e);
            self->statusBits.repeatedStartSent = 1;
            self->fsmState = I2CManager_FsmIdle;
            break;
        case I2C_MANAGER_SIG_TIMEOUT:
            /* @todo set a error flag of some kind, then transition to 
            stop state */
            e->sig = I2C_MANAGER_SIG_EXIT;
            self->fsmState(self, e);
            e->sig = I2C_MANAGER_SIG_ENTER;
            self->fsmState = I2CManager_FsmStop;
            self->fsmState(self, e);
            break;
        default:
            break;
    }
}

/*
 End of File
 */
