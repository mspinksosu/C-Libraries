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
#define UART1_BRG16     1   // use 16-bit baud rate generator
#define UART1_BRGH      1   // high speed brg

// -----------------------------------------------------------------------------
#if (UART1_BRG16 && UART1_BRGH)
#define UART1_BRG_DIV   4
#elif (UART1_BRGH)
#define UART1_BRG_DIV   16
#else
#define UART1_BRG_DIV   64
#endif

//#define UART1_ComputeBRGValue(desiredBaudRate, clkInMHz) ((clkInMHz * 1000000UL) / (UART1_BRG_DIV * desiredBaudRate) - 1)

/* Registers */
#define UART1_PIExbits      PIE3bits
#define UART1_PIRxbits      PIR3bits
#define UART1_RCxSTAbits    RC1STAbits
#define UART1_TXxSTAbits    TX1STAbits
#define UART1_BAUDxCONbits  BAUD1CONbits
#define UART1_SPxBRGH       SP2BRGH
#define UART1_SPxBRGL       SP1BRGL
#define UART1_RCxREG        RC1REG
#define UART1_TXxREG        TX1REG

// ***** Static Function Prototypes ********************************************

/* Put static function prototypes here */
static inline uint32_t UART1_ComputeBRGValue(uint32_t desiredBaudRate, float clkInMHz)
{
    return ((clkInMHz * 1000000UL) / (UART1_BRG_DIV * desiredBaudRate) - 1);
}

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

static bool use9Bit = false, flowControl = false;
static UARTStopBits stopBits = UART_ONE_P;
static UARTParity parity = UART_NO_PARITY;

// local function pointers
static void (*UART_TransmitRegisterEmptyCallback)(void);
static void (*UART_ReceivedDataCallback)(void);
static void (*UART_CTSPinFunc)(void);
static void (*UART_RTSPinFunc)(void);

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 */
void UART1_Init(UART *self)
{
    /* In case the user chooses to call this function directly instead of
    calling the base UART_Init function */
    if(self->interface == NULL)
    {
        UART_Create(self, &UART1_FunctionTable);
    }

    if(self->BRGValue == 0)
    {
        return;
    }

    use9Bit = self->use9Bit;
    flowControl = self->flowControl;
    stopBits = self->stopBits;
    parity = self->parity;

    /* TODO Add code to alter parity and stop bits */

    // Disable interrupts before changing states
    UART1_PIExbits.RCIE = 0;
    UART1_PIExbits.TXIE = 0;

    UART1_RCxSTAbits.RX9 = use9Bit;
    UART1_RCxSTAbits.ADDEN = use9Bit;
    UART1_RCxSTAbits.CREN = 1; // enable continuous receive
    UART1_RCxSTAbits.SPEN = 1; // enable serial port

    UART1_TXxSTAbits.SYNC = 0; // asynchronous
    UART1_TXxSTAbits.TX9 = use9Bit;
    UART1_TXxSTAbits.TXEN = 1; // enable transmitter

    UART1_TXxSTAbits.BRGH = UART1_BRGH; // high baud rate, baudclk/4
    UART1_BAUDxCONbits.BRG16 = UART1_BRG16; // 16-bit baud generator

    UART1_SPxBRGL = (uint8_t)self->BRGValue;
    UART1_SPxBRGH = (uint8_t)(self->BRGValue >> 8);

    UART1_PIExbits.RCIE = 1; // enable receive interrupt
}

/***************************************************************************//**
 * @brief 
 * 
 */
void UART1_ReceivedDataEvent(void)
{
    /* Clear any interrupt flags here if needed */

    if(1 == UART1_RCxSTAbits.OERR)
    {
        // EUSART error - restart
        UART1_RCxSTAbits.CREN = 0;
        UART1_RCxSTAbits.CREN = 1;
    }

    if(UART_ReceivedDataCallback)
    {
        UART_ReceivedDataCallback();
    }
}

/***************************************************************************//**
 * @brief 
 * 
 * @return uint8_t 
 */
uint8_t UART1_GetReceivedByte(void)
{
    return UART1_RCxREG;
}

/***************************************************************************//**
 * @brief 
 * 
 * @param data 
 */
void UART1_TransmitByte(uint8_t data)
{
    /* For this function I will be making use of interrupts to transmit my data.
    Writing to TX_REG will clear the TXIF flag after one instruction cycle */
    UART1_TXxREG = data;
    
    UART1_PIExbits.TXIE = 1; // enable transmit interrupt
}

/***************************************************************************//**
 * @brief 
 * 
 */
void UART1_TransmitFinished(void)
{
    /* Clear any interrupt flags here if needed */

    if(UART_TransmitRegisterEmptyCallback)
    {
        UART_TransmitRegisterEmptyCallback();
    }
}

/***************************************************************************//**
 * @brief 
 * 
 */
void UART1_ReceiveEnable(void)
{
    UART1_PIExbits.RCIE = 1;
}

/***************************************************************************//**
 * @brief 
 * 
 */
void UART1_ReceiveDisable(void)
{
    UART1_PIExbits.RCIE = 0;
}

/***************************************************************************//**
 * @brief 
 * 
 * @return true 
 */
bool UART1_IsReceiveRegisterFull(void)
{
    /* The receive character interrupt flag is set whenever there is an unread
    character and is cleared by reading the character */
    if(UART1_PIRxbits.RC1IF)
        return true;
    else
        return false;
}

/***************************************************************************//**
 * @brief 
 * 
 */
void UART1_TransmitEnable(void)
{
    UART1_PIExbits.TXIE = 1;
}

/***************************************************************************//**
 * @brief 
 * 
 */
void UART1_TransmitDisable(void)
{
    UART1_PIExbits.TXIE = 0;
}

/***************************************************************************//**
 * @brief 
 * 
 * @return true 
 */
bool UART1_IsTransmitRegisterEmpty(void)
{
    /* The transmit interrupt flag is set whenever the transmitter is enabled
    and there is no character in the register for transmission */
    if(UART1_PIRxbits.TX1IF)
        return true;
    else
        return false;
}

/***************************************************************************//**
 * @brief 
 * 
 * @param Function 
 */
void UART1_SetTransmitRegisterEmptyCallback(void (*Function)(void))
{
    UART_TransmitRegisterEmptyCallback = Function;
}

/***************************************************************************//**
 * @brief 
 * 
 * @param Function 
 */
void UART1_SetReceivedDataCallback(void (*Function)(void))
{
    UART_ReceivedDataCallback = Function;
}

/***************************************************************************//**
 * @brief 
 * 
 * @param Function 
 */
void UART1_SetCTSPinFunc(void (*Function)(bool))
{
    UART_CTSPinFunc = Function;
}

/***************************************************************************//**
 * @brief 
 * 
 * @param Function 
 */
void UART1_SetRTSPinFunc(void (*Function)(bool))
{
    UART_RTSPinFunc = Function;
}

/**
 End of File
*/