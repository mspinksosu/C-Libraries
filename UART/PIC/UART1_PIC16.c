/***************************************************************************//**
 * @brief UART Library Implementation (PIC16)
 * 
 * @file UART1_PIC16.c
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 12/1/15   Original creation
 * @date 2/4/22    Added Doxygen
 * @date 3/9/22    Re-factored to use function table and interface
 * @date 6/25/22   Updated receive callback function
 * @date 7/31/22   Added checks and handler for recursive function calls
 * 
 * @details
 *      If you are using XC-8, it should have your chip selection defined in 
 * your project settings. Including xc.h should do the trick. If not, processor 
 * header files can be found in the install path of your compiler: 
 * "Install_Path/xc8/v2.20/pic/include/proc/"
 * Example: #include <proc/pic16lf18855.h>
 * 
 * This implementation was created for a PIC16F1885X. Your PIC may differ a
 * little bit. I have defines down below for the registers that are used. All
 * you need to do is find the register names for your PIC and alter them. You
 * also need to check if the baud rate generator divisor settings are 
 * different.
 * 
 *  // TODO add more details, 9-bit, and software flow control
 * 
 * @section example_code Example Code
 *      UART myUART;
 *      UART_Create(&myUART, &UART1_FunctionTable);
 *      UART_SetToDefaultParams(&myUART);
 *      uint32_t baud = UART_ComputeBRGValue(&myUART, 115200, 12000000UL);
 *      UART_SetBRGValue(&myUART, baud);
 *      UART_Init(&myUART);
 * 
 * @section license License
 * SPDX-FileCopyrightText: © 2015 Matthew Spinks
 * SPDX-License-Identifier: Zlib
 * 
 * This software is released under the Zlib license. You are free alter and
 * redistribute it, but you must not misrepresent the origin of the software.
 * This notice may not be removed. <http://www.zlib.net/zlib_license.html>
 * 
 * ****************************************************************************/

#include "UART1.h"
#include <stddef.h>

/* Include processor specific header files here */
#include <xc.h>

// ***** Defines ***************************************************************

/* These will change the percent error of your baud rate generator based on 
your crystal frequency. Typically, these are both set to 1 to reach 115200 
baud. If you are unsure, check the reference manual. */
#define UART_BRG16   1 // use 16-bit baud rate generator
#define UART_BRGH    1 // high speed brg

#if (UART_BRG16 && UART_BRGH)
#define UART_BRG_DIV 4
#elif (!UART_BRG16 && !UART_BRGH)
#define UART_BRG_DIV 64
#else
#define UART_BRG_DIV 16
#endif

/* Registers */
#define PIExbits     PIE3bits     // UART interrupt control TXIE and RXIE bits
#define PIRxbits     PIR3bits     // UART interrupt status TXIF and RCIF bits
#define RCxSTAbits   RC1STAbits   // receive status and control register
#define TXxSTAbits   TX1STAbits   // transmit status and control register
#define BAUDxCONbits BAUD1CONbits // baud rate control register
#define SPxBRGH      SP1BRGH      // baud rate generator high register
#define SPxBRGL      SP1BRGL      // baud rate generator low register
#define RCxREG       RC1REG       // receive data register
#define TXxREG       TX1REG       // transmit data register

// ***** Global Variables ******************************************************

/* Assign functions to the interface */
UARTInterface UART1_FunctionTable = {
    .UART_ComputeBRGValue = UART1_ComputeBRGValue,
    .UART_Init = UART1_Init,
    .UART_ReceivedDataEvent = UART1_ReceivedDataEvent,
    .UART_GetReceivedByte = UART1_GetReceivedByte,
    .UART_IsReceiveRegisterFull = UART1_IsReceiveRegisterFull,
    .UART_ReceiveEnable = UART1_ReceiveEnable,
    .UART_ReceiveDisable = UART1_ReceiveDisable,
    .UART_TransmitRegisterEmptyEvent = UART1_TransmitRegisterEmptyEvent,
    .UART_TransmitByte = UART1_TransmitByte,
    .UART_IsTransmitRegisterEmpty = UART1_IsTransmitRegisterEmpty,
    .UART_IsTransmitFinished = UART1_IsTransmitFinished,
    .UART_TransmitEnable = UART1_TransmitEnable,
    .UART_TransmitDisable = UART1_TransmitDisable,
    .UART_PendingEventHandler = UART1_PendingEventHandler,
    .UART_SetTransmitRegisterEmptyCallback = UART1_SetTransmitRegisterEmptyCallback,
    .UART_SetReceivedDataCallback = UART1_SetReceivedDataCallback,
    .UART_SetIsCTSPinLowFunc = UART1_SetIsCTSPinLowFunc,
    .UART_SetRTSPinFunc = UART1_SetRTSPinFunc,
};

