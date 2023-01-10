/***************************************************************************//**
 * @brief UART Library Implementation (STM32F1)
 * 
 * @author Matthew Spinks
 * 
 * @date 3/12/22    Original creation (G0 implementation)
 * @date 6/13/22    Ported settings for F1
 * @date 6/25/22    Updated receive callback function
 * @date 7/31/22    Added checks and handler for recursive function calls
 * 
 * @file UART2_STM32F1.c
 * 
 * @details
 *      I am currently using register names as given by the header file
 * included with STM32 F10x standard peripheral library v2.0.3
 * 
 * Example Code:
 *      UART myUART;
 *      UART_Create(&myUART, &UART1_FunctionTable);
 *      UART_SetToDefaultParams(&myUART);
 *      uint32_t baud = UART_ComputeBRGValue(&myUART, 115200, 12000000UL);
 *      UART_SetBRGValue(&myUART, baud);
 *      UART_Init(&myUART);
 *      
 * ****************************************************************************/

#include "UART2.h"
#include <math.h>
#include <stddef.h>

/* Include processor specific header files here */
#include "stm32f10x_map.h"

// ***** Defines ***************************************************************

/* Peripheral addresses and registers */
#define UART2_ADDR          USART2
#define UART2_CLK_REG       RCC->APB1ENR
#define UART2_CLK_EN_MSK    RCC_APB1ENR_USART2EN

// ***** Global Variables ******************************************************

