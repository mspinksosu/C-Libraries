/***************************************************************************//**
 * @brief UART Library Implementation (STM32F1)
 * 
 * @author Matthew Spinks
 * 
 * @date 3/12/22    Original creation (G0 implementation)
 * @date 6/13/22    Ported settings for F1
 * @date 6/19/22    9-bit, parity, and Tx interrupt options
 * @date 6/25/22    Updated receive callback function
 * @date 7/31/22    Added checks and handler for recursive function calls
 * 
 * @file UART1_STM32F1.c
 * 
 * @details
 *      I am currently using register names as given by the header file
 * included with STM32 F10x standard peripheral library v2.0.3
 * 
 *      TODO Add details, 9-bit, and different parity, hardware flow control
 * 
 * Example Code:
 *      UART myUART;
 *      UART_Create(&myUART, &UART1_FunctionTable);
 *      UART_SetToDefaultParams(&myUART);
 *      uint32_t baud = UART_ComputeBRGValue(&myUART, 115200, 12.0);
 *      UART_SetBRGValue(&myUART, baud);
 *      UART_Init(&myUART);
 *      
 * ****************************************************************************/

#include <stddef.h>
#include <math.h>
#include "UART1.h"

/* Include processor specific header files here */
#include "stm32f10x_map.h"

// ***** Defines ***************************************************************

/* Peripheral addresses and registers */
#define UART1_ADDR          USART1
#define UART1_CLK_REG       RCC->APB2ENR
#define UART1_CLK_EN_MSK    RCC_APB2ENR_USART1EN

// ***** Global Variables ******************************************************

/* Assign functions to the interface */
UARTInterface UART1_FunctionTable = {
    .UART_ComputeBRGValue = UART1_ComputeBRGValue,
    .UART_Init = (void (*)(void *))UART1_Init,
    .UART_ReceivedDataEvent = UART1_ReceivedDataEvent,
    .UART_GetReceivedByte = UART1_GetReceivedByte,
    .UART_IsReceiveRegisterFull = UART1_IsReceiveRegisterFull,
    .UART_ReceiveEnable = UART1_ReceiveEnable,
    .UART_ReceiveDisable = UART1_ReceiveDisable,
    .UART_TransmitFinishedEvent = UART1_TransmitFinishedEvent,
    .UART_TransmitByte = UART1_TransmitByte,
    .UART_IsTransmitRegisterEmpty = UART1_IsTransmitRegisterEmpty,
    .UART_TransmitEnable = UART1_TransmitEnable,
    .UART_TransmitDisable = UART1_TransmitDisable,
    .UART_PendingEventHandler = UART1_PendingEventHandler,
    .UART_SetTransmitFinishedCallback = UART1_SetTransmitFinishedCallback,
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
static void (*TransmitFinishedCallback)(void);
static void (*ReceivedDataCallback)(uint8_t (*CallToGetData)(void));
static bool (*IsCTSPinLow)(void);
static void (*SetRTSPin)(bool setHigh);

// ***** Function Prototypes ***************************************************

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

    /* USART1 clock comes from PCLK2, all other USART's use PCLK1.
    Baud rate equations: Reference manual section 27.3.4. Page 798 */
    uint32_t uartDiv, divFraction;
    float uartDivFloat, divFractionFloat;
    double uartDivMantissa;
    uint8_t carry = 0;

    uartDivFloat = pclkInHz / (desiredBaudRate * 16);

    /* For some reason the reference manual likes to call the integer part 
    of uartDivFloat the mantissa. The modf function will set uartDivFloat to 
    the integer part, and divFraction to the fractional part. */
    divFractionFloat = modf(uartDivFloat, &uartDivMantissa);
    
    uartDiv = (uint32_t)uartDivMantissa;
    divFraction = (uint32_t)roundf(divFractionFloat * 16);

    /* The fractional part is 4-bits. If the rounding goes over 16, add it to
    the "mantissa part" (uartDiv). Example 3. Page 799 */
    if(divFraction > 0x000F) carry = 1;
    divFraction &= 0x0F;
    uartDiv += carry;
    uartDiv <<= 4;
    uartDiv |= divFraction;

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

    /* Turn off module before making changes */
    UART1_ADDR->CR1 &= ~USART_CR1_UE;

    /* Turn off tx and rx interrupts and other bits that I'm going to adjust.
    I will not mess with any others that the user may have changed prior to
    calling this function */
    UART1_ADDR->CR1 &= ~(USART_CR1_RXNEIE | USART_CR1_TXEIE | USART_CR1_M | USART_CR1_PCE);

    /* Set number of data bits, stop bits, and parity */
    if(use9Bit) UART1_ADDR->CR1 |= USART_CR1_M;

    switch(stopBits)
    {
        case UART_HALF_P:
            UART1_ADDR->CR2 |= USART_CR2_STOP_0; // [1:0] = 01
            UART1_ADDR->CR2 &= ~(USART_CR2_STOP_1);
            break;
        case UART_ONE_PLUS_HALF_P:
            UART1_ADDR->CR2 |= (USART_CR2_STOP_1 | USART_CR2_STOP_0);
            break;
        case UART_TWO_P:
            UART1_ADDR->CR2 &= ~(USART_CR2_STOP_0); // [1:0] = 10
            UART1_ADDR->CR2 |= USART_CR2_STOP_1;
            break;
        default:
            UART1_ADDR->CR2 &= ~(USART_CR2_STOP_1 | USART_CR2_STOP_0);
            break;
    }

    if(parity == UART_EVEN_PARITY)
    {
        UART1_ADDR->CR1 &= ~USART_CR1_PS;
        UART1_ADDR->CR1 |= USART_CR1_PCE;
    }
    else if(parity == UART_ODD_PARITY)
    {
        UART1_ADDR->CR1 |= USART_CR1_PS;
        UART1_ADDR->CR1 |= USART_CR1_PCE;
    }

    /* TODO Implement software flow control some day */
    if(flowControl == UART_FLOW_HARDWARE)
    {
        UART1_ADDR->CR3 |= (USART_CR3_CTSE | USART_CR3_RTSE);
    }
    else
    {
        UART1_ADDR->CR3 &= ~(USART_CR3_CTSE | USART_CR3_RTSE);
    }

    /* Set prescale and baud rate. For this processor, prescale is reserved
    for low power (IrDa) use only*/
    UART1_ADDR->BRR = (0x0000FFFF & params->BRGValue);

    /* Peripheral clock enable */
    UART1_CLK_REG |= UART1_CLK_EN_MSK;

    /* If you turn on the transmit interrupt during initialization, it could
    fire off repeatedly. It's best to turn it on after placing data in the 
    transmit register */

    if(useRxInterrupt) UART1_ADDR->CR1 |= USART_CR1_RXNEIE; // rx register not empty interrupt

    UART1_ADDR->CR1 |= USART_CR1_RE; // enable receiver
    UART1_ADDR->CR1 |= USART_CR1_TE; // enable transmitter
    UART1_ADDR->CR1 |= USART_CR1_UE; // enable UART 
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
    uint8_t data = UART1_ADDR->DR;

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

    /* The RX register not empty flag is set when the RDR has a character 
    placed in it. It is cleared by reading the character from RDR */
    if(UART1_ADDR->SR & USART_SR_RXNE)
        rxFull = true;

    /* If the user chooses to poll this function instead of using the receive 
    data event, we must still do something with the RTS pin. */
    if(flowControl == UART_FLOW_CALLBACKS && SetRTSPin != NULL)
    {
        if(UART1_ADDR->SR & USART_SR_RXNE)
            SetRTSPin(true); // "deassert" (high) when full
        else
            SetRTSPin(false);
    }

    return rxFull;
}

