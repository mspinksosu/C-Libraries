/***************************************************************************//**
 * @brief Basic UART Interface Header
 * 
 * @author Matthew Spinks
 * 
 * @date 12/1/14   Original creation
 * @date 2/4/22    Changed to interface. Added Doxygen
 * @date 3/3/22    Redesigned to use function table. Also added new parameters
 * @date 6/13/22   Changed compute baud rate function and flow control
 * @date 6/25/22   Changed received callback to include a function pointer
 * @date 7/31/22   Added handler for pending transmit interrupt
 * 
 * @file IUART.h
 * 
 * @details
 *      TODO 
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

/* Create the function table */
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
    uint32_t BRGValue; // The raw value for the baud rate generator
    UARTStopBits stopBits;
    UARTParity parity;
    bool use9Bit; 
    UARTFlowControl flowControl;
    bool useTxInterrupt;
    bool useRxInterrupt;
} UARTInitType;

/** 
 * Description of struct
 * 
 * member1      description of variable member1
 */

// ***** Function Prototypes ***************************************************

// ----- Non-Interface Functions -----------------------------------------------

void UART_Create(UART *self, UARTInterface *interface);

void UART_SetInitTypeToDefaultParams(UARTInitType *params);

void UART_SetInitTypeParams(UARTInitType *params, UARTStopBits numStopBits, UARTParity parityType, 
    bool enable9Bit, UARTFlowControl flowControl, bool useRxInterrupt, bool useTxInterrupt);

void UART_SetInitBRGValue(UARTInitType *params, uint32_t BRGValue);

// ----- Interface Functions ---------------------------------------------------

uint32_t UART_ComputeBRGValue(UART *self, uint32_t desiredBaudRate, uint32_t clkInHz);

void UART_Init(UART *self, UARTInitType *params);

void UART_ReceivedDataEvent(UART *self);

uint8_t UART_GetReceivedByte(UART *self);

bool UART_IsReceiveRegisterFull(UART *self);

void UART_ReceiveEnable(UART *self);

void UART_ReceiveDisable(UART *self);

void UART_TransmitFinishedEvent(UART *self);

void UART_TransmitByte(UART *self, uint8_t dataToSend);

bool UART_IsTransmitRegisterEmpty(UART *self);

void UART_TransmitEnable(UART *self);

void UART_TransmitDisable(UART *self);

void UART_PendingEventHandler(UART *self);

void UART_SetTransmitFinishedCallback(UART *self, void (*Function)(void));

void UART_SetReceivedDataCallback(UART *self, void (*Function)(uint8_t (*CallToGetData)(void)));

void UART_SetIsCTSPinLowFunc(UART *self, bool (*Function)(void));

void UART_SetRTSPinFunc(UART *self, void (*Function)(bool setPinHigh));

#endif  /* IUART_H */
