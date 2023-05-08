/***************************************************************************//**
 * @brief SPI Library Implementation (STM32F1)
 * 
 * @author Matthew Spinks
 * 
 * @date 12/2/14  Original creation
 * @date 2/4/22   Modified
 * 
 * @file SPI1_STM32F1.c
 * 
 * @details
 *      // TODO details
 * 
 ******************************************************************************/

#include "SPI1.h"
#include <stddef.h> // needed for NULL

/* Include processor specific header files here */
#include "stm32f10x_map.h"

// ***** Defines ***************************************************************

/* Peripheral addresses and registers */
#define SPI_ADDR        SPI1
#define SPI_CLK_REG     RCC->APB2ENR
#define SPI_CLK_EN_MSK  RCC_APB2ENR_SPI1EN

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

static bool useRxInterrupt = false, useTxInterrupt = false;
SPIRole role = SPI_ROLE_MASTER;
SPIMode mode = SPI_MODE_0;
SPISSControl ssControl = SPI_SS_NONE;

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
    role = params->role;
    mode = params->mode;
    ssControl = params->ssControl;

    /* Peripheral clock must be enabled before you can write any registers */
    SPI_CLK_REG |= SPI_CLK_EN_MSK;

    /* Turn off module before making changes */
    SPI_ADDR->CR1 &= ~SPI_CR1_SPE;

    /* Turn off tx/rx interrupts */
    SPI_ADDR->CR1 &= ~(SPI_CR2_RXNEIE | SPI_CR2_TXEIE);

    if(role == SPI_ROLE_MASTER)
    {
        SPI_ADDR->CR1 |= SPI_CR1_MSTR;
    }
    else
    {
        SPI_ADDR->CR1 &= ~SPI_CR1_MSTR;
    }

    switch(mode)
    {
        case SPI_MODE_1:
            SPI_ADDR->CR1 &= ~(SPI_CR1_CPOL);
            SPI_ADDR->CR1 |= SPI_CR1_CPHA;
            break;
        case SPI_MODE_2:
            SPI_ADDR->CR1 |= SPI_CR1_CPOL;
            SPI_ADDR->CR1 &= ~(SPI_CR1_CPHA);
            break;
        case SPI_MODE_3:
            SPI_ADDR->CR1 |= SPI_CR1_CPOL;
            SPI_ADDR->CR1 |= SPI_CR1_CPHA;
            break;
        default:
            SPI_ADDR->CR1 &= ~(SPI_CR1_CPOL | SPI_CR1_CPHA);
            break;
    }

    if(ssControl == SPI_SS_HARDWARE)
    {
        SPI_ADDR->CR2 |= SPI_CR2_SSOE;
    }
    else
    {
        SPI_ADDR->CR2 &= ~SPI_CR2_SSOE;
    }

    /* Set prescale value */
    SPI_ADDR->CR1 &= ~SPI_CR1_BR; // default 000 (pclk / 2)

    /* If you turn on the transmit interrupt during initialization, it could
    fire off repeatedly. It's best to turn it on after placing data in the 
    transmit register */

    if(useRxInterrupt)
        SPI_ADDR->CR2 |= SPI_CR2_RXNEIE;

    SPI_ADDR->CR1 |= SPI_CR1_SPE; // enable SPI
}

// *****************************************************************************

void SPI1_Enable(void)
{
    SPI_ADDR->CR1 |= SPI_CR1_SPE;

    if(useRxInterrupt)
        SPI_ADDR->CR2 |= SPI_CR2_RXNEIE;

    /* Set slave select high */
    if(ssControl == SPI_SS_CALLBACKS && SetSSPin != NULL)
    {
        SetSSPin(true);
    }
}

// *****************************************************************************

void SPI1_Disable(void)
{
    SPI_ADDR->CR1 &= ~SPI_CR1_SPE;

    // TODO wait for transmission to finish

    /* Set slave select high */
    if(ssControl == SPI_SS_CALLBACKS && SetSSPin != NULL)
    {
        SetSSPin(true);
    }
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