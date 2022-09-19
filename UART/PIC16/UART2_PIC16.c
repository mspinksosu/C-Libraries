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
 * @file UART2_PIC16_Int.c
 * 
 * @details
 *      TODO Add details, 9-bit, and different parity, hardware flow control
 * 
 * ****************************************************************************/

#include "UART2.h"

/* Include processor specific header files here */
#include <xc.h>

// ***** Defines ***************************************************************

/* These will change the percent error of your baud rate generator based on 
your crystal frequency. Typically, these are both set to 1 to reach 115200 
baud. If you are unsure, check the reference manual. */
#define UART2_BRG16     1   // use 16-bit baud rate generator
#define UART2_BRGH      1   // high speed brg

// -----------------------------------------------------------------------------
#if (UART2_BRG16 && UART2_BRGH)
#define UART2_BRG_DIV   4
#elif (UART2_BRGH)
#define UART2_BRG_DIV   16
#else
#define UART2_BRG_DIV   64
#endif

//#define UART2_ComputeBRGValue(desiredBaudRate, clkInMHz) ((clkInMHz * 1000000UL) / (UART2_BRG_DIV * desiredBaudRate) - 1)

/* Registers */
#define UART2_PIExbits      PIE3bits
#define UART2_PIRxbits      PIR3bits    
#define UART2_RCxSTAbits    RC1STAbits
#define UART2_TXxSTAbits    TX1STAbits
#define UART2_BAUDxCONbits  BAUD1CONbits
#define UART2_SPxBRGH       SP2BRGH
#define UART2_SPxBRGL       SP1BRGL
#define UART2_RCxREG        RC1REG
#define UART2_TXxREG        TX1REG

// ***** Static Function Prototypes ********************************************

/* Put static function prototypes here */
static inline uint32_t UART2_ComputeBRGValue(uint32_t desiredBaudRate, float clkInMHz)
{
    return ((clkInMHz * 1000000UL) / (UART1_BRG_DIV * desiredBaudRate) - 1);
}

// ***** Global Variables ******************************************************

