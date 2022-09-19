/***************************************************************************//**
 * @brief UART Library Implementation (STM32G0)
 * 
 * @author Matthew Spinks
 * 
 * @date 3/12/22   Original creation
 * @date 6/13/22   Changed compute baud rate function to match interface
 * 
 * @file UART1_STM32G0.c
 * 
 * @details
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

#include "UART1.h"
#include <stddef.h> // needed for NULL

/* Include processor specific header files here */
#include "stm32g071xx.h"

// ***** Defines ***************************************************************

enum {
    UART1_PRE_NO_DIV,
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
#define UART1_PRESCALE  UART1_PRE_NO_DIV // choose from the enum
#define OVER8           0                // 0 = oversample 16, 1 = oversample 8
// -----------------------------------------------------------------------------

/* Peripheral addresses and registers */
#define UART1_ADDR          USART1
#define UART1_CLK_REG       RCC->APBENR2
#define UART1_CLK_EN_MSK    RCC_APBENR2_USART1EN

static const uint8_t preLUT[16] = {0,2,4,6,8,10,12,16,32,64,128,256};

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
    if(desiredBaudRate == 0)
        return 0;

    uint32_t uartDiv;

    if(OVER8) 
    {
        uartDiv = (uint32_t)(2 * pclkInHz / preLUT[UART1_PRESCALE]) / desiredBaudRate;
        // BRR[2:0] = lower 4 bits shifted one to the right. Ref man page 1018
        uint32_t BRRLowerByte = (uartDiv & 0x000F) >> 1;
        uartDiv &= 0xFFF0;
        uartDiv |= BRRLowerByte;
    }
    else
    {
        uartDiv = (uint32_t)(pclkInHz / preLUT[UART1_PRESCALE]) / desiredBaudRate;
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

    /* Turn off module before making changes */
    UART1_ADDR->CR1 &= ~USART_CR1_UE;

    /* Turn off tx and rx interrupts and other bits that I'm going to adjust.
    I will not mess with any others that the user may have changed prior to
    calling this function */
    UART1_ADDR->CR1 &= ~(USART_CR1_RXNEIE_RXFNEIE | USART_CR1_TXEIE_TXFNFIE | USART_CR1_M | USART_CR1_M0 | USART_CR1_PCE);
    
    /* Set number of data bits, stop bits, and parity */
    if(use9Bit)  UART1_ADDR->CR1 |= USART_CR1_M0;

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

    /* Set prescale and baud rate. Use the defines at the top */
    UART1_ADDR->PRESC &= ~USART_PRESC_PRESCALER;
    UART1_ADDR->PRESC |= UART1_PRESCALE;

    UART1_ADDR->CR1 &= ~USART_CR1_OVER8;
    if(OVER8) UART1_ADDR->CR1 |= USART_CR1_OVER8;

    UART1_ADDR->BRR = (0x0000FFFF & params->BRGValue);

    /* Peripheral clock enable */
    UART1_CLK_REG |= UART1_CLK_EN_MSK;

    /* If you turn on the transmit interrupt during initialization, it could
    fire off repeatedly. It's best to turn it on after placing data in the 
    transmit register */

    if(useRxInterrupt)
        UART1_ADDR->CR1 |= USART_CR1_RXNEIE_RXFNEIE; // rx register not empty interrupt

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
    if(UART1_ADDR->ISR & USART_ISR_RXNE_RXFNE)
        rxFull = true;

    /* If the user chooses to poll this function instead of using the receive 
    data event, we must still do something with the RTS pin. */
    if(flowControl == UART_FLOW_CALLBACKS && SetRTSPin != NULL)
    {
        if(rxFull)
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

void UART1_TransmitRegisterEmptyEvent(void)
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
    UART1_ADDR->CR1 &= ~USART_CR1_TXEIE_TXFNFIE;

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
    UART1_ADDR->DR = data;

    /* Enable transmit interrupt here if needed */
    if(useTxInterrupt)
        UART1_ADDR->CR1 |= USART_CR1_TXEIE_TXFNFIE;
}

// *****************************************************************************

bool UART1_IsTransmitRegisterEmpty(void)
{
    bool txReady = false;

    /* The transmit register empty flag is set when the contents of the TDR 
    register are emptied. It is cleared when the TDR register is written to */
    if(UART1_ADDR->ISR & USART_ISR_TXE_TXFNF)
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