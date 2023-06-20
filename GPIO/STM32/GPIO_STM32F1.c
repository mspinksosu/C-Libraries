/***************************************************************************//**
 * @brief GPIO Library Implementation (STM32F1)
 * 
 * @file GPIO_STM32G0.c
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 2/4/22    Original creation
 * @date 2/19/22   Modified to use array for initialization
 * 
 * @details
 *      A GPIO library that implements the STM32 GPIO functions which in turn, 
 * conform to the IGPIO interface. 
 * 
 * The GPIOInterface or function table tells the interface what functions to 
 * call. We declare and define the function table in one step, setting its 
 * members (which are function pointers) the our local functions. Typecasting 
 * is necessary.
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

#include "GPIO_STM32F1.h"
#include <stddef.h> // needed for NULL

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/* Create the function table */
GPIOInterface GPIO_FunctionTable = {
    .GPIO_InitPin = (void (*)(GPIOPort, uint8_t, void *))GPIO_STM32_InitPin,
};

GPIO_TypeDef *GET_PORT[16] = {
    GPIOA, GPIOB, GPIOC, GPIOD,
    NULL, GPIOF, NULL, NULL,
    NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL};

static GPIO_TypeDef *stmPort = NULL;

// ***** Static Function Prototypes ********************************************

/* Put static function prototypes here */


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void GPIO_STM32_CreateInitType(GPIOInitType_STM32 *stm32Params, GPIOInitType *base)
{
    stm32Params->super = base;
    GPIO_CreateInitType(base, stm32Params);
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void GPIO_STM32_InitPin(GPIOPort port, uint8_t pin, GPIOInitType_STM32 *params)
{
    stmPort = GET_PORT[port];
    if(stmPort == NULL || pin > 15)
        return;

    /* Port configuration settings MODEx and CNFx are together with each of the 
    two 32-bit registers CRL and CRH covering 8 pins. */
    uint32_t configBit = pin, *configReg = (uint32_t *)&(stmPort->CRL);
    if(pin > 7)
    {
        configBit -= 8;
        configReg++; // use CRH register
    }

    /* Set the mode of the pins. Start by clearing configuration and resetting
    the mode bits for this pin. Each pin has four bits. This will set the pin 
    as an digital input by default */
    *configReg &= ~(15UL << (configBit * 4));
    *configReg |= (4UL << (configBit * 4)); // floating

    if(params->alternate == GPIO_STM32F1_AF_PP || 
        params->alternate == GPIO_STM32F1_AF_OD)
    {
        /* Set the alternate config bits and set pin to output */
        *configReg &= ~(4UL << (configBit * 4));
        *configReg |= params->alternate << (configBit * 4);
        /* Set the speed if there is one. Otherwise, just default to max */
        if(params->speed > 0 && params->speed > 4)
        {
            *configReg &= ~(3UL << (configBit * 4));
            *configReg |= params->speed << (configBit * 4);
        }
        else
        {
            *configReg |= (3UL << (configBit * 4)); // max speed if unspecified
        }
    }
    else if(params->super->type == GPIO_TYPE_ANALOG)
    {
        *configReg &= ~(4UL << (configBit * 4));
    }
    else if(params->super->type == GPIO_TYPE_DIGITAL_OUTPUT)
    {
        *configReg &= ~(4UL << (configBit * 4)); // push pull
        /* Set the speed if there is one. Otherwise, just default to max */
        if(params->speed > 0 && params->speed > 4)
        {
            *configReg &= ~(3UL << (configBit * 4));
            *configReg |= params->speed << (configBit * 4);
        }
        else
        {
            *configReg |= (3UL << (configBit * 4)); // max speed if unspecified
        }
    }
    else if(params->super->type == GPIO_TYPE_OPEN_DRAIN)
    {
        // no change to CNF
        /* Set the speed if there is one. Otherwise, just default to max */
        if(params->speed > 0 && params->speed > 4)
        {
            *configReg &= ~(3UL << (configBit * 4));
            *configReg |= params->speed << (configBit * 4);
        }
        else
        {
            *configReg |= (3UL << (configBit * 4)); // max speed if unspecified
        }
    }
    else if(params->super->pull == GPIO_PULL_UP)
    {
        *configReg |= (8UL << (configBit * 4));
        *configReg &= ~(4UL << (configBit * 4));
        stmPort->ODR |= (1UL << pin);
    }
    else if(params->super->pull == GPIO_PULL_DOWN)
    {
        *configReg |= (8UL << (configBit * 4));
        *configReg &= ~(4UL << (configBit * 4));
        stmPort->ODR &= ~(1UL << pin);
    }

    /* Set the speed if needed */
}

// *****************************************************************************

void GPIO_SetPin(GPIOPort port, uint8_t pin)
{
    stmPort = GET_PORT[port];
    /* For the BSRR register, the lower 16 bits set the pins. The upper 16 bits
    reset the pins. Zero writes are ignored completely */
    if(stmPort != NULL && pin <= 15)
        stmPort->BSRR = (1UL << (pin));
}

// *****************************************************************************

void GPIO_ClearPin(GPIOPort port, uint8_t pin)
{
    stmPort = GET_PORT[port];
    /* The lower 16 bits of the BRR register reset the pins. Zero writes are
    ignored completely */
    if(stmPort != NULL && pin <= 15)
        stmPort->BRR = (1UL << (pin));
}

// *****************************************************************************

void GPIO_InvertPin(GPIOPort port, uint8_t pin)
{
    stmPort = GET_PORT[port];
    /* The BSRR register cannot be read. We have to read ODR to get the current
    status of the pin instead */
    uint32_t pinMask = 1UL << (pin);

    if(stmPort != NULL && pin <= 15)
    {
        uint32_t pinState = stmPort->ODR & pinMask;

        /* The upper 16 bits reset the pin. The lower 16 bits set the pin.
        Zero writes are ignored completely. */
        if(pinState)
        {
            stmPort->BRR = pinMask << 16;
        }
        else
        {
            stmPort->BRR = pinMask;
        }
    }
}

// *****************************************************************************

void GPIO_WritePin(GPIOPort port, uint8_t pin, bool setPinHigh)
{
    if(setPinHigh)
        GPIO_SetPin(port, pin);
    else
        GPIO_ClearPin(port, pin);
}

// *****************************************************************************

bool GPIO_ReadPin(GPIOPort port, uint8_t pin)
{
    stmPort = GET_PORT[port];
    bool retValue = false;
    uint32_t configBit = pin, *configReg = (uint32_t *)&(stmPort->CRL);

    if(stmPort != NULL && pin <= 15)
    {
        if(pin > 7)
        {
            configBit -= 8;
            configReg++; // use CRH register
        }

        if(!(*configReg & (3UL << (configBit * 4))))
        {
            // digital input
            if(stmPort->IDR & (1UL << pin))
                retValue = true;
        }
    }
    return retValue;
}

// *****************************************************************************

void GPIO_SetType(GPIOPort port, uint8_t pin, GPIOType type)
{
    stmPort = GET_PORT[port];
    if(stmPort == NULL || pin > 15)
        return;

    /* Port configuration settings MODEx and CNFx are together with each of the 
    two 32-bit registers CRL and CRH covering 8 pins. */
    uint32_t configBit = pin, *configReg = (uint32_t *)&(stmPort->CRL);
    if(pin > 7)
    {
        configBit -= 8;
        configReg++; // use CRH register
    }

    switch(type)
    {
        case GPIO_TYPE_DIGITAL_INPUT:
            *configReg &= ~(15UL << (configBit * 4));
            *configReg |= (4UL << (configBit * 4)); // floating
            break;
        case GPIO_TYPE_DIGITAL_OUTPUT:
            if(!(*configReg & 3UL << (configBit * 4))) // if input
                *configReg |= (3UL << (configBit * 4)); // output, max speed
            *configReg &= ~(8UL << (configBit * 4));
            *configReg &= ~(4UL << (configBit * 4));
            break;
        case GPIO_TYPE_OPEN_DRAIN:
            if(!(*configReg & 3UL << (configBit * 4))) // if input
                *configReg |= (3UL << (configBit * 4)); // output, max speed
            *configReg &= ~(8UL << (configBit * 4));
            *configReg |= (4UL << (configBit * 4));
            break;
        default:
            // GPIO_TYPE_ANALOG
            *configReg &= ~(15UL << (configBit * 4));
            break;
    }
}

// *****************************************************************************

GPIOType GPIO_GetType(GPIOPort port, uint8_t pin)
{
    stmPort = GET_PORT[port];
    GPIOType retVal = GPIO_TYPE_ANALOG;
    uint32_t configBit = pin, *configReg = (uint32_t *)&(stmPort->CRL);

    if(stmPort != NULL && pin <= 15)
    {
        if(pin > 7)
        {
            configBit -= 8;
            configReg++; // use CRH register
        }
        uint32_t modeMask = *configReg & (15UL << (configBit * 4));
        modeMask >>= (configBit * 4);

        if(modeMask == 0)
        {
            retVal = GPIO_TYPE_ANALOG;
        }
        else if(modeMask == 0x04 || modeMask == 0x08)
        {
            retVal = GPIO_TYPE_DIGITAL_INPUT;
        }
        else if(modeMask & 0x04)
        {
            retVal = GPIO_TYPE_OPEN_DRAIN;
        }
        else
        {
            retVal = GPIO_TYPE_DIGITAL_OUTPUT;
        }
    }
    return retVal;
}

// *****************************************************************************

void GPIO_SetPull(GPIOPort port, uint8_t pin, GPIOPull pullType)
{
    stmPort = GET_PORT[port];
    if(stmPort == NULL || pin > 15)
        return;

    /* Port configuration settings MODEx and CNFx are together with each of the 
    two 32-bit registers CRL and CRH covering 8 pins. */
    uint32_t configBit = pin, *configReg = (uint32_t *)&(stmPort->CRL);
    if(pin > 7)
    {
        configBit -= 8;
        configReg++; // use CRH register
    }

    uint32_t modeMask = *configReg & (15UL << (configBit * 4));
    modeMask >>= (configBit * 4);

    if(modeMask == 0x08) // if input with pull-up / pull-down
    {
        if(pullType == GPIO_PULL_UP)
            stmPort->ODR |= (1UL << pin);
        else if(pullType == GPIO_PULL_DOWN)
            stmPort->ODR &= ~(1UL << pin);
    }
}

// *****************************************************************************

GPIOPull GPIO_GetPull(GPIOPort port, uint8_t pin)
{
    stmPort = GET_PORT[port];
    GPIOPull retVal = GPIO_PULL_NONE;
    uint32_t configBit = pin, *configReg = (uint32_t *)&(stmPort->CRL);

    if(stmPort != NULL && pin <= 15)
    {
        if(pin > 7)
        {
            configBit -= 8;
            configReg++; // use CRH register
        }
        uint32_t pullMask = *configReg & (3UL << (configBit * 4));
        pullMask >>= (pin * 4);

        if(pullMask == 1)
        {
            retVal = GPIO_PULL_UP;
        }
        else if(pullMask == 2)
        {
            retVal = GPIO_PULL_DOWN;
        }
    }
    return retVal;
}

// *****************************************************************************

void GPIO_PortWrite(GPIOPort port, uint16_t portMask)
{
    // TODO
}

// *****************************************************************************

uint16_t GPIO_PortGetOutput(GPIOPort port)
{
    // TODO
    return 0;
}

// *****************************************************************************

uint16_t GPIO_PortRead(GPIOPort port)
{
    // TODO
    return 0;
}

/*
 End of File
*/