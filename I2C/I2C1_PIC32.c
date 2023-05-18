/***************************************************************************//**
 * @brief Basic I2C Peripheral for PIC32
 * 
 * @file I2C1_PIC32.c
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 10/2/16   Original Creation
 * @date 2/21/22   Added Doxygen
 * 
 * @details
 *      TODO
 * 
 * @section license License
 * SPDX-FileCopyrightText: © 2016 Matthew Spinks
 * SPDX-License-Identifier: Zlib
 * 
 * This software is released under the Zlib license. You are free alter and
 * redistribute it, but you must not misrepresent the origin of the software.
 * This notice may not be removed. <http://www.zlib.net/zlib_license.html>
 * 
*******************************************************************************/

#include "I2C1.h"
#include "app.h"
#include <math.h> // only needed if we want to compute baud rate
#include <xc.h>

// ***** Defines ***************************************************************

// default values defined by the user
#define PBCLK_FREQ          40000000UL // in Hz
#define TIMEOUT_PERIOD_US   500 // desired timeout period in us
#define RETRY_COUNT 5 // how many times to retry a command before failing

/* C macros for computing period value for timeout. The source clock frequency
 * is basically how often you will call the function to update the state 
 * machine. You can set it to any value you want in Hz. If you update the FSM 
 * once per loop, just leave it the same as the source clock. The timeout value 
 * in us does not have to be very precise. It is just a rough number used for 
 * the FSM to determine if something went wrong. I have it set to 1 millisecond 
 * by default.
 */
#define TIMOUT_SRC_CLK_FREQ     PBCLK_FREQ // in Hz
#define TIMEOUT_SRC_CLK_US      (1.0 / TIMOUT_SRC_CLK_FREQ * 1000000.0)
#define TIMEOUT_PERIOD_COUNT    (TIMEOUT_PERIOD_US / TIMEOUT_SRC_CLK_US)

#define DEFAULT_BRG_VALUE       0x2C


// ***** Global Variables ******************************************************

// local function pointers
void (*I2C1_TransmitFinishedCallback)(void);
void (*I2C1_ReceiveInterruptCallback)(void);

// Free timer (with bit field)
typedef struct
{
    uint16_t period;
    uint16_t count;
    uint8_t retryCount;
    
    // bit field
    union {
        struct {
            unsigned start          :1;
            unsigned active         :1;
            unsigned expired        :1;
            unsigned retryFinished  :1;
            unsigned                :0; // fill to nearest byte
        };
    } flags;
} FsmTimer;

/* A lot of the enable bits are cleared automatically and they are cleared
 too quickly for me to catch just by reading the register. These status bits
 will assist us by telling us what the state machine is currently trying to do. 
 They will get set anytime a function like start, stop, or restart is called. 
 Then we can check these values against the current status of the I2C registers 
 to see if that action has finished */
typedef struct
{
    union {
        struct {
            unsigned sendingStart   :1;
            unsigned sendingRestart :1;
            unsigned sendingStop    :1;
            unsigned sendingAck     :1;
            unsigned receiveInProgress  :1;
            unsigned transmitInProgress :1;
            unsigned :2;
        };
        struct {
            unsigned all :8;
        };
    };
} StatusBits;

typedef uint8_t Signal;
typedef struct Fsm Fsm;
typedef struct Event Event;

// This is the function pointer type for the state machine functions
typedef void (*State)(Event *e);

enum Signal
{
    BEGIN_TRANSFER = 1,
    BUS_IDLE_EVENT,
    ACK_RECEIVED,
    DATA_RECEIVED,
    TIMEOUT,
};

struct Fsm
{
    State state;
    StatusBits statusBits;
};

struct Event
{
    /** Tells the state what kind of event just happened. Uses the enumerated 
     * type called Signal*/
    Signal sig;
    
//    /** If there is a received packet, place it in here and pass it to the 
//     * state machine*/
//    uint8_t readBuffer[RX_BUF_SIZE];
    
