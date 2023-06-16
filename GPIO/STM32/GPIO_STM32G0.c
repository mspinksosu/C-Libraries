/***************************************************************************//**
 * @brief GPIO Library Implementation (STM32G0)
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

#include "GPIO_STM32G0.h"
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

static GPIO_TypeDef *st_port = NULL;
static uint32_t pin;

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
    st_port = GET_PORT[port];

    if(st_port == NULL || pin > 15)
        return;

    /* Clear output type bits. Default is push-pull */
    st_port->OTYPER &= ~(1UL << (pin));

    /* Clear pull up bits for this pin. Each pin has two bits */
    st_port->PUPDR &= ~(3UL << (pin * 2));

    if(params->super->pull == GPIO_PULL_UP)
    {
        st_port->PUPDR |= 1UL << (pin * 2);
    }
    else if(params->super->pull == GPIO_PULL_DOWN)
    {
        st_port->PUPDR |= 2UL << (pin * 2);
    }

    /* Set the mode of the pins. Start by clearing alternate mode bits for this 
    pin. Each pin has two bits. This will set the pin as a digital input by 
    default */
    st_port->MODER &= ~(3UL << (pin * 2));

    if(params->alternate != 0)
    {
        /* Set pin mode to alternate */
        st_port->MODER |= 2UL << (pin * 2);
        
        /* Convert the alternate value for that pin into a mask */
        params->alternate = ((params->alternate) << (pin));

        /* Now set the actual alternate value for the pin */
        if(pin < 8)
        {
            // GPIOx_AFRL
            st_port->AFR[0] |= params->alternate;
        }
        else
        {
            // GPIOx_AFRH
            st_port->AFR[1] |= params->alternate;
        }
    }
    else if(params->super->type == GPIO_TYPE_ANALOG)
    {
        st_port->MODER |= 3UL << (pin * 2);
    }
    else if(params->super->type == GPIO_TYPE_DIGITAL_OUTPUT)
    {
        st_port->MODER |= 1UL << (pin * 2);
    }
    else if(params->super->type == GPIO_TYPE_OPEN_DRAIN)
    {
        st_port->OTYPER |= 1UL << (pin);
        st_port->MODER |= 1UL << (pin * 2);
    }
    
    /* Clear the speed bits for this pin. Each pin has two bits */
    st_port->OSPEEDR &= ~(3UL << (pin * 2));

    if(params->speed != 0)
    {
        /* Convert the speed value for that pin into a mask, then set it */
        params->speed = ((params->speed) << (pin));
        st_port->OSPEEDR |= params->speed;
    }
}

// *****************************************************************************

void GPIO_SetPin(GPIOPort port, uint8_t pin)
{
    st_port = GET_PORT[port];
    /* For the BSRR register, the lower 16 bits set the pins. The upper 16 bits
    reset the pins. Zero writes are ignored completely */
    if(st_port != NULL && pin <= 15)
        st_port->BSRR = (1UL << (pin));
}

// *****************************************************************************

void GPIO_ClearPin(GPIOPort port, uint8_t pin)
{
    st_port = GET_PORT[port];
    /* The lower 16 bits of the BRR register reset the pins. Zero writes are
    ignored completely */
    if(st_port != NULL && pin <= 15)
        st_port->BRR = (1UL << (pin));
}

// *****************************************************************************

void GPIO_InvertPin(GPIOPort port, uint8_t pin)
{
    st_port = GET_PORT[port];
    /* The BSRR register cannot be read. We have to read ODR to get the current
    status of the pin instead */
    uint32_t pinMask = 1UL << (pin);

    if(st_port != NULL && pin <= 15)
    {
        uint32_t pinState = st_port->ODR & pinMask;

        /* The upper 16 bits reset the pin. The lower 16 bits set the pin.
        Zero writes are ignored completely. */
        if(pinState)
        {
            st_port->BRR = pinMask << 16;
        }
        else
        {
            st_port->BRR = pinMask;
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
    st_port = GET_PORT[port];
    bool retValue = false;

    if(st_port != NULL && pin <= 15)
    {
        if(!(st_port->MODER & (3UL << (pin * 2))))
        {
            // digital input
            if(st_port->IDR & (1UL << pin))
                retValue = true;
        }
    }
    return retValue;
}

// *****************************************************************************

void GPIO_SetType(GPIOPort port, uint8_t pin, GPIOType type)
{
    st_port = GET_PORT[port];
    

    if(st_port == NULL || pin > 15)
        return;

    switch(type)
    {
        case GPIO_TYPE_ANALOG:
            st_port->MODER |= (3UL << (pin * 2));
            break;
        case GPIO_TYPE_DIGITAL_INPUT:
            st_port->MODER &= ~(3UL << (pin * 2));
            break;
        case GPIO_TYPE_DIGITAL_OUTPUT:
            st_port->OTYPER &= ~(1UL << (pin));
            st_port->MODER &= ~(3UL << (pin * 2));
            st_port->MODER |= (1UL << (pin * 2));
            break;
        case GPIO_TYPE_OPEN_DRAIN:
            st_port->OTYPER |= (1UL << (pin));
            st_port->MODER &= ~(3UL << (pin * 2));
            st_port->MODER |= (1UL << (pin * 2));
            break;
        default:
            // analog
            st_port->MODER |= (3UL << (pin * 2));
            break;
    }
}

// *****************************************************************************

GPIOType GPIO_GetType(GPIOPort port, uint8_t pin)
{
    st_port = GET_PORT[port];
    
    GPIOType retVal = GPIO_TYPE_ANALOG; // reset value
    uint32_t modeMask;

    if(st_port != NULL && pin <= 15)
    {
        modeMask = st_port->MODER & (3UL << (pin * 2));
        modeMask >>= (pin * 2);

       if(modeMask == 0)
       {
            retVal = GPIO_TYPE_DIGITAL_INPUT;
       }
       else if(modeMask == 1)
       {
            if(st_port->OTYPER & (1UL << (pin)))
            {
                retVal = GPIO_TYPE_OPEN_DRAIN;
            }
            else
            {
                retVal = GPIO_TYPE_DIGITAL_OUTPUT;
            }
       }
    }
    return retVal;
}

// *****************************************************************************

void GPIO_SetPull(GPIOPort port, uint8_t pin, GPIOPull pullType)
{
    st_port = GET_PORT[port];
    

    if(st_port == NULL || pin > 15)
        return;

    st_port->PUPDR &= ~(3UL << (pin * 2));

    if(pullType == GPIO_PULL_UP)
        st_port->PUPDR |= (1UL << (pin * 2));
    else if(pullType == GPIO_PULL_DOWN)
        st_port->PUPDR |= (2UL << (pin * 2));
}

// *****************************************************************************

GPIOPull GPIO_GetPull(GPIOPort port, uint8_t pin)
{
    st_port = GET_PORT[port];
    
    GPIOPull retVal = GPIO_PULL_NONE;
    uint32_t pullMask;

    if(st_port != NULL && pin <= 15)
    {
        pullMask = st_port->MODER & (3UL << (pin * 2));
        pullMask >>= (pin * 2);

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
}

// *****************************************************************************

uint16_t GPIO_PortRead(GPIOPort port)
{
    // TODO
}

/*
 End of File
*/