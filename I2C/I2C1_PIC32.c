/***************************************************************************//**
 * @brief Basic I2C Peripheral for PIC32
 * 
 * @file I2C1_PIC32.c
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 10/2/16   Original Creation (PIC32)
 * @date 2/21/22   Added Doxygen
 * 
 * @details
 *      @todo documentation
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
#include <math.h> // only needed if we want to compute baud rate
#include <xc.h>

// ***** Defines ***************************************************************

/* @remove old state machine defines later */
#define PBCLK_FREQ          40000000UL // in Hz
#define TIMEOUT_PERIOD_US   500 // desired timeout period in us

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

/* Registers */
#define I2CxCON      I2C1CON      // I2C control
#define I2CxCONbits  I2C1CONbits  // I2C control bits
#define I2CxSTAT     I2C1STAT     // I2C status
#define I2CxSTATbits I2C1STATbits // I2C status bits
#define I2CxBRG      I2C1BRG      // baud rate generator
#define I2CxRCV      I2C1RCV      // receive register
#define I2CxTRN      I2C1TRN      // transmit register
// interrupt status register

// ***** Global Variables ******************************************************

/* Assign functions to the interface */
I2CInterface I2C1_FunctionTable = {
    .I2C_ComputeBRGValue = I2C1_ComputeBRGValue,
    .I2C_Init = I2C1_Init,
    .I2C_Enable = I2C1_Enable,
    .I2C_Disable = I2C1_Disable,
    .I2C_ReceivedDataEvent = I2C1_ReceivedDataEvent,
    .I2C_GetReceivedByte = I2C1_GetReceivedByte,
    .I2C_IsReceiveRegisterFull = I2C1_IsReceiveRegisterFull,
    .I2C_IsReceiveUsingInterrupts = I2C1_IsReceiveUsingInterrupts,
    .I2C_ReceiveByte = I2C1_ReceiveByte,
    .I2C_ReceiveByteCancel = I2C1_ReceiveByteCancel,
    .I2C_TransmitRegisterEmptyEvent = I2C1_TransmitRegisterEmptyEvent,
    .I2C_TransmitByte = I2C1_TransmitByte,
    .I2C_IsTransmitRegisterEmpty = I2C1_IsTransmitRegisterEmpty,
    .I2C_IsTransmitFinished = I2C1_IsTransmitFinished,
    .I2C_IsTransmitUsingInterrupts = I2C1_IsTransmitUsingInterrupts,
    .I2C_PendingEventHandler = I2C1_PendingEventHandler,
    .I2C_SetTransmitRegisterEmptyCallback = I2C1_SetTransmitRegisterEmptyCallback,
    .I2C_SetReceivedDataCallback = I2C1_SetReceivedDataCallback,
    .I2C_Start = I2C1_Start,
    .I2C_Stop = I2C1_Stop,
    .I2C_Restart = I2C1_Restart,
    .I2C_SendAck = I2C1_SendAck,
    .I2C_IsBusy = I2C1_IsBusy,
    .I2C_GetState = I2C1_GetState,
    .I2C_GetStartStatus = I2C1_GetStartStatus,
    .I2C_GetStopStatus = I2C1_GetStopStatus,
    .I2C_GetRestartStatus = I2C1_GetRestartStatus,
    .I2C_GetAckSendStatus = I2C1_GetAckSendStatus,
    .I2C_GetAckSlaveStatus = I2C1_GetAckSlaveStatus,
    .I2C_GetReceiveEnableStatus = I2C1_GetReceiveEnableStatus,
};

static bool useRxInterrupt = false, useTxInterrupt = false; // @todo rx and tx interrupts

// @todo local function pointers
// static void (*TransmitRegisterEmptyCallback)(void);
// static void (*ReceivedDataCallback)(uint8_t (*CallToGetData)(void));

// ***** Static Function Prototypes ********************************************

/* Put static function prototypes here */


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

// @todo old init functions
// bool I2C1_InitWithFrequencies(float pbclkInMHz, uint16_t baudInKHz)
// {
//     if(pbclkInMHz == 0)
//     {
//         return true;
//     }

//     /* Calculate baud rate. Use the ceiling function to round up in the 
//      * second step to get our register value. This will make the actual 
//      * baud rate slightly slower than the exact value. It is better to be 
//      * slightly slower, rather than faster, than the computed bus speed. 
//      * Equation 24-1. Datasheet section 24, page 19 */
//     uint32_t brgValue = 1000000 / (2 * baudInKHz) - 104;
//     brgValue = (uint32_t)(ceil(brgValue / 1000.0 * pbclkInMHz - 2));
//     bool temp = I2C1_InitWithBRGValue(brgValue);
    
//     return temp;
// }

// bool I2C1_InitWithBRGValue(uint32_t brgValue)
// {
//     // If you call this function directly I must assume you know what you are 
//     // doing and that you've already calculated the correct BRG value
//     if(brgValue == 0x00 || brgValue == 0x01)
//     {
//         // You've chosen an incompatible number for the baud rate generator
//         I2C1BRG = DEFAULT_BRG_VALUE;
//         return true;
//     }
    
//     I2C1_Fsm.state = I2C1_FsmIdle;
//     I2C1_Event.sig = 0;
//     I2C1_FsmTimer.period = (uint16_t)TIMEOUT_PERIOD_COUNT;
//     I2C1_FsmTimer.retryCount = RETRY_COUNT;
    
//     I2C1BRG = (uint16_t)brgValue;
//     I2C1CONbits.ON = 1; // enable I2C module
//     return false;
// }

uint32_t I2C1_ComputeBRGValue(uint32_t desiredBaudRate, uint32_t pclkInHz)
{

}

