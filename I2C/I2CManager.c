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
#include "DataTransfer.h"
#include <stddef.h> // needed for NULL

// ***** Defines ***************************************************************

/* desired timeout in ns */
#define LONG_TIMEOUT_PERIOD_NS  (500000UL) // 500 us
#define SHORT_TIMEOUT_PERIOD_NS (100000UL) // 100 us
#define DEFAULT_TICK_RATE_NS    50
/* how many times to retry an I2C command */
#define DEFAULT_REPEAT_LIMIT    3

// ***** Global Variables ******************************************************

static bool I2CManagerEnabled; // @todo enable/disable

// states
static void I2CManager_FsmIdle(I2CManager *self, const I2CEvent *e);
static void I2CManager_FsmStart(I2CManager *self, const I2CEvent *e);
static void I2CManager_FsmWriteAddress(I2CManager *self, const I2CEvent *e);
static void I2CManager_FsmWriteData(I2CManager *self, const I2CEvent *e);
static void I2CManager_FsmStop(I2CManager *self, const I2CEvent *e);
static void I2CManager_FsmRestart(I2CManager *self, const I2CEvent *e);
static void I2CManager_FsmReadData(I2CManager *self, const I2CEvent *e);

// ***** Static Function Prototypes ********************************************

static void I2CManager_PushNode(I2CManager_Node *self, I2CManager_Node *endOfList);
static void I2CManager_DeleteNode(I2CManager_Node *key, I2CManager_Node *endOfList);
static void I2CManager_BeginDataTransfer(I2CManager *self, DataTransfer *dtObject);
static void I2CManager_GenerateFinishedTransferReport(I2CManager *self, I2CManagerTransferStatus *retReport);
static void I2CManager_SetFSMTimerRepeat(I2CManager *self, uint8_t numRetry);
static void I2CManager_StartFSMTimer(I2CManager *self, uint32_t period);
static void I2CManager_ClearFSMTimer(I2CManager *self);

// *****************************************************************************

void I2CManager_Init(I2CManager *self, I2C *peripheral, uint32_t tickRateNs)
{
    self->peripheral = peripheral;
    /* @follow-up Should init always clear the list by setting the pointers to NULL? This would require that 
    the person using the library makes sure that they call Init before AddDevice. If they accidentally 
    add all target devices, then call init, the manager list will be erased. - MS */
    // self->endOfList = NULL;
    // self->currentNode->i2cDevice = NULL;
    self->currentDataTransfer.length = 0;
    self->writeCount = 0;
    self->readCount = 0;
    // @todo init state and error variables

    if(tickRateNs == 0)
        tickRateNs = DEFAULT_TICK_RATE_NS;

    self->fsmLongTimeoutPeriod = LONG_TIMEOUT_PERIOD_NS / tickRateNs;
    self->fsmShortTimeoutPeriod = SHORT_TIMEOUT_PERIOD_NS / tickRateNs;

    if(self->fsmLongTimeoutPeriod == 0)
        self->fsmLongTimeoutPeriod = 1;
    if(self->fsmShortTimeoutPeriod == 0)
        self->fsmShortTimeoutPeriod = 1;

    self->fsmState = I2CManager_FsmIdle;
}

// *****************************************************************************

void I2CManager_AddDevice(I2CManager *self, I2CManager_Node *device, I2CTarget *target)
{
    /* Combine the I2C target with the node */
    device->i2cDevice = target;

    /* Now add the node to the list */
    if(self->endOfList == NULL)
    {
        /* Begin with a new list */
        self->endOfList = device;

        /* Since the list only contains one entry, the "next" pointer will
        also point to itself */
        self->endOfList->next = self->endOfList;
    }
    else
    {
        I2CManager_PushNode(device, self->endOfList);
    }
    self->currentNode = self->endOfList->next; // reset the index
}

// *****************************************************************************

void I2CManager_RemoveDevice(I2CManager *self, I2CManager_Node *device)
{
    if(device != NULL)
    {
        I2CManager_DeleteNode(device, self->endOfList);
    }
    self->currentNode = self->endOfList->next; // reset the index
}

// *****************************************************************************