    // Yo dawg. I heard you liked structs...
    struct
    {
        uint8_t slaveAddress;
        bool generateRepeatedStart; // at the end of transfer
        bool repeatedStart;         // repeated start has been performed
        bool masterRead;            // go into read state after sending address
        
        /* TODO Decide if data should be copied into an array and given to the 
         * state machine or if we want to pass by reference. Copying data may 
         * be the safest route, because if the something goes wrong in the FSM
         * and the user doesn't wait and check it, then the data could change */
        //uint8_t writeBuffer[TX_BUF_SIZE];
        //uint8_t writeSize;        
        
    } private;
    /** These variables should be treated as private and not modified directly
     * without the use of a function. Unfortunately, this is C, so I really 
     * can't stop you.*/
};

static FsmTimer I2C1_FsmTimer, I2C1_WaitTimer;
static Fsm I2C1_Fsm;
static Event I2C1_Event;
static bool isBusBusy, prevBusBusy, currentAckStat, prevAckStat;

// TODO decide if I want to use a separate static I2C object or just use 
// the I2C slave object
static I2CObject I2C1_I2CObject;
static I2CObject *ptrI2CSlave;

// ***** Static Function Prototypes ********************************************

// states
static void I2C1_FsmIdle(Event *e);
static void I2C1_FsmStart(Event *e);
static void I2C1_FsmWriteAddress(Event *e);
static void I2C1_FsmWriteData(Event *e);
static void I2C1_FsmStop(Event *e);
static void I2C1_FsmRestart(Event *e);
static void I2C1_FsmReadData(Event *e);

// private functions
static void I2C1_TimerStart(void);
static void I2C1_TimerStop(void);
//static void I2C1_Wait(void); // TODO deprecated. Try using bus idle instead
static StatusBits I2C1_GetStatusBits(void);

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** I2C Interface *******************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

// If your processor has multiple I2C peripherals, this function should only
// be defined once.
#ifndef I2C_OBJ
#define I2C_OBJ

void I2C_InitObject(I2CObject *self, uint8_t slaveAddress, uint8_t *writeBuffer, uint8_t *readBuffer)
{
    self->slaveAddress = slaveAddress;
    self->writeBuffer = writeBuffer;
    self->readBuffer = readBuffer;
}

#endif // I2C_OBJ

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** State Machine *******************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

static void I2C1_FsmIdle(Event *e)
{
    switch(e->sig)
    {
        case BEGIN_TRANSFER:
            if(e->private.repeatedStart)
            {
                // skip the start and go straight to the address
                e->private.repeatedStart = false;
                I2C1_TransmitByte(e->private.slaveAddress);
                I2C1_TimerStart();
                I2C1_Fsm.state = I2C1_FsmWriteAddress;
            }
            else
            {
                I2C1_Start(); // send I2C start command
                I2C1_TimerStart();
                I2C1_Fsm.state = I2C1_FsmStart;
            }
            break;
    }
}

static void I2C1_FsmStart(Event *e)
{
    switch(e->sig)
    {
        case BUS_IDLE_EVENT:
            I2C1_TimerStop();
            // send slave address
            // The address stored in the event has already had been manipulated 
            // during the write setup, so that we don't need to do it here.
            I2C1_TransmitByte(e->private.slaveAddress);
            I2C1_TimerStart();
            I2C1_Fsm.state = I2C1_FsmWriteAddress;
            break;
    }
}

static void I2C1_FsmWriteAddress(Event *e)
{
    switch(e->sig)
    {
        case BUS_IDLE_EVENT:
            I2C1_TimerStop();
            if(I2C1_GetAckStatus())
            {
                // address acknowledged.
                if(e->private.masterRead)
                {
                    // prepare to receive byte
                    I2C1_ReceiveEnable();
                    I2C1_Fsm.state = I2C1_FsmReadData;
                }
                else
                {
                    // load first data byte
                    I2C1_TransmitByte(ptrI2CSlave->writeBuffer[ptrI2CSlave->private.writeCount]);
                    I2C1_Fsm.state = I2C1_FsmWriteData;
                }
                I2C1_TimerStart();
            }
            break;
    }   
}

