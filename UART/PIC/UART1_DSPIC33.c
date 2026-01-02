/***************************************************************************//**
 * @brief UART Library Implementation (DSPIC33)
 * 
 * @file UART1_PIC16.c
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 12/1/15   Original creation (PIC16)
 * @date 2/4/22    Added Doxygen
 * @date 3/9/22    Re-factored to use function table and interface
 * @date 6/25/22   Updated receive callback function
 * @date 7/31/22   Added checks and handler for recursive function calls
 * @date 5/9/25    Ported to DSPIC33
 * 
 * @details
 *      If you are using XC-8, it should have your chip selection defined in 
 * your project settings. Including xc.h should do the trick. If not, processor 
 * header files can be found in the install path of your compiler: 
 * "Install_Path/xc8/v2.20/pic/include/proc/"
 * Example: #include <proc/pic16lf18855.h>
 * 
 * // TODO add more details, 9-bit, and software flow control
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
/* @note This specific processor has a conflicting definition for "UART". Other 
PIC processors I've used always had some sort of suffix or something related to 
the register name. The "UART" typedef in the header file isn't really used 
either. Using this preprocessor trick will keep from having to rename every 
single thing in my library. - MS */
#define UART UART_DSPIC
#include <xc.h>
#undef UART

// ***** Defines ***************************************************************

/* This will change the percent error of your baud rate generator based on 
your crystal frequency. Typically, this are set to 1 to reach 115200 baud. 
If you are unsure, check the reference manual. */
#define UART_BRGH    1 // high speed brg

#if (UART_BRGH)
#define UART_BRG_DIV 4
#else
#define UART_BRG_DIV 16
#endif

/* Registers */
#define UxTXIE       _U1TXIE    // UART interrupt control TXIE and RXIE bits
#define UxRXIE       _U1RXIE
#define UxTXIF       _U1TXIF    // UART interrupt status TXIF and RXIF bits
#define UxRXIF       _U1RXIF
#define UxMODEbits   U1MODEbits // receive status and control register
#define UxSTAbits    U1STAbits  // transmit status and control register
#define UxBRG        U1BRG      // baud rate generator register
#define UxRXREG      U1RXREG    // receive data register
#define UxTXREG      U1TXREG    // transmit data register

// ***** Global Variables ******************************************************

/* Assign functions to the interface */
UARTInterface UART1_FunctionTable = {
    .UART_ComputeBRGValue = UART1_ComputeBRGValue,
    .UART_Init = UART1_Init,
    .UART_ReceivedDataEvent = UART1_ReceivedDataEvent,
    .UART_GetReceivedByte = UART1_GetReceivedByte,
    .UART_IsReceiveRegisterFull = UART1_IsReceiveRegisterFull,
    .UART_IsReceiveUsingInterrupts = UART1_IsReceiveUsingInterrupts,
    .UART_ReceiveEnable = UART1_ReceiveEnable,
    .UART_ReceiveDisable = UART1_ReceiveDisable,
    .UART_TransmitRegisterEmptyEvent = UART1_TransmitRegisterEmptyEvent,
    .UART_TransmitByte = UART1_TransmitByte,
    .UART_IsTransmitRegisterEmpty = UART1_IsTransmitRegisterEmpty,
    .UART_IsTransmitFinished = UART1_IsTransmitFinished,
    .UART_IsTransmitUsingInterrupts = UART1_IsTransmitUsingInterrupts,
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

    /* Convert pclk to a 26.5 fixed point number */
    pclkInHz <<= 5;
    /* Divide by (Prescale * Baud) */
    tmp = pclkInHz / (UART_BRG_DIV * desiredBaudRate);
    /* Subtract 1 (1 is converted to a 26.5 fixed point number) */
    tmp -= (1 << 5);
    /* Add 0.5 to round up (0.5 converted to 26.5 fixed point number is 2^4) */
    tmp += (1 << 4);
    /* Shift back right to get the final result */
    return (tmp >> 5);
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

    /* TODO Implement software flow control some day */

    // Disable interrupts before changing states
    UxRXIE = 0;
    UxTXIE = 0;

    UxSTAbits.ADDEN = use9Bit;
    if(use9Bit)
        UxMODEbits.PDSEL = 0x03;
    else if(parity == UART_ODD_PARITY)
        UxMODEbits.PDSEL = 0x02;
    else if(parity == UART_EVEN_PARITY)
        UxMODEbits.PDSEL = 0x01;
    else
        UxMODEbits.PDSEL = 0x00;

    if(stopBits == UART_TWO_P)
        UxMODEbits.STSEL = 1;
    else
        UxMODEbits.STSEL = 0;

    UxMODEbits.UARTEN = 1; // enable serial port

    UxMODEbits.BRGH = UART_BRGH; // high speed baud rate
    UxBRG = params->BRGValue;

    /* If you turn on the transmit interrupt during initialization, it could
    fire off repeatedly. It's best to turn it on after placing data in the 
    transmit register */

    if(useRxInterrupt)
        UxRXIE = 1; // rx register not empty interrupt

    UxSTAbits.UTXEN = 1; // enable transmitter
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
    uint8_t data = UxRXREG;

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
    if(UxRXIF)
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

bool UART1_IsReceiveUsingInterrupts(void)
{
    return useRxInterrupt;
}

// *****************************************************************************

void UART1_ReceiveEnable(void)
{
    /* This processor does not have a bit to turn off the receiver 
    independently from the transmitter. */

    if(useRxInterrupt)
        UxRXIE = 1;

    /* RTS is asserted (low) whenever we are ready to receive data. */
    if(flowControl == UART_FLOW_CALLBACKS && SetRTSPin != NULL)
    {
        SetRTSPin(false); // set low
    }
}

// *****************************************************************************

void UART1_ReceiveDisable(void)
{
    /* This processor does not have a bit to turn off the receiver 
    independently from the transmitter. */

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
    UxTXIE = 0;

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

    UxTXREG = data;

    /* Enable transmit interrupt here if needed */
    if(useTxInterrupt)
        UxTXIE = 1;
}

// *****************************************************************************

bool UART1_IsTransmitRegisterEmpty(void)
{
    bool txReady = false;

    /* UTXBF is 1 when the transmit buffer is full */
    if(UxSTAbits.UTXBF == 0)
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
    if(UxTXIF && UxSTAbits.TRMT)
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

bool UART1_IsTransmitUsingInterrupts(void)
{
    return useTxInterrupt;
}

// *****************************************************************************

void UART1_TransmitEnable(void)
{
    bool txDataFull = false;

    if(!UxTXIF)
        txDataFull = true;

    UxSTAbits.UTXEN = 1;

    /* If the transmit register is full and interrupts are desired, enable 
    them. The transmit interrupt flag also gets set whenever the transmitter
    is enabled on this processor, so I checked the flag first. */
    if(useTxInterrupt && txDataFull)
        UxTXIE = 1;
}

// *****************************************************************************

void UART1_TransmitDisable(void)
{
    while(!UxSTAbits.TRMT){} // wait for transmission to finish
    UxSTAbits.UTXEN = 0;
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