void I2CManager_Process(I2CManager *self)
{
    I2CEvent event = {0};
    DataTransfer tempDataTransfer = {0};
    I2CManagerTransferStatus tempStatusReport = {0};

    /* Check if we need to start a timer */
    if(self->fsmTimer.flags.start)
    {
        self->fsmTimer.flags.start = 0;
        self->fsmTimer.count = self->fsmTimer.period;
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

    if(self->statusBits.sendingStart && currentPeripheralState == I2C_STATE_BUS_IDLE)
    {
        self->statusBits.sendingStart = 0;
        event.sig = I2CMANAGER_SIG_BUS_IDLE_EVENT;
        self->fsmState(self, &event); // call the current state and pass the event
    }
    if(self->statusBits.sendingRestart && currentPeripheralState == I2C_STATE_BUS_IDLE)
    {
        self->statusBits.sendingRestart = 0;
        event.sig = I2CMANAGER_SIG_BUS_IDLE_EVENT;
        self->fsmState(self, &event);
    }
    if(self->statusBits.sendingStop && currentPeripheralState == I2C_STATE_BUS_IDLE)
    {
        self->statusBits.sendingStop = 0;
        event.sig = I2CMANAGER_SIG_BUS_IDLE_EVENT;
        self->fsmState(self, &event);
    }
    if(self->statusBits.sendingAck && currentPeripheralState == I2C_STATE_BUS_IDLE)
    {
        self->statusBits.sendingAck = 0;
        event.sig = I2CMANAGER_SIG_BUS_IDLE_EVENT;
        self->fsmState(self, &event);
    }

    if(self->statusBits.transmitInProgress && currentPeripheralState != I2C_STATE_CONTROLLER_TRANSMITTING)
    {
        if(I2C_IsTransmitRegisterEmpty(self->peripheral))
        {
            self->statusBits.transmitInProgress = 0;
            event.sig = I2CMANAGER_SIG_BUS_IDLE_EVENT;
            self->fsmState(self, &event);
        }
    }
    else if(self->statusBits.receiveInProgress && currentPeripheralState != I2C_STATE_CONTROLLER_RECEIVING)
    {
        if(I2C_IsReceiveRegisterFull(self->peripheral))
        {
            self->statusBits.receiveInProgress = 0;
            event.sig = I2CMANAGER_SIG_DATA_RECEIVED;
            self->fsmState(self, &event);
        }
    }

    if(self->fsmTimer.flags.expired)
    {
        self->fsmTimer.flags.expired = 0;
        self->fsmRepeatCount++;
        if(self->fsmRepeatCount <= self->fsmRepeatLimit)
        {
            event.sig = I2CMANAGER_SIG_RETRY_TIMER_EXPIRED;
            self->fsmState(self, &event);
        }
        else
        {
            self->fsmRepeatCount = 0;
            self->fsmTimer.flags.start = 0;
            self->fsmTimer.flags.active = 0;
            self->fsmTimer.flags.expired = 0;
            event.sig = I2CMANAGER_SIG_RETRY_LIMIT_REACHED;
            self->fsmState(self, &event);
        }
    }
    self->peripheralState = currentPeripheralState;

    /* Check for bus collision and try and recover. Usually when the BCL bit is 
    set, the SDA line stuck low. And the SDA line is usually stuck low because 
    a target device still has a hold of the SDA line when it's not supposed to. 
    Call the bus clear function to try and make the target device let go. Set 
    an error flag to notify the user. */
    if(I2C_GetBusError(self->peripheral))
    {
        /* Stop the transfer and set the transfer error flag */
        self->currentDataTransferFinished = true;
        self->currentDataTransferError = I2CMANAGER_TRANSFER_ERROR_BUS_COLLISION;
        event.sig = I2CMANAGER_SIG_SEND_STOP;
        self->fsmState(self, &event);
        self->managerState = I2CMANAGER_STATE_IDLE;
        /* Clear the target device's receive finished and transmit finished 
        flags. The receive finished flag is cleared by reading out the finished 
        data transfer. */
        bool tempBool;
        uint8_t *tempPtr;
        uint16_t tempLength;
        I2CTarget_GetFinishedDataTransfer(self->currentNode->i2cDevice, &tempBool, &tempPtr, &tempLength);
        I2CTarget_ClearDataTransferStartedFlag(self->currentNode->i2cDevice);
        /* Try to reset the target devices */
        I2CManager_BusClear(self);
        /* Send the transfer error flag to notify the user */
        if(self->transferErrorCallback != NULL)
        {
            self->transferErrorCallback(self->currentDataTransferError, 
                self, self->currentNode->i2cDevice);
        }
    }

    /* Go through list and process each targets data requests. */
    if(self->currentNode != NULL)
    {
        switch(self->managerState)
        {
            case I2CMANAGER_STATE_IDLE:
                if(I2CTarget_GetDataTransferBufferCount(self->currentNode->i2cDevice) > 0 && 
                    self->fsmState == I2CManager_FsmIdle)
                {
                    tempDataTransfer.length = 0;
                    I2CTarget_ReadFromDataTransferBuffer(self->currentNode->i2cDevice, 
                        (bool*)&(tempDataTransfer.transferType),
                        &(tempDataTransfer.ptrDataArray),
                        &(tempDataTransfer.length));

                    if(tempDataTransfer.length > 0)
                    {
                        I2CManager_BeginDataTransfer(self, &tempDataTransfer);
                        self->managerState = I2CMANAGER_STATE_TRANSFER_IN_PROGRESS;
                        I2CTarget_DataTransferStartedEvent(self->currentNode->i2cDevice);
                    }
                }
                else
                {
                    self->currentNode = self->currentNode->next;
                }
                break;
            case I2CMANAGER_STATE_TRANSFER_IN_PROGRESS:
                if(self->currentDataTransferFinished)
                {
                    /* Get the report of the current transfer and write the 
                    report to the manager. Then call the target device's 
                    finished transfer event function to finish the transfer. */
                    I2CManager_GenerateFinishedTransferReport(self, &tempStatusReport);
                    self->finishedTransferReport = tempStatusReport;
                    
                    I2CTarget_DataTransferFinishedEvent(self->currentNode->i2cDevice, 
                                                        tempStatusReport.isReadType, 
                                                        tempStatusReport.ptrArray, 
                                                        tempStatusReport.sizeOfArray);
                    /* Check if there was an error. Then check if there is more 
                    data to transfer. If there is more data to transfer, send a 
                    repeated start event to the state machine. */
                    if(self->currentDataTransferError != I2CMANAGER_TRANSFER_ERROR_NONE)
                    {
                        /* @follow-up decide if I want to keep trying to write more bytes if there 
                        is a read or write data error, or just give up. I think the best option is 
                        to go on the to the next device. If the user has the error callback 
                        implemented, they can decided how they want to handle it. - MS */

                        /* Go to the next device. */
                        event.sig = I2CMANAGER_SIG_SEND_STOP;
                        self->fsmState(self, &event);
                        self->currentNode = self->currentNode->next;
                    }
                    else if(I2CTarget_GetDataTransferBufferCount(self->currentNode->i2cDevice) > 0)
                    {
                        I2CTarget_DataTransferStartedEvent(self->currentNode->i2cDevice);
                        event.sig = I2CMANAGER_SIG_SEND_RESTART;
                        self->fsmState(self, &event);
                    }
                    else
                    {
                        /* Finished. Go to the next device. */
                        event.sig = I2CMANAGER_SIG_SEND_STOP;
                        self->fsmState(self, &event);
                        self->currentNode = self->currentNode->next;
                    }
                    self->managerState = I2CMANAGER_STATE_IDLE;
                }
                break;
        }
    }
    else
    {
        self->currentNode = self->currentNode->next;
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
    *retDevice = *(self->currentNode->i2cDevice);
}

// *****************************************************************************

/* @todo not sure if/how I want to implement this yet. Should status reflect 
on going changes during the data transfer, such as the current number of bytes
transferred? Or should I just use it as a end of data transfer report? - MS */
void I2CManager_GetDataTransferStatus(I2CManager *self, I2CManagerTransferStatus *retTransferStatus)
{
    retTransferStatus->error = self->finishedTransferReport.error;
    retTransferStatus->isReadType = self->finishedTransferReport.isReadType;
    retTransferStatus->ptrArray = self->finishedTransferReport.ptrArray;
    retTransferStatus->sizeOfArray = self->finishedTransferReport.sizeOfArray;
    retTransferStatus->numBytesTransferred = self->finishedTransferReport.numBytesTransferred;
}

// *****************************************************************************

I2CManagerTransferError I2CManager_GetTransferError(I2CManager *self)
{
    return self->currentDataTransferError;
}

// *****************************************************************************

void I2CManager_SetTransferErrorCallback(I2CManager *self, I2CManagerCallback Function)
{
    self->transferErrorCallback = Function;
}

// *****************************************************************************

void I2CManager_BusClear(I2CManager *self)
{
    /* @note If the SDA gets stuck low this usually means that a target device 
    still has ahold of the bus. This can happen if a transfer gets interrupted 
    during a read, or sometimes even by sending the wrong command to a target 
    device. Like a read instead of write. When this happens, toggling the SCL 
    line nine times will cause the device holding the SDA line to release. In 
    my experience, it doesn't really matter how fast you toggle the SCL line. 
    I've done it at MHz speed and the target device still reset. I've gone 
    ahead and added a simple delay to slow it down a little bit, since 
    technically the SCL line isn't supposed to go that fast even though the 
    target device had no problems with it.
    
    If the SCL line gets stuck low though, you will have to use a hardware 
    reset line to try and reset the target device, or cycle power to it. */

    I2C_Disable(self->peripheral);
    if(self->SetSCLPinDirection != NULL && self->SetSCLPinLevel != NULL)
    {
        self->SetSCLPinDirection(true);
        for(uint8_t i = 0; i < 10; i++)
        {
            uint16_t count = 1000;
            self->SetSCLPinLevel(false);
            while(count--);
            self->SetSCLPinLevel(true);
            count = 1000;
            while(count--);
        }
        self->SetSCLPinDirection(false);
    }
    I2C_Enable(self->peripheral);
}

// *****************************************************************************

void I2CManager_SetSDAPinDirectionFunc(I2CManager *self, void(*Function)(bool setOutput))
{
    self->SetSDAPinDirection = Function;
}

// *****************************************************************************

void I2CManager_SetSCLPinDirectionFunc(I2CManager *self, void(*Function)(bool setOutput))
{
    self->SetSCLPinDirection = Function;
}

// *****************************************************************************

void I2CManager_SetSDAPinLevelFunc(I2CManager *self, void(*Function)(bool setHigh))
{
    self->SetSDAPinLevel = Function;
}

// *****************************************************************************

void I2CManager_SetSCLPinLevelFunc(I2CManager *self, void(*Function)(bool setHigh))
{
    self->SetSCLPinLevel = Function;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Local Functions *****************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

static void I2CManager_PushNode(I2CManager_Node *self, I2CManager_Node *endOfList)
{
    /* Add the new entry to the beginning of the list. Make the "next" pointer
    point to the head */
    self->next = endOfList->next;
    /* Update the beginning of the list to point to the new beginning */
    endOfList->next = self;
}
// *****************************************************************************

static void I2CManager_DeleteNode(I2CManager_Node *node, I2CManager_Node *endOfList)
{
    if(endOfList == NULL)
        return;

    /* Store the beginning of the list in a temporary variable. Keep track of 
    previous entry in order to update nodes on either side of the key item. */
    I2CManager_Node *prev = endOfList, *temp = endOfList->next;
    while(temp != endOfList && temp != node)
    {
        prev = temp;
        temp = temp->next;
    }
    /* Make the previous entry point to the one after the next entry. But 
    first check if the key was at the end of the list and update the endOfList 
    pointer if needed. */
    if(temp == node)
    {
        if(temp->next == prev->next) // list size = 1
            endOfList = NULL;
        else if(temp == endOfList)
            endOfList = prev;

        prev->next = temp->next;
    }
}

// *****************************************************************************

static void I2CManager_BeginDataTransfer(I2CManager *self, DataTransfer *dtObject)
{
    if(self->fsmState != I2CManager_FsmIdle)
        return;

    /* Copy the data over to our temporary transfer buffer */
    if(dtObject->transferType == DATA_TRANSFER_TYPE_READ)
        self->currentDataTransfer.isReadType = true;
    else
        self->currentDataTransfer.isReadType = false;
    self->currentDataTransfer.ptrDataArray = dtObject->ptrDataArray;
    self->currentDataTransfer.length = dtObject->length;
    /* Begin data transfer */
    self->currentDataTransferFinished = false;
    self->currentDataTransferError = I2CMANAGER_TRANSFER_ERROR_NONE;
    I2CEvent event = {0}; // create an event to give to the state machine
    event.sig = I2CMANAGER_SIG_BEGIN_TRANSFER;
    self->fsmState(self, &event); // call the current state and pass the event
}

// *****************************************************************************

static void I2CManager_GenerateFinishedTransferReport(I2CManager *self, I2CManagerTransferStatus *retReport)
{
    retReport->error = self->currentDataTransferError;
    retReport->isReadType = self->currentDataTransfer.isReadType;
    retReport->ptrArray = self->currentDataTransfer.ptrDataArray;
    retReport->sizeOfArray = self->currentDataTransfer.length;
    if(retReport->isReadType == true)
        retReport->numBytesTransferred = self->readCount;
    else
        retReport->numBytesTransferred = self->writeCount;
}

// *****************************************************************************

static void I2CManager_SetFSMTimerRepeat(I2CManager *self, uint8_t numRetry)
{
    if(self->fsmTimer.flags.active == 0)
    {
        self->fsmRepeatLimit = numRetry;
        self->fsmRepeatCount = 0;
    }
}

// *****************************************************************************

static void I2CManager_StartFSMTimer(I2CManager *self, uint32_t period)
{
    self->fsmTimer.period = period;
    self->fsmTimer.flags.start = 1;
}

// *****************************************************************************

static void I2CManager_ClearFSMTimer(I2CManager *self)
{
    self->fsmTimer.flags.start = 0;
    self->fsmTimer.flags.active = 0;
    self->fsmTimer.flags.expired = 0;
    self->fsmRepeatCount = 0;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** States **************************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

static void I2CManager_FsmIdle(I2CManager *self, const I2CEvent *e)
{
    I2CEvent action = {(I2CSignal)(0), (I2CFSMState)(0)};

    switch(e->sig)
    {
        case I2CMANAGER_SIG_BEGIN_TRANSFER:
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
            action.sig = I2CMANAGER_SIG_ENTER;
            self->fsmState(self, &action);
            break;
        default:
            break;
    }
}

// *****************************************************************************

static void I2CManager_FsmStart(I2CManager *self, const I2CEvent *e)
{
    I2CEvent action = {(I2CSignal)(0), I2CManager_FsmStart};

    switch(e->sig)
    {
        case I2CMANAGER_SIG_ENTER:
            I2C_Start(self->peripheral);
            if(e->callingState != NULL && e->callingState != I2CManager_FsmStart)
                I2CManager_SetFSMTimerRepeat(self, DEFAULT_REPEAT_LIMIT);
            I2CManager_StartFSMTimer(self, self->fsmShortTimeoutPeriod);
            self->statusBits.sendingStart = 1;
            break;
        case I2CMANAGER_SIG_EXIT:
            I2CManager_ClearFSMTimer(self);
            break;
        case I2CMANAGER_SIG_BUS_IDLE_EVENT:
            /* Transition to next state. Perform our exit action, 
            then perform entry action. */
            action.sig = I2CMANAGER_SIG_EXIT;
            self->fsmState(self, &action);
            action.sig = I2CMANAGER_SIG_ENTER;
            self->fsmState = I2CManager_FsmWriteAddress;
            self->fsmState(self, &action);
            break;
        case I2CMANAGER_SIG_SEND_STOP:
            action.sig = I2CMANAGER_SIG_EXIT;
            self->fsmState(self, &action);
            action.sig = I2CMANAGER_SIG_ENTER;
            self->fsmState = I2CManager_FsmStop;
            self->fsmState(self, &action);
            break;
        case I2CMANAGER_SIG_RETRY_TIMER_EXPIRED:
            /* Perform our entry action again. The event's state member will 
            indicate that this is a re-entry.*/
            action.sig = I2CMANAGER_SIG_ENTER;
            self->fsmState(self, &action);
            break;
        case I2CMANAGER_SIG_RETRY_LIMIT_REACHED:
            /* Set a error flag then transition to stop state */
            self->currentDataTransferFinished = true;
            self->currentDataTransferError = I2CMANAGER_TRANSFER_ERROR_START_STOP;
            action.sig = I2CMANAGER_SIG_EXIT;
            self->fsmState(self, &action);
            /* Error event callback */
            if(self->transferErrorCallback != NULL)
            {
                self->transferErrorCallback(self->currentDataTransferError, 
                    self, self->currentNode->i2cDevice);
            }
            action.sig = I2CMANAGER_SIG_ENTER;
            self->fsmState = I2CManager_FsmStop;
            self->fsmState(self, &action);
            break;
        default:
            break;
    }
}

// *****************************************************************************

static void I2CManager_FsmWriteAddress(I2CManager *self, const I2CEvent *e)
{
    I2CEvent action = {(I2CSignal)(0), I2CManager_FsmWriteAddress};

    switch(e->sig)
    {
        case I2CMANAGER_SIG_ENTER:
        {
            /* Prepare to write or read data */
            uint8_t targetAddressPlusRW = 0;
            if(self->currentDataTransfer.isReadType == false)
            {
                targetAddressPlusRW = (self->currentNode->i2cDevice->targetAddress7Bit) << 1;
                self->writeCount = 0;
            }
            else
            {
                targetAddressPlusRW = ((self->currentNode->i2cDevice->targetAddress7Bit << 1) | 1);
                self->readCount = 0;
            }
            I2C_TransmitByte(self->peripheral, targetAddressPlusRW);
            /* Check if we re-entered this state or not */
            if(e->callingState != NULL && e->callingState != I2CManager_FsmWriteAddress)
                I2CManager_SetFSMTimerRepeat(self, DEFAULT_REPEAT_LIMIT);
            I2CManager_StartFSMTimer(self, self->fsmLongTimeoutPeriod);
            self->statusBits.transmitInProgress = 1;
            break;
        }
        case I2CMANAGER_SIG_EXIT:
            I2CManager_ClearFSMTimer(self);
            break;
        case I2CMANAGER_SIG_BUS_IDLE_EVENT:
            if(I2C_GetAckTargetStatus(self->peripheral))
            {
                /* Address acknowledged. Perform our exit action, 
                then transition to next state. */
                action.sig = I2CMANAGER_SIG_EXIT;
                self->fsmState(self, &action);
                if(self->currentDataTransfer.isReadType == true)
                {
                    /* Prepare to receive byte */
                    self->readCount = 0;
                    action.sig = I2CMANAGER_SIG_ENTER;
                    self->fsmState = I2CManager_FsmReadData;
                    self->fsmState(self, &action);
                }
                else
                {
                    /* Prepare to transmit the first byte */
                    self->writeCount = 0;
                    action.sig = I2CMANAGER_SIG_ENTER;
                    self->fsmState = I2CManager_FsmWriteData;
                    self->fsmState(self, &action);
                }
            }
            else
            {
                /* Nack. Force a timer expired event immediately by setting 
                the expired flag. This will increment the retry counter. */
                self->fsmTimer.flags.active = 0;
                self->fsmTimer.flags.expired = 1;
            }
            break;
        case I2CMANAGER_SIG_SEND_STOP:
            action.sig = I2CMANAGER_SIG_EXIT;
            self->fsmState(self, &action);
            action.sig = I2CMANAGER_SIG_ENTER;
            self->fsmState = I2CManager_FsmStop;
            self->fsmState(self, &action);
            break;
        case I2CMANAGER_SIG_RETRY_TIMER_EXPIRED:
            /* Perform our entry action again. The event's state member will 
            indicate that this is a re-entry.*/
            action.sig = I2CMANAGER_SIG_ENTER;
            self->fsmState(self, &action);
            break;
        case I2CMANAGER_SIG_RETRY_LIMIT_REACHED:
            /* Set the flag to tell the manager that we are done and set the 
            error flag. */
            self->currentDataTransferFinished = true;
            self->currentDataTransferError = I2CMANAGER_TRANSFER_ERROR_ADDRESS;
            action.sig = I2CMANAGER_SIG_EXIT;
            self->fsmState(self, &action);
            /* Error event callback */
            if(self->transferErrorCallback != NULL)
            {
                self->transferErrorCallback(self->currentDataTransferError, 
                    self, self->currentNode->i2cDevice);
            }
            break;
        default:
            break;
    }
}

// *****************************************************************************

static void I2CManager_FsmWriteData(I2CManager *self, const I2CEvent *e)
{
    I2CEvent action = {(I2CSignal)(0), I2CManager_FsmWriteData};

    switch(e->sig)
    {
        case I2CMANAGER_SIG_ENTER:
            I2C_TransmitByte(self->peripheral, self->currentDataTransfer.ptrDataArray[self->writeCount]);
            /* Check if we re-entered this state or not */
            if(e->callingState != NULL && e->callingState != I2CManager_FsmWriteAddress)
                I2CManager_SetFSMTimerRepeat(self, DEFAULT_REPEAT_LIMIT);
            I2CManager_StartFSMTimer(self, self->fsmLongTimeoutPeriod);
            self->statusBits.transmitInProgress = 1;
            break;
        case I2CMANAGER_SIG_EXIT:
            I2CManager_ClearFSMTimer(self);
            break;
        case I2CMANAGER_SIG_BUS_IDLE_EVENT:
            if(self->currentDataTransfer.isReadType == false)
            {
                if(I2C_GetAckTargetStatus(self->peripheral))
                {
                    /* Data acknowledged. Check if there are more bytes to send */
                    self->writeCount++;
                    if(self->writeCount < self->currentDataTransfer.length)
                    {
                        /* Stay in this state and continue sending bytes. 
                        Perform exit action and re-enter to send next byte. */
                        action.sig = I2CMANAGER_SIG_EXIT;
                        self->fsmState(self, &action);
                        action.sig = I2CMANAGER_SIG_ENTER;
                        self->fsmState(self, &action);
                    }
                    else
                    {
                        /* We are finished with the transfer. Set the flag to 
                        tell the manager that we are done. The manager will 
                        tell us if we need to send a stop or a restart. */
                        self->currentDataTransferFinished = true;
                        self->currentDataTransferError = I2CMANAGER_TRANSFER_ERROR_NONE;
                        action.sig = I2CMANAGER_SIG_EXIT;
                        self->fsmState(self, &action);
                    }
                }
                else
                {
                    /* Nack. Force a timer expired event immediately by setting 
                    the expired flag. This will increment the retry counter. */
                    self->fsmTimer.flags.active = 0;
                    self->fsmTimer.flags.expired = 1;
                }
            }
            break;
        case I2CMANAGER_SIG_SEND_STOP:
            action.sig = I2CMANAGER_SIG_EXIT;
            self->fsmState(self, &action);
            action.sig = I2CMANAGER_SIG_ENTER;
            self->fsmState = I2CManager_FsmStop;
            self->fsmState(self, &action);
            break;
        case I2CMANAGER_SIG_SEND_RESTART:
            action.sig = I2CMANAGER_SIG_EXIT;
            self->fsmState(self, &action);
            action.sig = I2CMANAGER_SIG_ENTER;
            self->fsmState = I2CManager_FsmRestart;
            self->fsmState(self, &action);
            break;
        case I2CMANAGER_SIG_RETRY_TIMER_EXPIRED:
            /* Perform our entry action again. The event's state 
            member will indicate that this is a re-entry. */
            action.sig = I2CMANAGER_SIG_ENTER;
            self->fsmState(self, &action);
            break;
        case I2CMANAGER_SIG_RETRY_LIMIT_REACHED:
            /* Set the flag to tell the manager that we are done and set the 
            error flag. */
            self->currentDataTransferFinished = true;
            self->currentDataTransferError = I2CMANAGER_TRANSFER_ERROR_WRITE;
            action.sig = I2CMANAGER_SIG_EXIT;
            self->fsmState(self, &action);
            /* Error event callback */
            if(self->transferErrorCallback != NULL)
            {
                self->transferErrorCallback(self->currentDataTransferError, 
                    self, self->currentNode->i2cDevice);
            }
            break;
        default:
            break;
    }
}

// *****************************************************************************

static void I2CManager_FsmReadData(I2CManager *self, const I2CEvent *e)
{
    I2CEvent action = {(I2CSignal)(0), I2CManager_FsmReadData};

    switch(e->sig)
    {
        case I2CMANAGER_SIG_ENTER:
            I2C_ReceiveByte(self->peripheral);
            /* Check if we re-entered this state or not */
            if(e->callingState != NULL && e->callingState != I2CManager_FsmReadData)
                I2CManager_SetFSMTimerRepeat(self, DEFAULT_REPEAT_LIMIT);
            I2CManager_StartFSMTimer(self, self->fsmLongTimeoutPeriod);
            self->statusBits.receiveInProgress = 1;
            break;
        case I2CMANAGER_SIG_EXIT:
            I2CManager_ClearFSMTimer(self);
            break;
        case I2CMANAGER_SIG_DATA_RECEIVED:
            if(self->currentDataTransfer.isReadType == true) // @follow-up is this extra check needed? - MS
            {
                self->currentDataTransfer.ptrDataArray[self->readCount++] = I2C_GetReceivedByte(self->peripheral);
                if(self->readCount < self->currentDataTransfer.length)
                    I2C_SendAck(self->peripheral, true);
                else
                    I2C_SendAck(self->peripheral, false); // send NACK on last byte
                self->fsmTimer.period = self->fsmShortTimeoutPeriod;
                self->fsmTimer.flags.start = 1; // restart
                self->statusBits.sendingAck = 1;
            }
            break;
        case I2CMANAGER_SIG_BUS_IDLE_EVENT:
            /* Send ack finished. Prepare to receive byte if there are more 
            bytes to read. Otherwise, end transfer. */
            if(self->readCount < self->currentDataTransfer.length)
            {
                /* Re-enter this state. Perform exit action and entry action */
                action.sig = I2CMANAGER_SIG_EXIT;
                self->fsmState(self, &action);
                action.sig = I2CMANAGER_SIG_ENTER;
                self->fsmState(self, &action);
            }
            else
            {
                /* We are finished with the transfer. Set the flag to tell the 
                manager process that we are done. The manager process will 
                tell us if we need to send a stop or a restart. */
                self->currentDataTransferFinished = true;
                self->currentDataTransferError = I2CMANAGER_TRANSFER_ERROR_NONE;
                action.sig = I2CMANAGER_SIG_EXIT;
                self->fsmState(self, &action);
            }
            break;
        case I2CMANAGER_SIG_SEND_STOP:
            action.sig = I2CMANAGER_SIG_EXIT;
            self->fsmState(self, &action);
            action.sig = I2CMANAGER_SIG_ENTER;
            self->fsmState = I2CManager_FsmStop;
            self->fsmState(self, &action);
            break;
        case I2CMANAGER_SIG_SEND_RESTART:
            action.sig = I2CMANAGER_SIG_EXIT;
            self->fsmState(self, &action);
            action.sig = I2CMANAGER_SIG_ENTER;
            self->fsmState = I2CManager_FsmRestart;
            self->fsmState(self, &action);
            break;
        case I2CMANAGER_SIG_RETRY_TIMER_EXPIRED:
            /* Perform our entry action again. The event's state 
            member will indicate that this is a re-entry. */
            action.sig = I2CMANAGER_SIG_ENTER;
            self->fsmState(self, &action);
            break;
        case I2CMANAGER_SIG_RETRY_LIMIT_REACHED:
            /* Set the flag to tell the manager that we are done and set the 
            error flag. */
            self->currentDataTransferFinished = true;
            self->currentDataTransferError = I2CMANAGER_TRANSFER_ERROR_READ;
            action.sig = I2CMANAGER_SIG_EXIT;
            self->fsmState(self, &action);
            /* Error event callback */
            if(self->transferErrorCallback != NULL)
            {
                self->transferErrorCallback(self->currentDataTransferError, 
                    self, self->currentNode->i2cDevice);
            }
            break;
        default:
            break;
    }
}

// *****************************************************************************

static void I2CManager_FsmStop(I2CManager *self, const I2CEvent *e)
{
    I2CEvent action = {(I2CSignal)(0), I2CManager_FsmStop};

    switch(e->sig)
    {
        case I2CMANAGER_SIG_ENTER:
            I2C_Stop(self->peripheral);
            /* Check if we re-entered this state or not */
            if(e->callingState != NULL && e->callingState != I2CManager_FsmStop)
                I2CManager_SetFSMTimerRepeat(self, 1);
            I2CManager_StartFSMTimer(self, self->fsmShortTimeoutPeriod);
            self->statusBits.repeatedStartSent = 0;
            self->statusBits.sendingStop = 1;
            break;
        case I2CMANAGER_SIG_EXIT:
            I2CManager_ClearFSMTimer(self);
            break;
        case I2CMANAGER_SIG_BUS_IDLE_EVENT:
            /* Stop is finished. Transition back to idle state. Perform exit 
            action. No entry action. */
            action.sig = I2CMANAGER_SIG_EXIT;
            self->fsmState(self, &action);
            self->fsmState = I2CManager_FsmIdle;
            break;
        case I2CMANAGER_SIG_RETRY_TIMER_EXPIRED:
            /* Perform our entry action again. The event's state 
            member will indicate that this is a re-entry. */
            action.sig = I2CMANAGER_SIG_ENTER;
            self->fsmState(self, &action);
            break;
        case I2CMANAGER_SIG_RETRY_LIMIT_REACHED:
            /* Sending a stop failed somehow. Set an error flag then transition 
            to idle state. Perform exit action. No entry action.*/
            self->currentDataTransferFinished = true;
            self->currentDataTransferError = I2CMANAGER_TRANSFER_ERROR_UNKOWN;
            action.sig = I2CMANAGER_SIG_EXIT;
            self->fsmState(self, &action);
            /* Error event callback */
            if(self->transferErrorCallback != NULL)
            {
                self->transferErrorCallback(self->currentDataTransferError, 
                    self, self->currentNode->i2cDevice);
            }
            self->fsmState = I2CManager_FsmIdle;
            break;
        case I2CMANAGER_SIG_SEND_STOP:
            /* Something went wrong and the manager requested a stop. We are 
            already in stop, so perform one last stop action then transition
            back to idle. Perform exit action. No entry action. */
            I2C_Stop(self->peripheral);
            action.sig = I2CMANAGER_SIG_EXIT;
            self->fsmState(self, &action);
            self->fsmState = I2CManager_FsmIdle;
            break;
        default:
            break;
    }
}

// *****************************************************************************

static void I2CManager_FsmRestart(I2CManager *self, const I2CEvent *e)
{
    I2CEvent action = {(I2CSignal)(0), I2CManager_FsmRestart};

    switch(e->sig)
    {
        case I2CMANAGER_SIG_ENTER:
            I2C_Restart(self->peripheral);
            /* Check if we re-entered this state or not */
            if(e->callingState != NULL && e->callingState != I2CManager_FsmRestart)
                I2CManager_SetFSMTimerRepeat(self, DEFAULT_REPEAT_LIMIT);
            I2CManager_StartFSMTimer(self, self->fsmShortTimeoutPeriod);
            self->statusBits.repeatedStartSent = 0;
            self->statusBits.sendingRestart = 1;
            break;
        case I2CMANAGER_SIG_EXIT:
            I2CManager_ClearFSMTimer(self);
            break;
        case I2CMANAGER_SIG_BUS_IDLE_EVENT:
            /* Repeat start is finished */
            action.sig = I2CMANAGER_SIG_EXIT;
            self->fsmState(self, &action);
            self->statusBits.repeatedStartSent = 1;
            self->fsmState = I2CManager_FsmIdle;
            break;
        case I2CMANAGER_SIG_RETRY_TIMER_EXPIRED:
            /* Perform our entry action again. The event's state 
            member will indicate that this is a re-entry. */
            action.sig = I2CMANAGER_SIG_ENTER;
            self->fsmState(self, &action);
            break;
        case I2CMANAGER_SIG_RETRY_LIMIT_REACHED:
            /* Set a error flag then transition to stop state */
            self->currentDataTransferFinished = true;
            self->currentDataTransferError = I2CMANAGER_TRANSFER_ERROR_START_STOP;
            action.sig = I2CMANAGER_SIG_EXIT;
            self->fsmState(self, &action);
            /* Error event callback */
            if(self->transferErrorCallback != NULL)
            {
                self->transferErrorCallback(self->currentDataTransferError, 
                    self, self->currentNode->i2cDevice);
            }
            action.sig = I2CMANAGER_SIG_ENTER;
            self->fsmState = I2CManager_FsmStop;
            self->fsmState(self, &action);
            break;
        default:
            break;
    }
}

/*
 End of File
 */
