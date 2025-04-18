/***************************************************************************//**
 * @brief I2C Interface
 * 
 * @file II2C.c
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 3/24/25   Original creation
 * 
 * @details
 *      // @todo details
 * 
 * @section license License
 * SPDX-FileCopyrightText: © 2022 Matthew Spinks
 * SPDX-License-Identifier: Zlib
 * 
 * This software is released under the Zlib license. You are free alter and
 * redistribute it, but you must not misrepresent the origin of the software.
 * This notice may not be removed. <http://www.zlib.net/zlib_license.html>
 * 
 ******************************************************************************/

#include "II2C.h"
#include <stddef.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************


// ***** Static Function Prototypes ********************************************


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void I2C_Create(I2C *self, I2CInterface *interface)
{
    self->interface = interface;
}

// *****************************************************************************

void I2C_SetInitTypeToDefaultParams(I2CInitType *params)
{
    params->role = I2C_ROLE_MASTER;
    params->useRxInterrupt = false;
    params->useTxInterrupt = false;
}

// *****************************************************************************

void I2C_SetInitTypeParams(I2CInitType *params, bool useRxInterrupt, 
    bool useTxInterrupt, I2CRole role)
{
    params->role = role;
    params->useRxInterrupt = useRxInterrupt;
    params->useTxInterrupt = useTxInterrupt;
}

// *****************************************************************************

