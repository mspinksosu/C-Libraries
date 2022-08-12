/***************************************************************************//**
 * @brief Basic UART Interface Header
 * 
 * @author Matthew Spinks
 * 
 * @date 12/1/14   Original creation
 * @date 2/4/22    Changed to interface. Added Doxygen
 * @date 3/3/22    Redesigned to use function table. Also added new parameters
 * @date 6/13/22   Changed compute baud rate function and flow control
 * @date 7/31/22   Added handler for pending transmit interrupt
 * 
 * @file IUART.h
 * 
 * @details
 *      TODO 
 * 
 * Example Code:
 *      UART myUART, anotherUART;
 *      UART_Create(&myUART, &UART1_FunctionTable);
 *      UART_SetToDefaultParams(&myUART);
 *      uint32_t baud = UART_ComputeBRGValue(&myUART, 115200, 16000000UL);
 *      UART_SetBRGValue(&myUART, baud);
 *      UART_Init(&myUART);
 *      UART_Create(&anotherUART, &UART2FunctionTable);
 *      baud = UART_ComputeBRGValue(&anotherUART, 19200, 16000000UL);
 *      UART_SetBRGValue(&anotherUART, baud);
 *      UART_Init(&anotherUART);
 * 
 * ****************************************************************************/

#ifndef IUART_H
#define IUART_H

// ***** Includes **************************************************************

#include <stdint.h>
#include <stdbool.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

typedef enum UARTStopBitsTag
{
    UART_ONE_P = 0,
    UART_HALF_P,
    UART_ONE_PLUS_HALF_P,
    UART_TWO_P,
} UARTStopBits;

typedef enum UARTParityTag
{
    UART_NO_PARITY = 0,
    UART_EVEN_PARITY,
    UART_ODD_PARITY,
} UARTParity;

typedef enum UARTFlowControlTag
{
    UART_FLOW_NONE = 0,
    UART_FLOW_HARDWARE,
    UART_FLOW_CALLBACKS, // User will implement functions for controlling pins
    UART_FLOW_SOFTWARE,
} UARTFlowControl;

typedef struct UARTInterfaceTag
{
    /*  These are the functions that will be called. You will create your own
    interface object for your class that will have these function signatures.
    Set each of your functions equal to one of these pointers */
    uint32_t (*UART_ComputeBRGValue)(uint32_t, uint32_t);
    void (*UART_Init)(void *instance);
    void (*UART_ReceivedDataEvent)(void);
    uint8_t (*UART_GetReceivedByte)(void);
    bool (*UART_IsReceiveRegisterFull)(void);
    void (*UART_ReceiveEnable)(void);
    void (*UART_ReceiveDisable)(void);
    void (*UART_TransmitFinishedEvent)(void);
    void (*UART_TransmitByte)(uint8_t);
    bool (*UART_IsTransmitRegisterEmpty)(void);
    void (*UART_TransmitEnable)(void);
    void (*UART_TransmitDisable)(void);
    void (*UART_PendingEventHandler)(void);
    void (*UART_SetTransmitFinishedCallback)(void (*Function)(void));
    void (*UART_SetReceivedDataCallback)(void (*Function)(uint8_t (*CallToGetData)(void)));
    void (*UART_SetIsCTSPinLowFunc)(bool (*Function)(void));
    void (*UART_SetRTSPinFunc)(void (*Function)(bool));
} UARTInterface;

typedef struct UARTTag
{
    UARTInterface *interface;
    /* Add any more necessary base class members here */
} UART;

typedef struct UARTInitTypeTag
{
    uint32_t BRGValue;
    UARTStopBits stopBits;
    UARTParity parity;
    bool use9Bit; 
    UARTFlowControl flowControl;
    bool useTxInterrupt;
    bool useRxInterrupt;
} UARTInitType;

