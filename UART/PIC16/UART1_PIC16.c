/***************************************************************************//**
 * @brief Basic UART for PIC (interrupt driven)
 * 
 * @author Matthew Spinks
 * 
 * @date 12/1/15    Original creation
 * @date 2/4/22     Added Doxygen
 * @date 3/9/22     Reformatted to use function table. Added preprocessor
 *                  macros and defines for PIC registers
 * 
 * @file UART1_PIC16_Int.c
 * 
 * @details
 *      TODO Add details, 9-bit, and different parity, hardware flow control
 * 
 * ****************************************************************************/

#include <stddef.h>
#include "UART1.h"

/* Include processor specific header files here */
#include <xc.h>

// ***** Defines ***************************************************************

/* These will change the percent error of your baud rate generator based on 
your crystal frequency. Typically, these are both set to 1 to reach 115200 
baud. If you are unsure, check the reference manual. */
#define UART_BRG16     1   // use 16-bit baud rate generator
#define UART_BRGH      1   // high speed brg

#if (UART_BRG16 && UART_BRGH)
#define UART_BRG_DIV   4
#elif (UART_BRGH)
#define UART_BRG_DIV   16
#else
#define UART_BRG_DIV   64
#endif

/* Registers */
#define PIExbits      PIE3bits
#define PIRxbits      PIR3bits
#define RCxSTAbits    RC1STAbits
#define TXxSTAbits    TX1STAbits
#define BAUDxCONbits  BAUD1CONbits
#define SPxBRGH       SP2BRGH
#define SPxBRGL       SP1BRGL
#define RCxREG        RC1REG
#define TXxREG        TX1REG

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
    .UART_TransmitEnable = UART1_TransmitEnable,
    .UART_TransmitDisable = UART1_TransmitDisable,
    .UART_SetTransmitRegisterEmptyCallback = UART1_SetTransmitRegisterEmptyCallback,
    .UART_SetReceivedDataCallback = UART1_SetReceivedDataCallback,
    .UART_SetCTSPinFunc = UART1_SetCTSPinFunc,
    .UART_SetRTSPinFunc = UART1_SetRTSPinFunc,
};

static bool use9Bit = false, useRxInterrupt = false, useTxInterrupt = false;
static UARTFlowControl flowControl = UART_FLOW_NONE;
static UARTStopBits stopBits = UART_ONE_P;
static UARTParity parity = UART_NO_PARITY;

// local function pointers
static void (*UART_TransmitRegisterEmptyCallback)(void);
static void (*UART_ReceivedDataCallback)(void);
static void (*UART_CTSPinFunc)(void);
static void (*UART_RTSPinFunc)(void);

// ***** Static Function Prototypes ********************************************

/* Put static function prototypes here */


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

uint32_t UART1_ComputeBRGValue(uint32_t desiredBaudRate, float clkInMHz)
{
    return ((clkInMHz * 1000000UL) / (UART1_BRG_DIV * desiredBaudRate) - 1);
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

    /* TODO Add code to alter parity and stop bits */

    // Disable interrupts before changing states
    PIExbits.RCIE = 0;
    PIExbits.TXIE = 0;

    RCxSTAbits.RX9 = use9Bit;
    RCxSTAbits.ADDEN = use9Bit;
    RCxSTAbits.CREN = 1; // enable continuous receive
    RCxSTAbits.SPEN = 1; // enable serial port

    TXxSTAbits.SYNC = 0; // asynchronous
    TXxSTAbits.TX9 = use9Bit;
    TXxSTAbits.TXEN = 1; // enable transmitter

    TXxSTAbits.BRGH = UART1_BRGH; // high baud rate, baudclk/4
    BAUDxCONbits.BRG16 = UART1_BRG16; // 16-bit baud generator

    SPxBRGL = (uint8_t)params->BRGValue;
    SPxBRGH = (uint8_t)(params->BRGValue >> 8);

    PIExbits.RCIE = 1; // enable receive interrupt
}

// *****************************************************************************

void UART1_ReceivedDataEvent(void)
{
    /* Clear any interrupt flags here if needed */

    if(1 == RCxSTAbits.OERR)
    {
        // EUSART error - restart
        RCxSTAbits.CREN = 0;
        RCxSTAbits.CREN = 1;
    }

    if(UART_ReceivedDataCallback)
    {
        UART_ReceivedDataCallback();
    }
}

// *****************************************************************************

uint8_t UART1_GetReceivedByte(void)
{
    return RCxREG;
}

// *****************************************************************************

void UART1_TransmitByte(uint8_t data)
{
    /* For this function I will be making use of interrupts to transmit my data.
    Writing to TX_REG will clear the TXIF flag after one instruction cycle */
    TXxREG = data;
    
    PIExbits.TXIE = 1; // enable transmit interrupt
}

// *****************************************************************************

void UART1_TransmitFinished(void)
{
    /* Clear any interrupt flags here if needed */

    if(UART_TransmitRegisterEmptyCallback)
    {
        UART_TransmitRegisterEmptyCallback();
    }
}

// *****************************************************************************

void UART1_ReceiveEnable(void)
{
    PIExbits.RCIE = 1;
}

// *****************************************************************************

void UART1_ReceiveDisable(void)
{
    PIExbits.RCIE = 0;
}

// *****************************************************************************

bool UART1_IsReceiveRegisterFull(void)
{
    /* The receive character interrupt flag is set whenever there is an unread
    character and is cleared by reading the character */
    if(PIRxbits.RC1IF)
        return true;
    else
        return false;
}

// *****************************************************************************

void UART1_TransmitEnable(void)
{
    PIExbits.TXIE = 1;
}

// *****************************************************************************

void UART1_TransmitDisable(void)
{
    PIExbits.TXIE = 0;
}

// *****************************************************************************

bool UART1_IsTransmitRegisterEmpty(void)
{
    /* The transmit interrupt flag is set whenever the transmitter is enabled
    and there is no character in the register for transmission */
    if(PIRxbits.TX1IF)
        return true;
    else
        return false;
}

// *****************************************************************************

void UART1_SetTransmitRegisterEmptyCallback(void (*Function)(void))
{
    UART_TransmitRegisterEmptyCallback = Function;
}

// *****************************************************************************

void UART1_SetReceivedDataCallback(void (*Function)(void))
{
    UART_ReceivedDataCallback = Function;
}

// *****************************************************************************

void UART1_SetCTSPinFunc(void (*Function)(bool))
{
    UART_CTSPinFunc = Function;
}

// *****************************************************************************

void UART1_SetRTSPinFunc(void (*Function)(bool))
{
    UART_RTSPinFunc = Function;
}

/*
 End of File
*/