void I2C_SetInitBRGValue(I2CInitType *params, uint32_t BRGValue)
{
    params->BRGValue = BRGValue; 
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

uint32_t I2C_ComputeBRGValue(I2C *self, uint32_t desiredBaudRate, uint32_t clkInHz)
{
    if(self->interface->I2C_ComputeBRGValue != NULL)
    {
        return (self->interface->I2C_ComputeBRGValue)(desiredBaudRate, clkInHz);
    }
    else
    {
        return 0;
    }
}

// *****************************************************************************

void I2C_Init(I2C *self, I2CInitType *params)
{
    if(self->interface->I2C_Init != NULL && params != NULL)
    {
        (self->interface->I2C_Init)(params);
    }
}

// *****************************************************************************

void I2C_Enable(I2C *self)
{
    if(self->interface->I2C_Enable != NULL)
    {
        (self->interface->I2C_Enable)();
    }
}

// *****************************************************************************

void I2C_Disable(I2C *self)
{
    if(self->interface->I2C_Disable != NULL)
    {
        (self->interface->I2C_Disable)();
    }
}

// *****************************************************************************

bool I2C_IsEnabled(I2C *self)
{
    if(self->interface->I2C_IsEnabled != NULL)
    {
        return (self->interface->I2C_IsEnabled)();
    }
    else
    {
        return false;
    }
}

// *****************************************************************************

void I2C_ReceivedDataEvent(I2C *self)
{
    if(self->interface->I2C_ReceivedDataEvent != NULL)
    {
        (self->interface->I2C_ReceivedDataEvent)();
    }
}

// *****************************************************************************

uint8_t I2C_GetReceivedByte(I2C *self)
{
    if(self->interface->I2C_GetReceivedByte != NULL)
    {
        return (self->interface->I2C_GetReceivedByte)();
    }
    else
    {
        return 0;
    }
}

// *****************************************************************************

bool I2C_IsReceiveRegisterFull(I2C *self)
{
    if(self->interface->I2C_IsReceiveRegisterFull != NULL)
    {
        return (self->interface->I2C_IsReceiveRegisterFull)();
    }
    else
    {
        return false;
    }
}

// *****************************************************************************

bool I2C_IsReceiveUsingInterrupts(I2C *self)
{
    if(self->interface->I2C_IsReceiveUsingInterrupts != NULL)
    {
        return (self->interface->I2C_IsReceiveUsingInterrupts)();
    }
    else
    {
        return false;
    }
}

// *****************************************************************************

void I2C_ReceiveByte(I2C *self)
{
    if(self->interface->I2C_ReceiveByte != NULL)
    {
        (self->interface->I2C_ReceiveByte)();
    }
}

// *****************************************************************************

void I2C_ReceiveByteCancel(I2C *self)
{
    if(self->interface->I2C_ReceiveByteCancel != NULL)
    {
        (self->interface->I2C_ReceiveByteCancel)();
    }
}

// *****************************************************************************

void I2C_TransmitRegisterEmptyEvent(I2C *self)
{
    if(self->interface->I2C_TransmitRegisterEmptyEvent != NULL)
    {
        (self->interface->I2C_TransmitRegisterEmptyEvent)();
    }
}

// *****************************************************************************

void I2C_TransmitByte(I2C *self, uint8_t dataToSend)
{
    if(self->interface->I2C_TransmitByte != NULL)
    {
        (self->interface->I2C_TransmitByte)(dataToSend);
    }
}

// *****************************************************************************

bool I2C_IsTransmitRegisterEmpty(I2C *self)
{
    if(self->interface->I2C_IsTransmitRegisterEmpty != NULL)
    {
        return (self->interface->I2C_IsTransmitRegisterEmpty)();
    }
    else
    {
        return false;
    }
}

// *****************************************************************************

bool I2C_IsTransmitFinished(I2C *self)
{
    if(self->interface->I2C_IsTransmitFinished != NULL)
    {
        return (self->interface->I2C_IsTransmitFinished)();
    }
    else
    {
        return false;
    }
}

// *****************************************************************************

bool I2C_IsTransmitUsingInterrupts(I2C *self)
{
    if(self->interface->I2C_IsTransmitUsingInterrupts != NULL)
    {
        return (self->interface->I2C_IsTransmitUsingInterrupts)();
    }
    else
    {
        return false;
    }
}

// *****************************************************************************

void I2C_PendingEventHandler(I2C *self)
{
    if(self->interface->I2C_PendingEventHandler != NULL)
    {
        (self->interface->I2C_PendingEventHandler)();
    }
}

// *****************************************************************************

void I2C_SetTransmitRegisterEmptyCallback(I2C *self, void (*Function)(void))
{
    if(self->interface->I2C_SetTransmitRegisterEmptyCallback != NULL)
    {
        (self->interface->I2C_SetTransmitRegisterEmptyCallback)(Function);
    }
}

// *****************************************************************************

void I2C_SetReceivedDataCallback(I2C *self, void (*Function)(uint8_t (*CallToGetData)(void)))
{
    if(self->interface->I2C_SetReceivedDataCallback != NULL)
    {
        (self->interface->I2C_SetReceivedDataCallback)(Function);
    }
}

// *****************************************************************************

void I2C_Start(I2C *self)
{
    if(self->interface->I2C_Start != NULL)
    {
        (self->interface->I2C_Start)();
    }
}

// *****************************************************************************

void I2C_Stop(I2C *self)
{
    if(self->interface->I2C_Stop != NULL)
    {
        (self->interface->I2C_Stop)();
    }
}

// *****************************************************************************

void I2C_Restart(I2C *self)
{
    if(self->interface->I2C_Restart != NULL)
    {
        (self->interface->I2C_Restart)();
    }
}

// *****************************************************************************

void I2C_SendAck(I2C *self, bool ackOrNack)
{
    if(self->interface->I2C_SendAck != NULL)
    {
        (self->interface->I2C_SendAck)(ackOrNack);
    }
}

// *****************************************************************************

bool I2C_IsBusy(I2C *self)
{
    if(self->interface->I2C_IsBusy != NULL)
    {
        return (self->interface->I2C_IsBusy)();
    }
    else
    {
        return false;
    }
}

// *****************************************************************************

I2CState I2C_GetState(I2C *self)
{
    I2CState retVal = I2C_STATE_UNKNOWN;

    if(self->interface->I2C_GetState != NULL)
    {
        retVal = (self->interface->I2C_GetState)();
    }
    
    return retVal;

}

// *****************************************************************************

bool I2C_GetAckSlaveStatus(I2C *self)
{
    if(self->interface->I2C_GetAckSlaveStatus != NULL)
    {
        return (self->interface->I2C_GetAckSlaveStatus)();
    }
    else
    {
        return false;
    }
}

// *****************************************************************************

bool I2C_GetBusError(I2C *self)
{
    if(self->interface->I2C_GetBusError != NULL)
    {
        return (self->interface->I2C_GetBusError)();
    }
    else
    {
        return false;
    }
}

/*
 End of File
 */