static void I2C1_FsmWriteData(Event *e)
{
    switch(e->sig)
    {
        case BUS_IDLE_EVENT:
            I2C1_TimerStop();
            if(I2C1_GetAckStatus())
            {
                // data acknowledged. Check if there are more bytes to send
                ptrI2CSlave->private.writeCount++;
                if(ptrI2CSlave->private.writeCount < ptrI2CSlave->numBytesToSend)
                {
                    I2C1_TransmitByte(ptrI2CSlave->writeBuffer[ptrI2CSlave->private.writeCount]);
                    I2C1_TimerStart();
                    // stay in this state and continue sending bytes
                }
                else
                {
                    // We are finished sending bytes. Check to see if we should
                    // generate a repeated start
                    if(e->private.generateRepeatedStart)
                    {
                        I2C1_Restart(); // send repeated start command
                        I2C1_TimerStart();
                        I2C1_Fsm.state = I2C1_FsmRestart;
                    }
                    else
                    {
                        I2C1_Stop(); // send stop command
                        I2C1_TimerStart();
                        I2C1_Fsm.state = I2C1_FsmStop;
                    }
                }
            }
            break;
    }
}

static void I2C1_FsmStop(Event *e)
{
    switch(e->sig)
    {
        case BUS_IDLE_EVENT:
            I2C1_TimerStop();
            // Stop is finished.
            I2C1_Fsm.state = I2C1_FsmIdle;
            break;
    }
}

static void I2C1_FsmRestart(Event *e)
{
    switch(e->sig)
    {
        case BUS_IDLE_EVENT:
            I2C1_TimerStop();
            // restart is finished.
            e->private.generateRepeatedStart = false;
            e->private.repeatedStart = true;
            I2C1_Fsm.state = I2C1_FsmIdle;
            break;
    }
}

