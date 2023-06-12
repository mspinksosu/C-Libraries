/***************************************************************************//**
 * @brief UART Library Implementation (STM32G0)
 * 
 * @file UART1_STM32G0.c
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 3/12/22   Original creation
 * @date 6/13/22   Changed compute baud rate function
 * @date 6/25/22   Updated receive callback function
 * @date 7/31/22   Added checks and handler for recursive function calls
 * 
 * @details
 *      // TODO Add details, 9-bit, parity, software flow control
 * 
 * Example Code:
 *      UART myUART;
 *      UART_Create(&myUART, &UART1_FunctionTable);
 *      UART_SetToDefaultParams(&myUART);
 *      uint32_t baud = UART_ComputeBRGValue(&myUART, 115200, 12000000UL);
 *      UART_SetBRGValue(&myUART, baud);
 *      UART_Init(&myUART);
 * 
 * @section license License
 * SPDX-FileCopyrightText: © 2022 Matthew Spinks
 * SPDX-License-Identifier: Zlib
 * 
 * This software is released under the Zlib license. You are free alter and
 * redistribute it, but you must not misrepresent the origin of the software.
 * This notice may not be removed. <http://www.zlib.net/zlib_license.html>
 * 
 * ****************************************************************************/

#include "UART1.h"
#include <stddef.h> // needed for NULL

/* Include processor specific header files here */
#include "stm32g071xx.h"

// ***** Defines ***************************************************************

enum {
    UART1_PRE_NO_DIV = 0,
    UART1_PRE_DIV_2,
    UART1_PRE_DIV_4,
    UART1_PRE_DIV_6,
    UART1_PRE_DIV_8,
    UART1_PRE_DIV_10,
    UART1_PRE_DIV_12,
    UART1_PRE_DIV_16,
    UART1_PRE_DIV_32,
    UART1_PRE_DIV_64,
    UART1_PRE_DIV_128,
    UART1_PRE_DIV_256};

// ----- User selectable values ------------------------------------------------
#define UART_PRESCALE UART1_PRE_NO_DIV // choose from the enum
#define OVER8         0                // 0 = oversample 16, 1 = oversample 8
// -----------------------------------------------------------------------------

/* Peripheral addresses and registers */
#define UART_ADDR       USART1
#define UART_CLK_REG    RCC->APBENR2
#define UART_CLK_EN_MSK RCC_APBENR2_USART1EN

static const uint8_t preLUT[16] = {0,2,4,6,8,10,12,16,32,64,128,256};

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
    if(desiredBaudRate == 0)
        return 0;

    uint32_t uartDiv;

    if(OVER8) 
    {
        uartDiv = (uint32_t)(2 * pclkInHz / preLUT[UART_PRESCALE]) / desiredBaudRate;
        // BRR[2:0] = lower 4 bits shifted one to the right. Ref man page 1018
        uint32_t BRRLowerByte = (uartDiv & 0x000F) >> 1;
        uartDiv &= 0xFFF0;
        uartDiv |= BRRLowerByte;
    }
    else
    {
        uartDiv = (uint32_t)(pclkInHz / preLUT[UART_PRESCALE]) / desiredBaudRate;
    }
    return uartDiv;
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

    /* Peripheral clock must be enabled before you can write any registers */
    UART_CLK_REG |= UART_CLK_EN_MSK;

    /* Turn off module before making changes */
    UART_ADDR->CR1 &= ~USART_CR1_UE;

    /* Turn off tx/rx interrupts and other bits that I'm going to adjust */
    UART_ADDR->CR1 &= ~(USART_CR1_RXNEIE_RXFNEIE | USART_CR1_TXEIE_TXFNFIE | USART_CR1_M | USART_CR1_M0 | USART_CR1_PCE);

    /* Set number of data bits, stop bits, and parity */
    if(use9Bit)
        UART_ADDR->CR1 |= USART_CR1_M0;

    switch(stopBits)
    {
        case UART_HALF_P:
            UART_ADDR->CR2 |= USART_CR2_STOP_0; // [1:0] = 01
            UART_ADDR->CR2 &= ~(USART_CR2_STOP_1);
            break;
        case UART_ONE_PLUS_HALF_P:
            UART_ADDR->CR2 |= (USART_CR2_STOP_1 | USART_CR2_STOP_0);
            break;
        case UART_TWO_P:
            UART_ADDR->CR2 &= ~(USART_CR2_STOP_0); // [1:0] = 10
            UART_ADDR->CR2 |= USART_CR2_STOP_1;
            break;
        default:
            UART_ADDR->CR2 &= ~(USART_CR2_STOP_1 | USART_CR2_STOP_0);
            break;
    }

    if(parity == UART_EVEN_PARITY)
    {
        UART_ADDR->CR1 &= ~USART_CR1_PS;
        UART_ADDR->CR1 |= USART_CR1_PCE;
    }
    else if(parity == UART_ODD_PARITY)
    {
        UART_ADDR->CR1 |= USART_CR1_PS;
        UART_ADDR->CR1 |= USART_CR1_PCE;
    }

    /* TODO Implement software flow control some day */
    if(flowControl == UART_FLOW_HARDWARE)
    {
        UART_ADDR->CR3 |= (USART_CR3_CTSE | USART_CR3_RTSE);
    }
    else
    {
        UART_ADDR->CR3 &= ~(USART_CR3_CTSE | USART_CR3_RTSE);
    }

    /* Set prescale and baud rate. Use the defines at the top */
    UART_ADDR->PRESC &= ~USART_PRESC_PRESCALER;
    UART_ADDR->PRESC |= UART_PRESCALE;

    UART_ADDR->CR1 &= ~USART_CR1_OVER8;
    if(OVER8) UART_ADDR->CR1 |= USART_CR1_OVER8;

    UART_ADDR->BRR = (uint16_t)(params->BRGValue);

    /* If you turn on the transmit interrupt during initialization, it could
    fire off repeatedly. It's best to turn it on after placing data in the 
    transmit register */

    if(useRxInterrupt)
        UART_ADDR->CR1 |= USART_CR1_RXNEIE_RXFNEIE; // rx register not empty interrupt

    UART_ADDR->CR1 |= USART_CR1_RE; // enable receiver
    UART_ADDR->CR1 |= USART_CR1_TE; // enable transmitter
    UART_ADDR->CR1 |= USART_CR1_UE; // enable UART 
}