static bool use9Bit = false, useRxInterrupt = false, useTxInterrupt = false;
static UARTFlowControl flowControl = UART_FLOW_NONE;
static UARTStopBits stopBits = UART_ONE_P;
static UARTParity parity = UART_NO_PARITY;
static bool lockTxFinishedEvent = false, txFinishedEventPending = false,
    lockRxReceivedEvent = false;

// local function pointers
static void (*TransmitRegisterEmptyCallback)(void);
static void (*ReceivedDataCallback)(uint8_t (*CallToGetData)(void));
static bool (*IsCTSPinLow)(void);
static void (*SetRTSPin)(bool setHigh);

// ***** Static Function Prototypes ********************************************

/* Put static function prototypes here */


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

uint32_t UART1_ComputeBRGValue(uint32_t desiredBaudRate, uint32_t pclkInHz)
{
    /* BRG = Clk / (Prescale * Baud) - 1 */
    uint32_t tmp = 0;

    /* Convert pclk to a 26.6 fixed point number */
    pclkInHz <<= 6;
    /* Divide by (Prescale * Baud) */
    tmp = pclkInHz / (UART_BRG_DIV * desiredBaudRate);
    /* Subtract 1 (1 is converted to a 26.6 fixed point number) */
    tmp -= (1 << 6);
    /* Add 0.5 to round up (0.5 converted to 26.6 fixed point number is 2^5) */
    tmp += (1 << 5);
    /* Shift back right to get the final result */
    return (tmp >> 6);
}

// *****************************************************************************

void UART1_Init(UARTInitType *params)
{
    if(params->BRGValue == 0)
        return;

    use9Bit = params->use9Bit;
    flowControl = params->flowControl;
    stopBits = params->stopBits;
    parity = params->parity;
    useRxInterrupt = params->useRxInterrupt;
    useTxInterrupt = params->useTxInterrupt;

    /* There is no hardware flow control, multiple stop bits, or automatic 
    parity bits on this processor */
    /* TODO Implement software flow control some day */

    // Disable interrupts before changing states
    PIExbits.RCIE = 0;
    PIExbits.TXIE = 0;

    RCxSTAbits.RX9 = use9Bit;
    RCxSTAbits.ADDEN = use9Bit;

    RCxSTAbits.SPEN = 1; // enable serial port
    TXxSTAbits.SYNC = 0; // asynchronous
    TXxSTAbits.TX9 = use9Bit;

    TXxSTAbits.BRGH = UART_BRGH;     // high speed baud rate
    BAUDxCONbits.BRG16 = UART_BRG16; // 16-bit baud generator

    SPxBRGL = (uint8_t)params->BRGValue;
    SPxBRGH = (uint8_t)(params->BRGValue >> 8);

    /* If you turn on the transmit interrupt during initialization, it could
    fire off repeatedly. It's best to turn it on after placing data in the 
    transmit register */

    if(useRxInterrupt)
        PIExbits.RCIE = 1; // rx register not empty interrupt
    
    RCxSTAbits.CREN = 1; // enable receiver (continuous receive)
    TXxSTAbits.TXEN = 1; // enable transmitter
}

// *****************************************************************************

void UART1_ReceivedDataEvent(void)
{
    if(lockRxReceivedEvent == true)
    {
        /* On a PIC, global interrupts are disabled automatically before
        entering the interrupt. However, it is possible for the user to 
        re-enable interrupts again before leaving. Prevent the possibility 
        of another interrupt from calling us while we're in a callback */
        return;
    }
    lockRxReceivedEvent = true;

    /* RTS is asserted (low) whenever we are ready to receive data. It is 
    deasserted (high) when the receive register is full. */
    if(flowControl == UART_FLOW_CALLBACKS && SetRTSPin != NULL)
    {
        SetRTSPin(true); // set high
    }

    if(ReceivedDataCallback)
    {
        ReceivedDataCallback(UART1_GetReceivedByte);
    }
    lockRxReceivedEvent = false;
}

// *****************************************************************************

uint8_t UART1_GetReceivedByte(void)
{
    uint8_t data = RCxREG;

    /* RTS is asserted (low) whenever we are ready to receive data. It is 
    deasserted (high) when the receive register is full */
    if(flowControl == UART_FLOW_CALLBACKS && SetRTSPin != NULL)
    {
        SetRTSPin(false); // set low
    }

    return data;
}

// *****************************************************************************

bool UART1_IsReceiveRegisterFull(void)
{
    bool rxFull = false;

    /* The receive character interrupt flag is set whenever there is an unread
    character and is cleared by reading the character */
    if(PIRxbits.RCIF)
        rxFull = true;

    /* If the user chooses to poll this function instead of using the receive 
    data event, we must still do something with the RTS pin. */
    if(flowControl == UART_FLOW_CALLBACKS && SetRTSPin != NULL)
    {
        SetRTSPin(rxFull); // "deassert" (high) when full
    }

    return rxFull;
}

// *****************************************************************************