// *****************************************************************************

void UART1_ReceiveEnable(void)
{
    UART1_ADDR->CR1 |= USART_CR1_RE;
}

// *****************************************************************************

void UART1_ReceiveDisable(void)
{
    UART1_ADDR->CR1 &= ~USART_CR1_RE;
}

// *****************************************************************************

void UART1_TransmitFinishedEvent(void)
{
    /* This will prevent recursive calls if we call transmit byte function 
    from within the transmit interrupt callback. This requires the process
    function to be called to catch the txFinishedEventPending flag. */
    if(lockTxFinishedEvent == true)
    {
        txFinishedEventPending = true;
        return;
    }
    lockTxFinishedEvent = true;

    /* Disable transmit interrupt here */
    UART1_ADDR->CR1 &= ~USART_CR1_TXEIE;

    if(TransmitFinishedCallback)
    {
        TransmitFinishedCallback();
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
    UART1_ADDR->DR = data;

    /* Enable transmit interrupt here if needed */
    if(useTxInterrupt)
        UART1_ADDR->CR1 |= USART_CR1_TXEIE;
}

// *****************************************************************************

bool UART1_IsTransmitRegisterEmpty(void)
{
    bool txReady = false;

    /* The transmit register empty flag is set when the contents of the TDR 
    register are emptied. It is cleared when the TDR register is written to */
    if(UART1_ADDR->SR & USART_SR_TXE)
        txReady = true;

    /* The transmit empty function also functions as a "transmit ready" sort
    of function. If the user chooses to poll the transmit register empty 
    function, we want to make sure we block input to the transmit register 
    when CTS is asserted */
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
    UART1_ADDR->CR1 |= USART_CR1_TE;
}

// *****************************************************************************

void UART1_TransmitDisable(void)
{
    UART1_ADDR->CR1 &= ~USART_CR1_TE;
}

// *****************************************************************************

void UART1_PendingEventHandler(void)
{
    if(txFinishedEventPending && !lockTxFinishedEvent)
    {
        txFinishedEventPending = false;
        UART1_TransmitFinishedEvent();
    }
}

// *****************************************************************************

void UART1_SetTransmitFinishedCallback(void (*Function)(void))
{
    TransmitFinishedCallback = Function;
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