/* Assign functions to the interface */
UARTInterface UART2_FunctionTable = {
    .UART_ComputeBRGValue = UART2_ComputeBRGValue,
    .UART_Init = UART2_Init,
    .UART_ReceivedDataEvent = UART2_ReceivedDataEvent,
    .UART_GetReceivedByte = UART2_GetReceivedByte,
    .UART_IsReceiveRegisterFull = UART2_IsReceiveRegisterFull,
    .UART_ReceiveEnable = UART2_ReceiveEnable,
    .UART_ReceiveDisable = UART2_ReceiveDisable,
    .UART_TransmitRegisterEmptyEvent = UART2_TransmitRegisterEmptyEvent,
    .UART_TransmitByte = UART2_TransmitByte,
    .UART_IsTransmitRegisterEmpty = UART2_IsTransmitRegisterEmpty,
    .UART_IsTransmitFinished = UART2_IsTransmitFinished,
    .UART_TransmitEnable = UART2_TransmitEnable,
    .UART_TransmitDisable = UART2_TransmitDisable,
    .UART_PendingEventHandler = UART2_PendingEventHandler,
    .UART_SetTransmitRegisterEmptyCallback = UART2_SetTransmitRegisterEmptyCallback,
    .UART_SetReceivedDataCallback = UART2_SetReceivedDataCallback,
    .UART_SetIsCTSPinLowFunc = UART2_SetIsCTSPinLowFunc,
    .UART_SetRTSPinFunc = UART2_SetRTSPinFunc,
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

uint32_t UART2_ComputeBRGValue(uint32_t desiredBaudRate, uint32_t pclkInHz)
{
    if(desiredBaudRate == 0)
        return 0;

    /* USART1 clock comes from PCLK2, all other USART's use PCLK1.
    Baud rate equations: Reference manual section 27.3.4. Page 798 */
    uint32_t uartDiv, divFraction;
    float uartDivFloat, divFractionFloat;
    double uartDivMantissa;
    uint8_t carry = 0;

    uartDivFloat = pclkInHz / (desiredBaudRate * 16.0f);

    /* For some reason the reference manual likes to call the integer part 
    of uartDivFloat the mantissa. The modf function will set uartDivFloat to 
    the integer part, and divFraction to the fractional part. */
    divFractionFloat = modf(uartDivFloat, &uartDivMantissa);
    
    uartDiv = (uint32_t)uartDivMantissa;
    divFraction = (uint32_t)roundf(divFractionFloat * 16.0f);

    /* The fractional part is 4-bits. If the rounding goes over 16, add it to
    the "mantissa part" (uartDiv). Example 3. Page 799 */
    if(divFraction > 0x000F)
        carry = 1;
    divFraction &= 0x0F;
    uartDiv += carry;
    uartDiv <<= 4;
    uartDiv |= divFraction;

    return uartDiv;
}

// *****************************************************************************

void UART2_Init(UARTInitType *params)
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
    UART2_CLK_REG |= UART2_CLK_EN_MSK;

    /* Turn off module before making changes */
    UART2_ADDR->CR1 &= ~USART_CR1_UE;

    /* Turn off tx/rx interrupts and other bits that I'm going to adjust */
    UART2_ADDR->CR1 &= ~(USART_CR1_RXNEIE | USART_CR1_TXEIE | USART_CR1_M | USART_CR1_PCE);

    /* Set number of data bits, stop bits, and parity */
    if(use9Bit)
        UART2_ADDR->CR1 |= USART_CR1_M;

    switch(stopBits)
    {
        case UART_HALF_P:
            UART2_ADDR->CR2 |= USART_CR2_STOP_0; // [1:0] = 01
            UART2_ADDR->CR2 &= ~(USART_CR2_STOP_1);
            break;
        case UART_ONE_PLUS_HALF_P:
            UART2_ADDR->CR2 |= (USART_CR2_STOP_1 | USART_CR2_STOP_0);
            break;
        case UART_TWO_P:
            UART2_ADDR->CR2 &= ~(USART_CR2_STOP_0); // [1:0] = 10
            UART2_ADDR->CR2 |= USART_CR2_STOP_1;
            break;
        default:
            UART2_ADDR->CR2 &= ~(USART_CR2_STOP_1 | USART_CR2_STOP_0);
            break;
    }

    if(parity == UART_EVEN_PARITY)
    {
        UART2_ADDR->CR1 &= ~USART_CR1_PS;
        UART2_ADDR->CR1 |= USART_CR1_PCE;
    }
    else if(parity == UART_ODD_PARITY)
    {
        UART2_ADDR->CR1 |= USART_CR1_PS;
        UART2_ADDR->CR1 |= USART_CR1_PCE;
    }

    /* TODO Implement software flow control some day */
    if(flowControl == UART_FLOW_HARDWARE)
    {
        UART2_ADDR->CR3 |= (USART_CR3_CTSE | USART_CR3_RTSE);
    }
    else
    {
        UART2_ADDR->CR3 &= ~(USART_CR3_CTSE | USART_CR3_RTSE);
    }

    /* Set prescale and baud rate. For this processor, prescale is reserved
    for low power (IrDa) use only*/
    UART2_ADDR->BRR = (0x0000FFFF & params->BRGValue);

    /* If you turn on the transmit interrupt during initialization, it could
    fire off repeatedly. It's best to turn it on after placing data in the 
    transmit register */

    if(useRxInterrupt) 
        UART2_ADDR->CR1 |= USART_CR1_RXNEIE; // rx register not empty interrupt

    UART2_ADDR->CR1 |= USART_CR1_RE; // enable receiver
    UART2_ADDR->CR1 |= USART_CR1_TE; // enable transmitter
    UART2_ADDR->CR1 |= USART_CR1_UE; // enable UART 
}

// *****************************************************************************

void UART2_ReceivedDataEvent(void)
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
        ReceivedDataCallback(UART2_GetReceivedByte);
    }
    lockRxReceivedEvent = false;
}

// *****************************************************************************

uint8_t UART2_GetReceivedByte(void)
{
    uint8_t data = UART2_ADDR->DR;

    /* RTS is asserted (low) whenever we are ready to receive data. It is 
    deasserted (high) when the receive register is full */
    if(flowControl == UART_FLOW_CALLBACKS && SetRTSPin != NULL)
    {
        SetRTSPin(false); // set low
    }

    return data;
}

// *****************************************************************************

