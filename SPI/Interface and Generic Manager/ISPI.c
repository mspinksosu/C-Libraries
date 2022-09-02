/***************************************************************************//**
 * @brief Basic SPI Interface
 * 
 * @author Matthew Spinks
 * 
 * @date 8/28/22  Original creation
 * 
 * @file ISPI.c
 * 
 * @details
 *      TODO
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
    params->control = SPI_SS_NONE;
    params->useRxInterrupt = false;
    params->useTxInterrupt = false;
}

// *****************************************************************************

void SPI_SetInitTypeParams(SPIInitType *params, SPIRole role, SPIMode mode, 
    SPISSControl ssControl, bool useRxInterrupt, bool useTxInterrupt)
{
    params->role = role;
    params->mode = mode;
    params->control = ssControl;
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

void SPI_ReceiveEnable(SPI *self)
{
    if(self->interface->SPI_ReceiveEnable != NULL)
    {
        (self->interface->SPI_ReceiveEnable)();
    }
}

// *****************************************************************************

void SPI_ReceiveDisable(SPI *self)
{
    if(self->interface->SPI_ReceiveDisable != NULL)
    {
        (self->interface->SPI_ReceiveDisable)();
    }
}

// *****************************************************************************

void SPI_TransmitFinishedEvent(SPI *self)
{
    if(self->interface->SPI_TransmitFinishedEvent != NULL)
    {
        (self->interface->SPI_TransmitFinishedEvent)();
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

void SPI_TransmitEnable(SPI *self)
{
    if(self->interface->SPI_TransmitEnable != NULL)
    {
        (self->interface->SPI_TransmitEnable)();
    }
}

// *****************************************************************************

void SPI_TransmitDisable(SPI *self)
{
    if(self->interface->SPI_TransmitDisable != NULL)
    {
        (self->interface->SPI_TransmitDisable)();
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

void SPI_SetTransmitFinishedCallback(SPI *self, void (*Function)(void))
{
    if(self->interface->SPI_SetTransmitFinishedCallback != NULL)
    {
        (self->interface->SPI_SetTransmitFinishedCallback)(Function);
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