void I2C1_Init(I2CInitType *params)
{
    if(params->BRGValue == 0)
        return;

    useRxInterrupt = params->useRxInterrupt;
    useTxInterrupt = params->useTxInterrupt;

    // disable interrupts

    /* @note Right now, I'm only implementing master I2C. - MS */
    I2CxCONbits.IPMIEN = 0; // do not ACK automatically (must be off if master)
    I2CxCONbits.STREN = 0; // clock stretch off
    I2CxCONbits.ACKEN = 0;
    I2CxCONbits.RCEN = 0;
    I2CxCONbits.PEN = 0;
    I2CxCONbits.RSEN = 0;
    I2CxCONbits.SEN = 0;

    I2CxBRG = params->BRGValue; // set baud rate generator

    // @todo enable any interrupts

    I2CxCONbits.I2CEN = 1; // enable peripheral
}

void I2C1_Enable(void)
{
    I2CxCONbits.I2CEN = 1;
}

void I2C1_Disable(void)
{
    I2CxCONbits.I2CEN = 0;
}

void I2C1_ReceivedDataEvent(void)
{
    // @todo receive interrupt stuff
}

uint8_t I2C1_GetReceivedByte(void)
{
    return I2CxRCV;
}

bool I2C1_IsReceiveRegisterFull(void)
{
    if(I2CxSTATbits.RBF)
        return true; 
    else
        return false;
}

bool I2C1_IsReceiveUsingInterrupts(void)
{
    return useRxInterrupt;
}

void I2C1_ReceiveByte(void)
{
    /* RCEN bit is automatically cleared at the end of the 8th bit of a 
    received data byte */
    I2CxCONbits.RCEN = 1;
}

void I2C1_ReceiveByteCancel(void)
{
    I2CxCONbits.RCEN = 0;
    // @todo send stop if needed. Test - MS
}

void I2C1_TransmitFinishedEvent(void)
{
    // @todo transmit interrupt stuff
}

void I2C1_TransmitByte(uint8_t dataToSend)
{
    I2CxTRN = dataToSend;
}

bool I2C1_IsTransmitRegisterEmpty(void)
{
    // transmit buffer full status bit is cleared after 8 bits are shifted out
    if(I2CxSTATbits.TBF)
        return false;
    else
        return true;
}

bool I2C1_IsTransmitFinished(void)
{
    // transmit status is set after 9 bits. (8 data bits plus ack or nack)
    if(I2CxSTATbits.TRSTAT)
        return true;
    else
        return false;
}

bool I2C1_IsTransmitUsingInterrupts(void)
{
    return useTxInterrupt;
}

void I2C1_Start(void)
{
    I2CxCONbits.SEN = 1; // cleared by module when finished
}

void I2C1_Stop(void)
{
    I2CxCONbits.PEN = 1; // cleared by module when finished
}

void I2C1_Restart(void)
{
    I2CxCONbits.RSEN = 1; // cleared by module when finished
}

void I2C1_SendAck(bool ackOrNack)
{
    if(ackOrNack)
        I2CxCONbits.ACKDT = 0; // send ack
    else
        I2CxCONbits.ACKDT = 1; // send nack
    
    I2CxCONbits.ACKEN = 1; // cleared by module when finished
}

bool I2C1_IsBusy(void)
{
    /* I2CxCON bits:
     0, SEN:   0 = start condition idle
     1, RSEN:  0 = restart condition idle
     2, PEN:   0 = stop condition idle
     3, RCEN:  0 = receive sequence not in progress
     4, ACKEN: 0 = acknowledge sequence idle
     TRSTAT: 0 = master transmit not in progress  */
    if((I2CxCON & 0x001F) || I2C1STATbits.TRSTAT)
        return true;
    else
        return false;
}

I2CState I2C1_GetState(void)
{
    I2CState retState = I2C_STATE_UNKNOWN;
    uint8_t enableBits = I2CxCON & 0x001F;

    if(enableBits == 0x10) // ACKEN
        retState = I2C_STATE_MASTER_SENDING_ACK;
    else if(enableBits == 0x08) // RCEN
        retState = I2C_STATE_MASTER_RECEIVING;
    else if(enableBits == 0x04) // PEN
        retState = I2C_STATE_SENDING_STOP;
    else if(enableBits == 0x02) // RSEN
        retState = I2C_STATE_SENDING_REPEATED_START;
    else if(enableBits == 0x01) // SEN
        retState = I2C_STATE_SENDING_START;
    else if(enableBits == 0x00)
    {
        if(I2CxSTATbits.TRSTAT)
            retState = I2C_STATE_MASTER_TRANSMITTING;
        else
            retState = I2C_STATE_BUS_IDLE;
    }

    return retState;
}

bool I2C1_GetStartStatus(void)
{
    if(I2CxSTATbits.SEN)
        return true;
    else
        return false;
}

bool I2C1_GetStopStatus(void)
{
    if(I2CxSTATbits.PEN)
        return true;
    else
        return false;
}

bool I2C1_GetRestartStatus(void)
{
    if(I2CxSTATbits.RSEN)
        return true;
    else
        return false;
}

bool I2C1_GetAckSendStatus(void)
{
    if(I2CxSTATbits.ACKEN)
        return true;
    else
        return false;
}

bool I2C1_GetAckSlaveStatus(void)
{
    if(I2CxSTATbits.ACKSTAT)
        return false; // 1 = ack was not received (NACK)
    else
        return true;
}

bool I2C1_GetReceiveEnableStatus(void)
{
    if(I2CxSTATbits.RCEN)
        return true;
    else
        return false;
}

/*
 End of File
 */
