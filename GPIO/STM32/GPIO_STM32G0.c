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
    .GPIO_InitPin = (void (*)(GPIOPin, void *))GPIO_STM32_InitPin,
};

GPIO_TypeDef *GET_PORT[16] = {
    GPIOA, GPIOB, GPIOC, GPIOD,
    NULL, GPIOF, NULL, NULL,
    NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL};

static GPIO_TypeDef *st_port = NULL;
static uint32_t pinNumber;

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

void GPIO_STM32_InitPin(GPIOPin pin, GPIOInitType_STM32 *params)
{
    st_port = GET_PORT[(pin >> 4)];
    pinNumber = pin & 0x0F;

    if(st_port == NULL)
        return;

    /* Clear output type bits. Default is push-pull */
    st_port->OTYPER &= ~(1UL << (pinNumber));

    /* Clear pull up bits for this pin. Each pin has two bits */
    st_port->PUPDR &= ~(3UL << (pinNumber * 2));

    if(params->super->pull == GPIO_PULL_UP)
    {
        st_port->PUPDR |= 1UL << (pinNumber * 2);
    }
    else if(params->super->pull == GPIO_PULL_DOWN)
    {
        st_port->PUPDR |= 2UL << (pinNumber * 2);
    }

    /* Set the mode of the pins. Start by clearing alternate mode bits for this 
    pin. Each pin has two bits. This will set the pin as a digital input by 
    default */
    st_port->MODER &= ~(3UL << (pinNumber * 2));

    if(params->alternate != 0)
    {
        /* Set pin mode to alternate */
        st_port->MODER |= 2UL << (pinNumber * 2);
        
        /* Convert the alternate value for that pin into a mask */
        params->alternate = ((params->alternate) << (pinNumber));

        /* Now set the actual alternate value for the pin */
        if(pinNumber < 8)
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
        st_port->MODER |= 3UL << (pinNumber * 2);
    }
    else if(params->super->type == GPIO_TYPE_DIGITAL_OUTPUT)
    {
        st_port->MODER |= 1UL << (pinNumber * 2);
    }
    else if(params->super->type == GPIO_TYPE_OPEN_DRAIN)
    {
        st_port->OTYPER |= 1UL << (pinNumber);
        st_port->MODER |= 1UL << (pinNumber * 2);
    }
    
    /* Clear the speed bits for this pin. Each pin has two bits */
    st_port->OSPEEDR &= ~(3UL << (pinNumber * 2));

    if(params->speed != 0)
    {
        /* Convert the speed value for that pin into a mask, then set it */
        params->speed = ((params->speed) << (pinNumber));
        st_port->OSPEEDR |= params->speed;
    }
}

// *****************************************************************************

void GPIO_SetPin(GPIOPin pin)
{
    st_port = GET_PORT[(pin >> 4)];
    pinNumber = pin & 0x0F;
    /* For the BSRR register, the lower 16 bits set the pins. The upper 16 bits
    reset the pins. Zero writes are ignored completely */
    if(st_port != NULL)
        st_port->BSRR = (1UL << (pinNumber));
}

// *****************************************************************************

void GPIO_ClearPin(GPIOPin pin)
{
    st_port = GET_PORT[(pin >> 4)];
    pinNumber = pin & 0x0F;
    /* The lower 16 bits of the BRR register reset the pins. Zero writes are
    ignored completely */
    if(st_port != NULL)
        st_port->BRR = (1UL << (pinNumber));
}

// *****************************************************************************

void GPIO_InvertPin(GPIOPin pin)
{
    st_port = GET_PORT[(pin >> 4)];
    pinNumber = pin & 0x0F;
    /* The BSRR register cannot be read. We have to read ODR to get the current
    status of the pin instead */
    uint32_t pinMask = 1UL << (pinNumber);

    if(st_port != NULL)
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

void GPIO_WritePin(GPIOPin pin, bool setPinHigh)
{
    if(setPinHigh)
        GPIO_SetPin(pin);
    else
        GPIO_ClearPin(pin);
}

// *****************************************************************************

bool GPIO_ReadPin(GPIOPin pin)
{
    st_port = GET_PORT[(pin >> 4)];
    pinNumber = pin & 0x0F;
    uint16_t retValue = 0;
    uint32_t modeMask;

    if(st_port != NULL)
    {
        modeMask = st_port->MODER & (3UL << (pinNumber * 2));
        modeMask >>= (pinNumber * 2);

        if(modeMask == 0)
        {
            // digital input
            if(st_port->IDR & (1UL << pinNumber))
                retValue = 1;
        }
    }
    return retValue;
}

// *****************************************************************************

void GPIO_SetType(GPIOPin pin, GPIOType type)
{
    st_port = GET_PORT[(pin >> 4)];
    pinNumber = pin & 0x0F;

    if(st_port == NULL)
        return;

    switch(type)
    {
        case GPIO_TYPE_ANALOG:
            st_port->MODER |= (3UL << (pinNumber * 2));
            break;
        case GPIO_TYPE_DIGITAL_INPUT:
            st_port->MODER &= ~(3UL << (pinNumber * 2));
            break;
        case GPIO_TYPE_DIGITAL_OUTPUT:
            st_port->OTYPER &= ~(1UL << (pinNumber));
            st_port->MODER &= ~(3UL << (pinNumber * 2));
            st_port->MODER |= (1UL << (pinNumber * 2));
            break;
        case GPIO_TYPE_OPEN_DRAIN:
            st_port->OTYPER |= (1UL << (pinNumber));
            st_port->MODER &= ~(3UL << (pinNumber * 2));
            st_port->MODER |= (1UL << (pinNumber * 2));
            break;
        default:
            // analog
            st_port->MODER |= (3UL << (pinNumber * 2));
            break;
    }
}

// *****************************************************************************

GPIOType GPIO_GetType(GPIOPin pin)
{
    st_port = GET_PORT[(pin >> 4)];
    pinNumber = pin & 0x0F;
    GPIOType retVal = GPIO_TYPE_ANALOG; // reset value
    uint32_t modeMask;

    if(st_port != NULL)
    {
        modeMask = st_port->MODER & (3UL << (pinNumber * 2));
        modeMask >>= (pinNumber * 2);

       if(modeMask == 0)
       {
            retVal = GPIO_TYPE_DIGITAL_INPUT;
       }
       else if(modeMask == 1)
       {
            if(st_port->OTYPER & (1UL << (pinNumber)))
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

void GPIO_SetPull(GPIOPin pin, GPIOPull pullType)
{
    st_port = GET_PORT[(pin >> 4)];
    pinNumber = pin & 0x0F;

    if(st_port == NULL)
        return;

    st_port->PUPDR &= ~(3UL << (pinNumber * 2));

    if(pullType == GPIO_PULL_UP)
        st_port->PUPDR |= (1UL << (pinNumber * 2));
    else if(pullType == GPIO_PULL_DOWN)
        st_port->PUPDR |= (2UL << (pinNumber * 2));
}

// *****************************************************************************

GPIOPull GPIO_GetPull(GPIOPin pin)
{
    st_port = GET_PORT[(pin >> 4)];
    pinNumber = pin & 0x0F;
    GPIOPull retVal = GPIO_PULL_NONE;
    uint32_t pullMask;

    if(st_port != NULL)
    {
        pullMask = st_port->MODER & (3UL << (pinNumber * 2));
        pullMask >>= (pinNumber * 2);

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

/*
 End of File
*/