// *****************************************************************************

void UART1_ReceivedDataEvent(void)
{
    if(lockRxReceivedEvent == true)
    {
        /* Prevent the possibility of another interrupt from somehow calling us 
        while we're in a callback */
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
    uint8_t data = UART_ADDR->TDR;

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

    /* The RX register not empty flag is set when the receive data register has 
    a character placed in it. It is cleared by reading the character from the
    receive data register. */
    if(UART_ADDR->ISR & USART_ISR_RXNE_RXFNE)
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
    UART_ADDR->CR1 |= USART_CR1_RE;

    if(useRxInterrupt) 
        UART_ADDR->CR1 |= USART_CR1_RXNEIE_RXFNEIE;

    /* RTS is asserted (low) whenever we are ready to receive data. */
    if(flowControl == UART_FLOW_CALLBACKS && SetRTSPin != NULL)
    {
        SetRTSPin(false); // set low
    }
}

// *****************************************************************************

void UART1_ReceiveDisable(void)
{
    UART_ADDR->CR1 &= ~USART_CR1_RE;

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
    UART_ADDR->CR1 &= ~USART_CR1_TXEIE_TXFNFIE;

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
    
    /* Clear the transmission complete flag if implemented */
    UART_ADDR->ICR |= USART_ICR_TCCF;

    UART_ADDR->TDR = data;

    /* Enable transmit interrupt here if needed */
    if(useTxInterrupt)
        UART_ADDR->CR1 |= USART_CR1_TXEIE_TXFNFIE;
}

// *****************************************************************************

bool UART1_IsTransmitRegisterEmpty(void)
{
    bool txReady = false;

    /* The transmit register empty flag is set when the contents of the
    transmit data register are emptied. It is cleared when the transmit data
    register is written to */
    if(UART_ADDR->ISR & USART_ISR_TXE_TXFNF)
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

    /* The transmit complete flag is set when a full data byte is shifted out 
    and the transmit register empty flag is set. It is cleared by writing a 1
    to TCCF in USART_ICR or a write to the transmit data register. */
    if(UART_ADDR->ISR & USART_ISR_TC)
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
    UART_ADDR->CR1 |= USART_CR1_TE;

    /* If the transmit register is full and interrupts are desired, 
    enable them */
    if(useTxInterrupt && !(UART_ADDR->ISR & USART_ISR_TXE_TXFNF))
        UART_ADDR->CR1 |= USART_CR1_TXEIE_TXFNFIE;
}

// *****************************************************************************

void UART1_TransmitDisable(void)
{
    while(!(UART_ADDR->ISR & USART_ISR_TC)){} // wait for transmission to finish
    UART_ADDR->CR1 &= ~USART_CR1_TE;
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