/** 
 * Description of struct members. You shouldn't really mess with any of these
 * variables directly. That is why I made functions for you to use.
 * 
 * UARTInterface  The table of functions that need to be implemented
 * 
 * BRGValue  The raw value that gets loaded into the baud rate generator 
 *           register. (not the baud rate!) Varies based on the processor.
 * 
 * stopBits  Most UART's use one stop bit, but they don't have to.
 * 
 * parity  The parity type, none, even, or odd.
 * 
 * use9Bit  Use 9 bit addressing. The actual method varies based on the 
 *          implementation.
 * 
 * flowControl  Hardware means the processor has some sort of dedicated flow
 *              control setting for that peripheral that takes control of the
 *              CTS and RTS pins. Callbacks allows the user make a simple 
 *              function to set the CTS and RTS pins. Software sends a special
 *              flow control message over the line.
 * 
 * useTxInterrupt  Enables or disables the transmit interrupt for you
 *                 automatically. Place the TransmitFinishedEvent function 
 *                 wherever that interrupt happens.
 * 
 * useRxInterrupt  Enables or disables the receive interrupt for you
 *                 automatically. Place the ReceivedDataEvent function wherever 
 *                 that interrupt happens. GetReceivedByte must still be called
 *                 in order to get the data and clear the flag.
 */

// ***** Function Prototypes ***************************************************

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Combine the object and function table 
 * 
 * The UARTInterface is a list of functions for your UART. When you create a
 * a UART implementation, you implement each of the functions listed in the 
 * interface and also assign them to one of the function pointers in the 
 * UARTInterface object.
 * 
 * @param self  
 * @param interface 
 */
void UART_Create(UART *self, UARTInterface *interface);

/***************************************************************************//**
 * @brief 
 * 
 * @param params 
 */
void UART_SetInitTypeToDefaultParams(UARTInitType *params);

/***************************************************************************//**
 * @brief 
 * 
 * @param params 
 * @param numStopBits 
 * @param parityType 
 * @param enable9Bit 
 * @param flowControl 
 * @param useRxInterrupt 
 * @param useTxInterrupt 
 */
void UART_SetInitTypeParams(UARTInitType *params, UARTStopBits numStopBits, UARTParity parityType, 
    bool enable9Bit, UARTFlowControl flowControl, bool useRxInterrupt, bool useTxInterrupt);

/***************************************************************************//**
 * @brief Set the value for the baud rate generator
 * 
 * This is the value that gets loaded directly to the register, NOT the baud 
 * rate.
 * 
 * @param params 
 * @param BRGValue 
 */
void UART_SetInitBRGValue(UARTInitType *params, uint32_t BRGValue);

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief 
 * 
 * @param self  
 * @param desiredBaudRate  
 * @param clkInHz  
 * @return uint32_t  
 */
uint32_t UART_ComputeBRGValue(UART *self, uint32_t desiredBaudRate, uint32_t clkInHz);

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 * @param params 
 */
void UART_Init(UART *self, UARTInitType *params);

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 */
void UART_ReceivedDataEvent(UART *self);

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 * @return uint8_t 
 */
uint8_t UART_GetReceivedByte(UART *self);

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 * @return true 
 */
bool UART_IsReceiveRegisterFull(UART *self);

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 */
void UART_ReceiveEnable(UART *self);

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 */
void UART_ReceiveDisable(UART *self);

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 */
void UART_TransmitFinishedEvent(UART *self);

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 * @param dataToSend 
 */
void UART_TransmitByte(UART *self, uint8_t dataToSend);

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 * @return true 
 */
bool UART_IsTransmitRegisterEmpty(UART *self);

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 */
void UART_TransmitEnable(UART *self);

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 */
void UART_TransmitDisable(UART *self);

/***************************************************************************//**
 * @brief Checks for pending transmit finished events
 * 
 * Because the transmit finished callback is called within the interrupt, if
 * the user wants to transmit another byte, the transmit finished interrupt
 * will almost certainly fire before the current callback is done. This can
 * lead to multiple recursive function calls. Call this function in a loop 
 * continously and it will check for a pending interrupt for you. This will
 * let the stack unwind. Note that this is really only an issue if you are 
 * using interrupts to transmit.
 */
void UART_PendingEventHandler(UART *self);

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 * @param Function 
 */
void UART_SetTransmitFinishedCallback(UART *self, void (*Function)(void));

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 * @param Function 
 */
void UART_SetReceivedDataCallback(UART *self, void (*Function)(uint8_t (*CallToGetData)(void)));

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 * @param Function 
 */
void UART_SetIsCTSPinLowFunc(UART *self, bool (*Function)(void));

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 * @param Function 
 */
void UART_SetRTSPinFunc(UART *self, void (*Function)(bool setPinHigh));

#endif  /* IUART_H */