void UART1_ReceiveEnable(void)
{
    RCxSTAbits.CREN = 1;

    if(useRxInterrupt)
        PIExbits.RCIE = 1;

    /* RTS is asserted (low) whenever we are ready to receive data. */
    if(flowControl == UART_FLOW_CALLBACKS && SetRTSPin != NULL)
    {
        SetRTSPin(false); // set low
    }
}

// *****************************************************************************

void UART1_ReceiveDisable(void)
{
    RCxSTAbits.CREN = 0;

    /* RTS is deasserted (high) whenever we are not ready to receive data. */
    if(flowControl == UART_FLOW_CALLBACKS && SetRTSPin != NULL)
    {
        SetRTSPin(true); // set high
    }
}

// *****************************************************************************

void UART1_TransmitRegisterEmptyEvent(void)
{
    /* This will prevent recursive calls if we call transmit byte function from
    within the transmit interrupt callback. This requires the pending event
    handler function to be called to catch the txFinishedEventPending flag. */
    if(lockTxFinishedEvent == true)
    {
        txFinishedEventPending = true;
        return;
    }
    lockTxFinishedEvent = true;

    /* Disable transmit interrupt here */
    PIExbits.TXIE = 0;

    if(TransmitRegisterEmptyCallback)
    {
        TransmitRegisterEmptyCallback();
    }
    lockTxFinishedEvent = false;
}

// *****************************************************************************

void UART1_TransmitByte(uint8_t data)
{
    /* Check if CTS is asserted (low) before transmitting. If so, send data */
    if(flowControl == UART_FLOW_CALLBACKS && IsCTSPinLow != NULL &&
        IsCTSPinLow() == false)
    {
        return; // CTS was high
    }
    
    /* Clear the transmission complete flag if needed */

    TXxREG = data;

    /* Enable transmit interrupt here if needed */
    if(useTxInterrupt)
        PIExbits.TXIE = 1;
}

// *****************************************************************************

bool UART1_IsTransmitRegisterEmpty(void)
{
    bool txReady = false;

    /* The transmit interrupt flag is set whenever the transmitter is enabled
    and there is no character in the register for transmission. It is cleared 
    when the transmit register is written to */
    if(PIRxbits.TXIF)
        txReady = true;

    /* If the user chooses to poll this function instead of using the transmit
    register empty event, we want to try and prevent transmission if CTS is 
    asserted */
    if(flowControl == UART_FLOW_CALLBACKS && IsCTSPinLow != NULL &&
        IsCTSPinLow() == false)
    {
        txReady = false; // CTS was high. Don't allow transmission
    }

    return txReady;
}

// *****************************************************************************

bool UART1_IsTransmitFinished(void)
{
    bool txReady = false;

    /* The transmit register is empty and the shift register is empty.
    The TRMT bit is read only */
    if(PIRxbits.TXIF && TXxSTAbits.TRMT)
        txReady = true;

    /* This function will behave the same as the transmit register empty 
    function. If the user chooses to poll this function, we want to make sure 
    we block input to the transmit register when CTS is asserted */
    if(flowControl == UART_FLOW_CALLBACKS && IsCTSPinLow != NULL &&
        IsCTSPinLow() == false)
    {
        txReady = false; // CTS was high. Don't allow transmission
    }

    return txReady;
}

// *****************************************************************************

void UART1_TransmitEnable(void)
{
    bool txDataFull = false;

    if(!PIRxbits.TXIF)
        txDataFull = true;

    TXxSTAbits.TXEN = 1;

    /* If the transmit register is full and interrupts are desired, enable 
    them. The transmit interrupt flag also gets set whenever the transmitter
    is enabled on this processor, so I checked the flag first. */
    if(useTxInterrupt && txDataFull)
        PIExbits.TXIE = 1;
}

// *****************************************************************************

void UART1_TransmitDisable(void)
{
    while(!TXxSTAbits.TRMT){} // wait for transmission to finish
    TXxSTAbits.TXEN = 0;
}

// *****************************************************************************

void UART1_PendingEventHandler(void)
{
    if(txFinishedEventPending && !lockTxFinishedEvent)
    {
        txFinishedEventPending = false;
        UART1_TransmitRegisterEmptyEvent();
    }
}

// *****************************************************************************

void UART1_SetTransmitRegisterEmptyCallback(void (*Function)(void))
{
    TransmitRegisterEmptyCallback = Function;
}

// *****************************************************************************

void UART1_SetReceivedDataCallback(void (*Function)(uint8_t (*CallToGetData)(void)))
{
    ReceivedDataCallback = Function;
}

// *****************************************************************************

void UART1_SetIsCTSPinLowFunc(bool (*Function)(void))
{
    IsCTSPinLow = Function;
}

// *****************************************************************************

void UART1_SetRTSPinFunc(void (*Function)(bool setPinHigh))
{
    SetRTSPin = Function;
}

/*
 End of File
 */