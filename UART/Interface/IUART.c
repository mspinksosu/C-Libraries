/***************************************************************************//**
 * @brief Basic UART Interface
 * 
 * @author Matthew Spinks
 * 
 * @date 12/2/14   Original creation
 * @date 2/4/22    Changed to interface. Added Doxygen
 * @date 3/3/22    Redesigned function table
 * @date 6/13/22   Changed compute baud rate function and flow control
 * @date 6/25/22   Changed received callback to include a function pointer
 * @date 7/31/22   Added handler for pending transmit interrupt
 * 
 * @file IUART.c
 * 
 * @details
 *      TODO details
 * 
 * Example Code:
 *      UART myUART, anotherUART;
 *      UART_Create(&myUART, &UART1_FunctionTable);
 *      UART_SetToDefaultParams(&myUART);
 *      uint32_t baud = UART_ComputeBRGValue(&myUART, 115200, 12.0);
 *      UART_SetBRGValue(&myUART, baud);
 *      UART_Init(&myUART);
 *      UART_Create(&anotherUART, &UART2FunctionTable);
 *      baud = UART_ComputeBRGValue(&anotherUART, 19200, 12.0);
 *      UART_SetBRGValue(&anotherUART, baud);
 *      UART_Init(&anotherUART);
 * 
 ******************************************************************************/

#include <stddef.h>
#include "IUART.h"

// ***** Defines ***************************************************************


// ***** Function Prototypes ***************************************************


// ***** Global Variables ******************************************************

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
 * 
 * @param interface 
 */
void UART_Create(UART *self, UARTInterface *interface)
{
    self->interface = interface;
}

/***************************************************************************//**
 * @brief 
 * 
 * @param params  
 */
void UART_SetInitTypeToDefaultParams(UARTInitType *params)
{
    params->stopBits = UART_ONE_P;
    params->parity = UART_NO_PARITY;
    params->use9Bit = false;
    params->flowControl = UART_FLOW_NONE;
    params->useRxInterrupt = false;
    params->useTxInterrupt = false;
}

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
    bool enable9Bit, UARTFlowControl flowControl, bool useRxInterrupt, bool useTxInterrupt)
{
    params->stopBits = numStopBits;
    params->parity = parityType;
    params->use9Bit = enable9Bit;
    params->flowControl = flowControl;
    params->useRxInterrupt = useRxInterrupt;
    params->useTxInterrupt = useTxInterrupt;
}

/***************************************************************************//**
 * @brief Set the value for the baud rate generator
 * 
 * This is the value that gets loaded directly to the register, NOT the baud 
 * rate.
 * 
 * @param params  
 * 
 * @param BRGValue  
 */
void UART_SetInitBRGValue(UARTInitType *params, uint32_t BRGValue)
{
    params->BRGValue = BRGValue; 
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief 
 * 
 * @param self  
 * 
 * @param desiredBaudRate  
 * 
 * @param clkInHz  
 * 
 * @return uint32_t  
 */
uint32_t UART_ComputeBRGValue(UART *self, uint32_t desiredBaudRate, uint32_t clkInHz)
{
    if(self->interface->UART_ComputeBRGValue != NULL)
    {
        return (self->interface->UART_ComputeBRGValue)(desiredBaudRate, clkInHz);
    }
    else
    {
        return 0;
    }
}

/***************************************************************************//**
 * @brief 
 * 
 * @param self  
 */
void UART_Init(UART *self, UARTInitType *params)
{
    if(self->interface->UART_Init != NULL && params != NULL)
    {
        (self->interface->UART_Init)(params);
    }
}

/***************************************************************************//**
 * @brief 
 * 
 * @param self  
 */
void UART_ReceivedDataEvent(UART *self)
{
    if(self->interface->UART_ReceivedDataEvent != NULL)
    {
        (self->interface->UART_ReceivedDataEvent)();
    }
}

/***************************************************************************//**
 * @brief 
 * 
 * @param self  
 * 
 * @return uint8_t  
 */
uint8_t UART_GetReceivedByte(UART *self)
{   
    if(self->interface->UART_GetReceivedByte != NULL)
    {
        return (self->interface->UART_GetReceivedByte)();
    }
    else
    {
        return 0;
    }
}

/***************************************************************************//**
 * @brief 
 * 
 * @param self  
 * 
 * @return  
 */
bool UART_IsReceiveRegisterFull(UART *self)
{
    if(self->interface->UART_IsReceiveRegisterFull != NULL)
    {
        return (self->interface->UART_IsReceiveRegisterFull)();
    }
    else
    {
        return false;
    }
}

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 */
void UART_ReceiveEnable(UART *self)
{
    if(self->interface->UART_ReceiveEnable != NULL)
    {
        (self->interface->UART_ReceiveEnable)();
    }
}

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 */
void UART_ReceiveDisable(UART *self)
{
    if(self->interface->UART_ReceiveDisable != NULL)
    {
        (self->interface->UART_ReceiveDisable)();
    }
}

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 */
void UART_TransmitFinishedEvent(UART *self)
{
    if(self->interface->UART_TransmitFinishedEvent != NULL)
    {
        (self->interface->UART_TransmitFinishedEvent)();
    }
}

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 * 
 * @param dataToSend 
 */
void UART_TransmitByte(UART *self, uint8_t dataToSend)
{
    if(self->interface->UART_TransmitByte != NULL)
    {
        (self->interface->UART_TransmitByte)(dataToSend);
    }
}

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 * 
 * @return true 
 */
bool UART_IsTransmitRegisterEmpty(UART *self)
{
    if(self->interface->UART_IsTransmitRegisterEmpty != NULL)
    {
        return (self->interface->UART_IsTransmitRegisterEmpty)();
    }
    else
    {
        return false;
    }
}

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 */
void UART_TransmitEnable(UART *self)
{
    if(self->interface->UART_TransmitEnable != NULL)
    {
        (self->interface->UART_TransmitEnable)();
    }
}

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 */
void UART_TransmitDisable(UART *self)
{
    if(self->interface->UART_TransmitDisable != NULL)
    {
        (self->interface->UART_TransmitDisable)();
    }
}

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 */
void UART_PendingEventHandler(UART *self)
{
    if(self->interface->UART_PendingEventHandler != NULL)
    {
        (self->interface->UART_PendingEventHandler)();
    }
}

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 * 
 * @param Function 
 */
void UART_SetTransmitFinishedCallback(UART *self, void (*Function)(void))
{
    if(self->interface->UART_SetTransmitFinishedCallback != NULL)
    {
        (self->interface->UART_SetTransmitFinishedCallback)(Function);
    }
}

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 * 
 * @param Function 
 */
void UART_SetReceivedDataCallback(UART *self, void (*Function)(uint8_t (*CallToGetData)(void)))
{
    if(self->interface->UART_SetReceivedDataCallback != NULL)
    {
        (self->interface->UART_SetReceivedDataCallback)(Function);
    }
}

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 * 
 * @param Function 
 */
void UART_SetIsCTSPinLowFunc(UART *self, bool (*Function)(void))
{
    if(self->interface->UART_SetIsCTSPinLowFunc != NULL)
    {
        (self->interface->UART_SetIsCTSPinLowFunc)(Function);
    }
}

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 * 
 * @param Function 
 */
void UART_SetRTSPinFunc(UART *self, void (*Function)(bool))
{
    if(self->interface->UART_SetRTSPinFunc != NULL)
    {
        (self->interface->UART_SetRTSPinFunc)(Function);
    }
}

/*
 End of File
 */