static void I2C1_FsmReadData(Event *e)
{
     switch(e->sig)
    {
        case DATA_RECEIVED:
            I2C1_TimerStop();
            ptrI2CSlave->readBuffer[ptrI2CSlave->private.readCount++] = I2C1_GetReceivedByte();
            if(ptrI2CSlave->private.readCount < ptrI2CSlave->numBytesToRead)
            {
                I2C1_SendAck(true);
            }
            else
            {
                I2C1_SendAck(false); // send NACK on last byte
            }
            I2C1_TimerStart();
            break;
        case BUS_IDLE_EVENT:
            I2C1_TimerStop();
            // prepare to receive byte
            if(ptrI2CSlave->private.readCount < ptrI2CSlave->numBytesToRead)
            {
                I2C1_ReceiveEnable();
                I2C1_TimerStart();
            }
            else
            {
                I2C1_Stop();
                I2C1_TimerStart();
                I2C1_Fsm.state = I2C1_FsmStop;
            }
            break;
    }
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** State Machine Helper Functions **************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void I2C1_FsmInit(uint16_t tickRateInNs, uint16_t timeoutInUs)
{
    I2C1_Event.sig = 0;
    I2C1_Fsm.state = I2C1_FsmIdle;
    
    // TODO finish setting up timer using variables given
    I2C1_FsmTimer.period = (uint16_t)TIMEOUT_PERIOD_COUNT;
}

void I2C1_FsmProcess(void)
{
// ----- Check for Active Timers -----------------------------------------------
    
    if(I2C1_FsmTimer.flags.start)
    {
        I2C1_FsmTimer.flags.start = 0;
        I2C1_FsmTimer.count = TIMEOUT_PERIOD_COUNT;
        I2C1_FsmTimer.retryCount = RETRY_COUNT;
        I2C1_FsmTimer.flags.active = 1;
    }
    
// ----- Update Active Timers --------------------------------------------------
    
    if(I2C1_FsmTimer.flags.active)
    {
        I2C1_FsmTimer.count--;
        if(I2C1_FsmTimer.count == 0)
        {
            I2C1_FsmTimer.retryCount--;
            if(I2C1_FsmTimer.retryCount == 0)
            {
                I2C1_FsmTimer.flags.active = 0;
                I2C1_FsmTimer.flags.expired = 1;
                I2C1_FsmTimer.flags.retryFinished = 1;
            }
        }
    }
    
// ----- Check for I2C Events --------------------------------------------------
    
    StatusBits currentStatus;
    currentStatus = I2C1_GetStatusBits();
    
    if(I2C1_Fsm.statusBits.sendingStart && !currentStatus.sendingStart)
    {
        I2C1_Fsm.statusBits.sendingStart = 0;
        I2C1_Event.sig = BUS_IDLE_EVENT;
        I2C1_Fsm.state(&I2C1_Event); // call the current state and pass the event
    }
    if(I2C1_Fsm.statusBits.sendingRestart && !currentStatus.sendingRestart)
    {
        I2C1_Fsm.statusBits.sendingRestart = 0;
        I2C1_Event.sig = BUS_IDLE_EVENT;
        I2C1_Fsm.state(&I2C1_Event); // call the current state and pass the event
    }
    if(I2C1_Fsm.statusBits.sendingStop && !currentStatus.sendingStop)
    {
        I2C1_Fsm.statusBits.sendingStop = 0;
        I2C1_Event.sig = BUS_IDLE_EVENT;
        I2C1_Fsm.state(&I2C1_Event); // call the current state and pass the event
    }
    if(I2C1_Fsm.statusBits.sendingAck && !currentStatus.sendingAck)
    {
        I2C1_Fsm.statusBits.sendingAck = 0;
        I2C1_Event.sig = BUS_IDLE_EVENT;
        I2C1_Fsm.state(&I2C1_Event); // call the current state and pass the event
    }
    if(I2C1_Fsm.statusBits.transmitInProgress && !currentStatus.transmitInProgress)
    {
        // Be careful, with the PIC it will clear the transmit finished flag
        // before all 8 bits have been shifted out. So we must check for an 
        // empty transmit register as well before clearing our flag.
        if(!I2C1_IsTransmitRegisterFull())
        {
            I2C1_Fsm.statusBits.transmitInProgress = 0;
            I2C1_Event.sig = BUS_IDLE_EVENT;
            I2C1_Fsm.state(&I2C1_Event); // call the current state and pass the event
        }
    }
    else if(I2C1_Fsm.statusBits.receiveInProgress && !currentStatus.receiveInProgress)
    {
        if(I2C1_IsReceivedDataAvailable())
        {
            I2C1_Fsm.statusBits.receiveInProgress = 0;
            I2C1_Event.sig = DATA_RECEIVED;
            I2C1_Fsm.state(&I2C1_Event);
        }
    }
    else if(I2C1_FsmTimer.flags.expired)
    {
        I2C1_FsmTimer.flags.expired = 0;
        I2C1_Event.sig = TIMEOUT;
        // TODO This is temporary! I removed during troubleshooting with 
        // logic analyzer. I don't think the timeout value is long enough.
        // So I need to do some measurements first
        //I2C1_Fsm.state(&I2C1_Event); 
    }
    else if(I2C1_FsmTimer.flags.retryFinished)
    {
        // We've sent the event to retry the command multiple times and it
        // has failed
        I2C1_FsmTimer.flags.retryFinished = 0;
        
        // TODO stop everything
    }
}

void I2C1_FsmMasterWrite(I2CObject *self, uint8_t *writeData, uint8_t numBytes, bool repeatedStart)
{
    // TODO setup a lock to keep this function from being called if it's
    // already running. Decide if I want to make the return type a bool
    if(I2C1_Fsm.state != I2C1_FsmIdle)
        return;
    
    // TODO decide if I want to use a separate static I2C object or just use 
    // the I2C slave object
    ptrI2CSlave = self;
    ptrI2CSlave->writeBuffer = writeData;
    ptrI2CSlave->numBytesToSend = numBytes;
    ptrI2CSlave->private.writeCount = 0;
    
    // create an event to give to the state machine
    I2C1_Event.private.masterRead = false;
    I2C1_Event.private.slaveAddress = self->slaveAddress << 1; // clear R/W bit
    I2C1_Event.private.generateRepeatedStart = repeatedStart;
    I2C1_Event.sig = BEGIN_TRANSFER;
    I2C1_Fsm.state(&I2C1_Event); // call the current state and pass the event
}

void I2C1_FsmMasterRead(I2CObject *self, uint8_t *readData, uint8_t numBytes)
{
    // TODO setup a lock to keep this function from being called if it's
    // already running. Decide if I want to make the return type a bool
    if(I2C1_Fsm.state != I2C1_FsmIdle)
        return;
    
    // TODO decide if I want to use a separate static I2C object or just use 
    // the I2C slave object
    ptrI2CSlave = self;
    ptrI2CSlave->readBuffer = readData;
    ptrI2CSlave->numBytesToRead = numBytes;
    ptrI2CSlave->private.readCount = 0;
    
    // create an event to give to the state machine
    I2C1_Event.private.masterRead = true;
    I2C1_Event.private.slaveAddress = ((self->slaveAddress << 1) | 1);
    I2C1_Event.sig = BEGIN_TRANSFER;
    I2C1_Fsm.state(&I2C1_Event); // call the current state and pass the event
}

bool I2C1_FsmIsIdle(void)
{
    if(I2C1_Fsm.state == I2C1_FsmIdle)
        return true;
    else
        return false;
}

void I2C1_FsmGetData(uint8_t *numBytesWritten, uint8_t *numBytesRead, I2CObject *context)
{
    // TODO decide if I want to use a separate static I2C object or just use 
    // the I2C slave object
    *numBytesWritten = ptrI2CSlave->private.writeCount;
    *numBytesRead = ptrI2CSlave->private.readCount;
    context = ptrI2CSlave;
}

static void I2C1_TimerStart(void)
{
    I2C1_FsmTimer.flags.start = 1;
}

static void I2C1_TimerStop(void)
{
    I2C1_FsmTimer.flags.active = 0;
}

static StatusBits I2C1_GetStatusBits(void)
{
    StatusBits retValue;
    retValue.sendingStart = I2C1CONbits.SEN;
    retValue.sendingRestart = I2C1CONbits.RSEN;
    retValue.sendingStop = I2C1CONbits.PEN;
    retValue.sendingAck = I2C1CONbits.ACKEN;
    retValue.receiveInProgress = I2C1CONbits.RCEN;
    retValue.transmitInProgress = I2C1STATbits.TRSTAT;
    return retValue;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Peripheral Functions ************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

bool I2C1_InitWithFrequencies(float pbclkInMHz, uint16_t baudInKHz)
{
    // TODO check bounds
    if(pbclkInMHz == 0)
    {
        return true;
    }

    /* Calculate baud rate. Use the ceiling function to round up in the 
     * second step to get our register value. This will make the actual 
     * baud rate slightly slower than the exact value. It is better to be 
     * slightly slower, rather than faster, than the computed bus speed. 
     * Equation 24-1. Datasheet section 24, page 19 */
    uint32_t brgValue = 1000000 / (2 * baudInKHz) - 104;
    brgValue = (uint32_t)(ceil(brgValue / 1000.0 * pbclkInMHz - 2));
    bool temp = I2C1_InitWithBRGValue(brgValue);
    
    return temp;
}

bool I2C1_InitWithBRGValue(uint32_t brgValue)
{
    // If you call this function directly I must assume you know what you are 
    // doing and that you've already calculated the correct BRG value
    if(brgValue == 0x00 || brgValue == 0x01)
    {
        // You've chosen an incompatible number for the baud rate generator
        I2C1BRG = DEFAULT_BRG_VALUE;
        return true;
    }
    
    I2C1_Fsm.state = I2C1_FsmIdle;
    I2C1_Event.sig = 0;
    I2C1_FsmTimer.period = (uint16_t)TIMEOUT_PERIOD_COUNT;
    I2C1_FsmTimer.retryCount = RETRY_COUNT;
    
    I2C1BRG = (uint16_t)brgValue;
    I2C1CONbits.ON = 1; // enable I2C module
    return false;
}

bool I2C1_IsBusy(void)
{
    /* I2CxCON bits:
     * 0, SEN:   0 = start condition idle
     * 1, RSEN:  0 = restart condition idle
     * 2, PEN:   0 = stop condition idle
     * 3, RCEN:  0 = receive sequence not in progress
     * 4, ACKEN: 0 = acknowledge sequence idle
     * 
     * TRSTAT: 0 = master transmit not in progress
     **/
    bool retValue;
    if((I2C1CON & 0x0000001F)  || I2C1STATbits.TRSTAT)
        retValue = true;
    else
        retValue = false;
    
    return retValue;
}

void I2C1_Start(void)
{
    I2C1CONbits.SEN = 1; // cleared by module when finished
    I2C1_Fsm.statusBits.sendingStart = 1;
}

void I2C1_Stop(void)
{
    I2C1CONbits.PEN = 1; // cleared by module when finished
    I2C1_Fsm.statusBits.sendingStop = 1;
}

void I2C1_Restart(void)
{
    I2C1CONbits.RSEN = 1; // cleared by module when finished
    I2C1_Fsm.statusBits.sendingRestart = 1;
}

void I2C1_SendAck(bool ackOrNack)
{
    if(ackOrNack)
        I2C1CONbits.ACKDT = 0; // send ack
    else
        I2C1CONbits.ACKDT = 1; // send nack
    
    I2C1CONbits.ACKEN = 1; // cleared by module when finished
    I2C1_Fsm.statusBits.sendingAck = 1;
}

bool I2C1_GetAckStatus(void)
{
    if(I2C1STATbits.ACKSTAT == 1)
        return false; // acknowledge not received
    else
        return true;
}

void I2C1_ReceiveEnable(void)
{
    // RCEN bit is automatically cleared at the end of 8-bit receive data byte
    I2C1CONbits.RCEN = 1;
    I2C1_Fsm.statusBits.receiveInProgress = 1;
}

void I2C1_ReceiveDisable(void)
{
    // On this PIC, the receiver is disabled automatically after 8 bits have
    // been received.
}

bool I2C1_IsReceivedDataAvailable(void)
{
    if(I2C1STATbits.RBF == 1) // 1: receive complete
        return true; 
    else
        return false;
}

uint8_t I2C1_GetReceivedByte(void)
{
    return I2C1RCV;
}

void I2C1_TransmitEnable(void)
{
    // This PIC does not have a specific control bit to enable or disable
    // the I2C transmitter
}

void I2C1_TransmitDisable(void)
{
    // This PIC does not have a specific control bit to enable or disable
    // the I2C transmitter
}

void I2C1_TransmitByte(uint8_t dataToSend)
{
    I2C1TRN = dataToSend;
    I2C1_Fsm.statusBits.transmitInProgress = 1;
}

bool I2C1_IsTransmitRegisterFull(void)
{
    if(I2C1STATbits.TBF == 1)
        return true;
    else
        return false;
}

// *****************************************************************************

/*
 End of File
 */