/* Assign functions to the interface */
UARTInterface UART2_FunctionTable = {
    .UART_ComputeBRGValue = UART1_ComputeBRGValue,
    .UART_Init = UART2_Init,
    .UART_ReceivedDataEvent = UART2_ReceivedDataEvent,
    .UART_GetReceivedByte = UART2_GetReceivedByte,
    .UART_IsReceiveRegisterFull = UART2_IsReceiveRegisterFull,
    .UART_ReceiveEnable = UART2_ReceiveEnable,
    .UART_ReceiveDisable = UART2_ReceiveDisable,
    .UART_TransmitRegisterEmptyEvent = UART2_TransmitRegisterEmptyEvent,
    .UART_TransmitByte = UART2_TransmitByte,
    .UART_IsTransmitRegisterEmpty = UART2_IsTransmitRegisterEmpty,
    .UART_TransmitEnable = UART2_TransmitEnable,
    .UART_TransmitDisable = UART2_TransmitDisable,
    .UART_SetTransmitRegisterEmptyCallback = UART2_SetTransmitRegisterEmptyCallback,
    .UART_SetReceivedDataCallback = UART2_SetReceivedDataCallback,
    .UART_SetCTSPinFunc = UART2_SetCTSPinFunc,
    .UART_SetRTSPinFunc = UART2_SetRTSPinFunc,
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
void UART2_Init(UART *self)
{
    /* In case the user chooses to call this function directly instead of
    calling the base UART_Init function */
    if(self->interface == NULL)
    {
        UART_Create(self, &UART2_FunctionTable);
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
    UART2_PIExbits.RCIE = 0;
    UART2_PIExbits.TXIE = 0;

    UART2_RCxSTAbits.RX9 = use9Bit;
    UART2_RCxSTAbits.ADDEN = use9Bit;
    UART2_RCxSTAbits.CREN = 1; // enable continuous receive
    UART2_RCxSTAbits.SPEN = 1; // enable serial port

    UART2_TXxSTAbits.SYNC = 0; // asynchronous
    UART2_TXxSTAbits.TX9 = use9Bit;
    UART2_TXxSTAbits.TXEN = 1; // enable transmitter

    UART2_TXxSTAbits.BRGH = UART2_BRGH; // high baud rate, baudclk/4
    UART2_BAUDxCONbits.BRG16 = UART2_BRG16; // 16-bit baud generator

    UART2_SPxBRGL = (uint8_t)self->BRGValue;
    UART2_SPxBRGH = (uint8_t)(self->BRGValue >> 8);

    UART2_PIExbits.RCIE = 1; // enable receive interrupt
}

/***************************************************************************//**
 * @brief 
 * 
 */
void UART2_ReceivedDataEvent(void)
{
    /* Clear any interrupt flags here if needed */

    if(1 == UART2_RCxSTAbits.OERR)
    {
        // EUSART error - restart
        UART2_RCxSTAbits.CREN = 0;
        UART2_RCxSTAbits.CREN = 1;
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
uint8_t UART2_GetReceivedByte(void)
{
    return UART2_RCxREG;
}

/***************************************************************************//**
 * @brief 
 * 
 * @param data 
 */
void UART2_TransmitByte(uint8_t data)
{
    /* For this function I will be making use of interrupts to transmit my data.
    Writing to TX_REG will clear the TXIF flag after one instruction cycle */
    UART2_TXxREG = data;
    
    UART2_PIExbits.TXIE = 1; // enable transmit interrupt
}

/***************************************************************************//**
 * @brief 
 * 
 */
void UART2_TransmitFinished(void)
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
void UART2_ReceiveEnable(void)
{
    UART2_PIExbits.RCIE = 1;
}

/***************************************************************************//**
 * @brief 
 * 
 */
void UART2_ReceiveDisable(void)
{
    UART2_PIExbits.RCIE = 0;
}

/***************************************************************************//**
 * @brief 
 * 
 * @return true 
 */
bool UART2_IsReceiveRegisterFull(void)
{
    /* The receive character interrupt flag is set whenever there is an unread
    character and is cleared by reading the character */
    if(UART2_PIRxbits.RC1IF)
        return true;
    else
        return false;
}

/***************************************************************************//**
 * @brief 
 * 
 */
void UART2_TransmitEnable(void)
{
    UART2_PIExbits.TXIE = 1;
}

/***************************************************************************//**
 * @brief 
 * 
 */
void UART2_TransmitDisable(void)
{
    UART2_PIExbits.TXIE = 0;
}

/***************************************************************************//**
 * @brief 
 * 
 * @return true 
 */
bool UART2_IsTransmitRegisterEmpty(void)
{
    /* The transmit interrupt flag is set whenever the transmitter is enabled
    and there is no character in the register for transmission */
    if(UART2_PIRxbits.TX1IF)
        return true;
    else
        return false;
}

/***************************************************************************//**
 * @brief 
 * 
 * @param Function 
 */
void UART2_SetTransmitRegisterEmptyCallback(void (*Function)(void))
{
    UART_TransmitRegisterEmptyCallback = Function;
}

/***************************************************************************//**
 * @brief 
 * 
 * @param Function 
 */
void UART2_SetReceivedDataCallback(void (*Function)(void))
{
    UART_ReceivedDataCallback = Function;
}

/***************************************************************************//**
 * @brief 
 * 
 * @param Function 
 */
void UART2_SetCTSPinFunc(void (*Function)(bool))
{
    UART_CTSPinFunc = Function;
}

/***************************************************************************//**
 * @brief 
 * 
 * @param Function 
 */
void UART2_SetRTSPinFunc(void (*Function)(bool))
{
    UART_RTSPinFunc = Function;
}

/**
 End of File
*/