bool UART2_IsReceiveRegisterFull(void)
{
    bool rxFull = false;

    /* The RX register not empty flag is set when the RDR has a character 
    placed in it. It is cleared by reading the character from RDR */
    if(UART2_ADDR->SR & USART_SR_RXNE)
        rxFull = true;

    /* If the user chooses to poll this function instead of using the receive 
    data event, we must still do something with the RTS pin. */
    if(flowControl == UART_FLOW_CALLBACKS && SetRTSPin != NULL)
    {
        if(UART2_ADDR->SR & USART_SR_RXNE)
            SetRTSPin(true); // "deassert" (high) when full
        else
            SetRTSPin(false);
    }

    return rxFull;
}

// *****************************************************************************

void UART2_ReceiveEnable(void)
{
    UART2_ADDR->CR1 |= USART_CR1_RE;

    /* RTS is asserted (low) whenever we are ready to receive data. */
    if(flowControl == UART_FLOW_CALLBACKS && SetRTSPin != NULL)
    {
        SetRTSPin(false); // set low
    }
}

// *****************************************************************************

void UART2_ReceiveDisable(void)
{
    UART2_ADDR->CR1 &= ~USART_CR1_RE;

    /* RTS is deasserted (high) whenever we are not ready to receive data. */
    if(flowControl == UART_FLOW_CALLBACKS && SetRTSPin != NULL)
    {
        SetRTSPin(true); // set high
    }
}

// *****************************************************************************

void UART2_TransmitRegisterEmptyEvent(void)
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
    UART2_ADDR->CR1 &= ~USART_CR1_TXEIE;

    if(TransmitRegisterEmptyCallback)
    {
        TransmitRegisterEmptyCallback();
    }
    lockTxFinishedEvent = false;
}

// *****************************************************************************

void UART2_TransmitByte(uint8_t data)
{
    /* Check if CTS is asserted (low) before transmitting. If so, send data */
    if(flowControl == UART_FLOW_CALLBACKS && IsCTSPinLow != NULL &&
        IsCTSPinLow() == false)
    {
        return; // CTS was high
    }
    
    /* Clear the transmission complete flag if implemented */
    UART2_ADDR->SR &= ~USART_SR_TC;

    UART2_ADDR->DR = data;

    /* Enable transmit interrupt here if needed */
    if(useTxInterrupt)
        UART2_ADDR->CR1 |= USART_CR1_TXEIE;
}

// *****************************************************************************

bool UART2_IsTransmitRegisterEmpty(void)
{
    bool txReady = false;

    /* The transmit register empty flag is set when the contents of the TDR 
    register are emptied. It is cleared when the TDR register is written to */
    if(UART2_ADDR->SR & USART_SR_TXE)
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

bool UART2_IsTransmitFinished(void)
{
    bool txReady = false;

    /* The transmit complete flag is set when a full data byte is shifted out 
    and the transmit register empty flag is set. It is cleared by writing a
    zero to it. */
    if(UART2_ADDR->SR & USART_SR_TC)
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

void UART2_TransmitEnable(void)
{
    UART2_ADDR->CR1 |= USART_CR1_TE;
}

// *****************************************************************************

void UART2_TransmitDisable(void)
{
    while(!(UART2_ADDR->SR & USART_SR_TC)){} // wait for transmission to finish
    UART2_ADDR->CR1 &= ~USART_CR1_TE;
}

// *****************************************************************************

void UART2_PendingEventHandler(void)
{
    if(txFinishedEventPending && !lockTxFinishedEvent)
    {
        txFinishedEventPending = false;
        UART2_TransmitRegisterEmptyEvent();
    }
}

// *****************************************************************************

void UART2_SetTransmitRegisterEmptyCallback(void (*Function)(void))
{
    TransmitRegisterEmptyCallback = Function;
}

// *****************************************************************************

void UART2_SetReceivedDataCallback(void (*Function)(uint8_t (*CallToGetData)(void)))
{
    ReceivedDataCallback = Function;
}

// *****************************************************************************

void UART2_SetIsCTSPinLowFunc(bool (*Function)(void))
{
    IsCTSPinLow = Function;
}

// *****************************************************************************

void UART2_SetRTSPinFunc(void (*Function)(bool setPinHigh))
{
    SetRTSPin = Function;
}

/*
 End of File
 */