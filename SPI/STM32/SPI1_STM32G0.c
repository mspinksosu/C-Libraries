/***************************************************************************//**
 * @brief SPI Library Implementation (STM32G0)
 * 
 * @author Matthew Spinks
 * 
 * @date 12/2/14  Original creation
 * @date 2/4/22   Modified
 * 
 * @file SPI1_STM32G0.c
 * 
 * @details
 *      // TODO details
 * 
 ******************************************************************************/

#include "SPI1.h"
#include <stddef.h> // needed for NULL

/* Include processor specific header files here */
#include "stm32g071xx.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/* Assign functions to the interface */
SPIInterface SPI1_FunctionTable = {
    .SPI_Init = SPI1_Init,
    .SPI_Enable = SPI1_Enable,
    .SPI_Disable = SPI1_Disable,
    .SPI_ReceivedDataEvent = SPI1_ReceivedDataEvent,
    .SPI_GetReceivedByte = SPI1_GetReceivedByte,
    .SPI_IsReceiveRegisterFull = SPI1_IsReceiveRegisterFull,
    .SPI_TransmitFinishedEvent = SPI1_TransmitFinishedEvent,
    .SPI_TransmitByte = SPI1_TransmitByte,
    .SPI_IsTransmitRegisterEmpty = SPI1_IsTransmitRegisterEmpty,
    .SPI_GetStatus = SPI1_GetStatus,
    .SPI_PendingEventHandler = SPI1_PendingEventHandler,
    .SPI_SetTransmitFinishedCallback = SPI1_SetTransmitFinishedCallback,
    .SPI_SetReceivedDataCallback = SPI1_SetReceivedDataCallback,
    .SPI_SetSSPinFunc = SPI1_SetSSPinFunc,
};

// local function pointers
static void (*TransmitFinishedCallback)(void);
static void (*ReceivedDataCallback)(uint8_t data);
static void (*SetSSPin)(bool setHigh);

// ***** Static Function Prototypes ********************************************

/* Put static function prototypes here */


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void SPI1_Init(SPIInitType *params)
{

}

// *****************************************************************************

void SPI1_Enable(void)
{

}

// *****************************************************************************

void SPI1_Disable(void)
{

}

// *****************************************************************************

void SPI1_ReceivedDataEvent(void)
{

}

// *****************************************************************************

uint8_t SPI1_GetReceivedByte(void)
{

}

// *****************************************************************************

bool SPI1_IsReceiveRegisterFull(void)
{

}

// *****************************************************************************

void SPI1_TransmitFinishedEvent(void)
{

}

// *****************************************************************************

void SPI1_TransmitByte(uint8_t dataToSend)
{

}

// *****************************************************************************

bool SPI1_IsTransmitRegisterEmpty(void)
{

}

// *****************************************************************************

SPIStatusBits SPI1_GetStatus(void)
{

}

// *****************************************************************************

void SPI1_PendingEventHandler(void)
{

}

// *****************************************************************************

void SPI1_SetTransmitFinishedCallback(void (*Function)(void))
{
    TransmitFinishedCallback = Function;
}

// *****************************************************************************

void SPI1_SetReceivedDataCallback(void (*Function)(uint8_t data))
{
    ReceivedDataCallback = Function;
}

// *****************************************************************************

void SPI1_SetSSPinFunc(void (*Function)(bool setPinHigh))
{
    SetSSPin = Function;
}

/*
 End of File
 */