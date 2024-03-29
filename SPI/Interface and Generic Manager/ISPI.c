/***************************************************************************//**
 * @brief SPI Interface
 * 
 * @file ISPI.c
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 8/28/22   Original creation
 * 
 * @details
 *      // TODO details. 8-bit, MSB first.
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

#include "ISPI.h"
#include <stddef.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************


// ***** Static Function Prototypes ********************************************


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void SPI_Create(SPI *self, SPIInterface *interface)
{
    self->interface = interface;
}

// *****************************************************************************

void SPI_SetInitTypeToDefaultParams(SPIInitType *params)
{
    params->role = SPI_ROLE_MASTER;
    params->mode = SPI_MODE_0;
    params->ssControl = SPI_SS_NONE;
    params->useRxInterrupt = false;
    params->useTxInterrupt = false;
}

// *****************************************************************************

void SPI_SetInitTypeParams(SPIInitType *params, SPIRole role, SPIMode mode, 
    SPISSControl ssControl, bool useRxInterrupt, bool useTxInterrupt)
{
    params->role = role;
    params->mode = mode;
    params->ssControl = ssControl;
    params->useRxInterrupt = useRxInterrupt;
    params->useTxInterrupt = useTxInterrupt;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void SPI_Init(SPI *self, SPIInitType *params)
{
    if(self->interface->SPI_Init != NULL && params != NULL)
    {
        (self->interface->SPI_Init)(params);
    }
}

// *****************************************************************************

void SPI_Enable(SPI *self)
{
    if(self->interface->SPI_Enable != NULL)
    {
        (self->interface->SPI_Enable)();
    }
}

// *****************************************************************************

void SPI_Disable(SPI *self)
{
    if(self->interface->SPI_Disable != NULL)
    {
        (self->interface->SPI_Disable)();
    }
}

// *****************************************************************************

void SPI_ReceivedDataEvent(SPI *self)
{
    if(self->interface->SPI_ReceivedDataEvent != NULL)
    {
        (self->interface->SPI_ReceivedDataEvent)();
    }
}

// *****************************************************************************

uint8_t SPI_GetReceivedByte(SPI *self)
{   
    if(self->interface->SPI_GetReceivedByte != NULL)
    {
        return (self->interface->SPI_GetReceivedByte)();
    }
    else
    {
        return 0;
    }
}

// *****************************************************************************

bool SPI_IsReceiveRegisterFull(SPI *self)
{
    if(self->interface->SPI_IsReceiveRegisterFull != NULL)
    {
        return (self->interface->SPI_IsReceiveRegisterFull)();
    }
    else
    {
        return false;
    }
}

// *****************************************************************************

void SPI_TransmitRegisterEmptyEvent(SPI *self)
{
    if(self->interface->SPI_TransmitRegisterEmptyEvent != NULL)
    {
        (self->interface->SPI_TransmitRegisterEmptyEvent)();
    }
}

// *****************************************************************************

void SPI_TransmitByte(SPI *self, uint8_t dataToSend)
{
    if(self->interface->SPI_TransmitByte != NULL)
    {
        (self->interface->SPI_TransmitByte)(dataToSend);
    }
}

// *****************************************************************************

bool SPI_IsTransmitRegisterEmpty(SPI *self)
{
    if(self->interface->SPI_IsTransmitRegisterEmpty != NULL)
    {
        return (self->interface->SPI_IsTransmitRegisterEmpty)();
    }
    else
    {
        return false;
    }
}

// *****************************************************************************

bool SPI_IsTransmitFinished(SPI *self)
{
    if(self->interface->SPI_IsTransmitFinished != NULL)
    {
        return (self->interface->SPI_IsTransmitFinished)();
    }
    else
    {
        return false;
    }
}

// *****************************************************************************

SPIStatusBits SPI_GetStatus(SPI *self)
{
    SPIStatusBits retVal = {0};

    if(self->interface->SPI_GetStatus != NULL)
    {
        retVal = (self->interface->SPI_GetStatus)();
    }

    return retVal;
}

// *****************************************************************************

void SPI_PendingEventHandler(SPI *self)
{
    if(self->interface->SPI_PendingEventHandler != NULL)
    {
        (self->interface->SPI_PendingEventHandler)();
    }
}

// *****************************************************************************

void SPI_SetTransmitRegisterEmptyCallback(SPI *self, void (*Function)(void))
{
    if(self->interface->SPI_SetTransmitRegisterEmptyCallback != NULL)
    {
        (self->interface->SPI_SetTransmitRegisterEmptyCallback)(Function);
    }
}

// *****************************************************************************

void SPI_SetReceivedDataCallback(SPI *self, void (*Function)(uint8_t (*CallToGetData)(void)))
{
    if(self->interface->SPI_SetReceivedDataCallback != NULL)
    {
        (self->interface->SPI_SetReceivedDataCallback)(Function);
    }
}

// *****************************************************************************

void SPI_SetSSPinFunc(SPI *self, void (*Function)(bool))
{
    if(self->interface->SPI_SetSSPinFunc != NULL)
    {
        (self->interface->SPI_SetSSPinFunc)(Function);
    }
}

/*
 End of File
 */