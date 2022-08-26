/***************************************************************************//**
 * @brief GPIO Library Implementation (STM32G0)
 * 
 * @author Matthew Spinks
 * 
 * @date 2/4/22   Original creation
 * @date 2/19/22  Modified to use array for initialization
 * 
 * @file GPIO_STM32G0.c
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
 ******************************************************************************/

#include <stddef.h> // needed for NULL
#include "GPIO_STM32G0.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/* Create the function table */
GPIOInterface GPIOFunctionTable = {
    .GPIO_InitPin = (void (*)(void *, void *))GPIO_STM32_InitPin,
    .GPIO_SetPin = (void (*)(void *))GPIO_STM32_SetPin,
    .GPIO_ClearPin = (void (*)(void *))GPIO_STM32_ClearPin,
    .GPIO_InvertPin = (void (*)(void *))GPIO_STM32_InvertPin,
    .GPIO_WritePin = (void (*)(void *, bool))GPIO_STM32_WritePin,
    .GPIO_ReadPin = (uint16_t (*)(void *))GPIO_STM32_ReadPin,
    .GPIO_ReadBool = (bool (*)(void *))GPIO_STM32_ReadBool,
    .GPIO_SetType = (void (*)(void *, GPIOType))GPIO_STM32_SetType,
    .GPIO_GetType = (GPIOType (*)(void *))GPIO_STM32_GetType,
    .GPIO_SetPull = (void (*)(void *, GPIOPull))GPIO_STM32_SetPull,
    .GPIO_GetPull = (GPIOPull (*)(void *))GPIO_STM32_GetPull,
    // Add the rest of the functions listed in the interface
};

// ***** Static Function Prototypes ********************************************

/* Put static function prototypes here */


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void GPIO_STM32_Create(GPIO_STM32 *stm32Pin, GPIO *base)
{
    stm32Pin->super = base;
    GPIO_Create(base, stm32Pin);
}

// *****************************************************************************

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

void GPIO_STM32_InitPin(GPIO_STM32 *self, GPIOInitType_STM32 *params)
{
    if(self->st_port == NULL)
        return;

    /* Clear output type bits. Default is push-pull */
    self->st_port->OTYPER &= ~(1UL << (self->super->pinNumber));

    /* Clear pull up bits for this pin. Each pin has two bits */
    self->st_port->PUPDR &= ~(3UL << (self->super->pinNumber * 2));

    if(params->super->pull == GPIO_PULL_UP)
    {
        self->st_port->PUPDR |= 1UL << (self->super->pinNumber * 2);
    }
    else if(params->super->pull == GPIO_PULL_DOWN)
    {
        self->st_port->PUPDR |= 2UL << (self->super->pinNumber * 2);
    }

    /* Set the mode of the pins. Start by clearing alternate mode bits for this 
    pin. Each pin has two bits. This will set the pin as a digital input by 
    default */
    self->st_port->MODER &= ~(3UL << (self->super->pinNumber * 2));

    if(params->alternate != 0)
    {
        /* Set pin mode to alternate */
        self->st_port->MODER |= 2UL << (self->super->pinNumber * 2);
        
        /* Convert the alternate value for that pin into a mask */
        params->alternate = ((params->alternate) << (self->super->pinNumber));

        /* Now set the actual alternate value for the pin */
        if(self->super->pinNumber < 8)
        {
            // GPIOx_AFRL
            self->st_port->AF[0] |= params->alternate;
        }
        else
        {
            // GPIOx_AFRH
            self->st_port->AF[1] |= params->alternate;
        }
    }
    else if(params->super->type == GPIO_TYPE_ANALOG)
    {
        self->st_port->MODER |= 3UL << (self->super->pinNumber * 2);
    }
    else if(params->super->type == GPIO_TYPE_DIGITAL_OUTPUT)
    {
        self->st_port->MODER |= 1UL << (self->super->pinNumber * 2);
    }
    else if(params->super->type == GPIO_TYPE_OPEN_DRAIN)
    {
        self->st_port->OTYPER |= 1UL << (self->super->pinNumber);
        self->st_port->MODER |= 1UL << (self->super->pinNumber * 2);
    }
    
    /* Clear the speed bits for this pin. Each pin has two bits */
    self->st_port->OSPEEDR &= ~(3UL << (self->super->pinNumber * 2));

    if(params->speed != 0)
    {
        /* Convert the speed value for that pin into a mask, then set it */
        params->speed = ((params->speed) << (self->super->pinNumber));
        self->st_port->OSPEEDR |= params->speed;
    }
}

// *****************************************************************************

void GPIO_STM32_SetPin(GPIO_STM32 *self)
{
    /* For the BSRR register, the lower 16 bits set the pins. The upper 16 bits
    reset the pins. Zero writes are ignored completely */
    if(self->st_port != NULL && self->super->pinNumber < 16)
        self->st_port->BSRR = (1UL << (self->super->pinNumber));
}

// *****************************************************************************

void GPIO_STM32_ClearPin(GPIO_STM32 *self)
{
    /* The lower 16 bits of the BRR register reset the pins. Zero writes are
    ignored completely */
    if(self->st_port != NULL)
        self->st_port->BRR = (1UL << (self->super->pinNumber));
}

// *****************************************************************************

void GPIO_STM32_InvertPin(GPIO_STM32 *self)
{
    /* The BSRR register cannot be read. We have to read ODR to get the current
    status of the pin instead */
    uint32_t pinMask = 1UL << (self->super->pinNumber);

    if(self->st_port != NULL)
    {
        uint32_t pinState = self->st_port->ODR & pinMask;

        /* The upper 16 bits reset the pin. The lower 16 bits set the pin.
        Zero writes are ignored completely. */
        if(pinState)
        {
            self->st_port->BRR = pinMask << 16;
        }
        else
        {
            self->st_port->BRR = pinMask;
        }
    }
}

// *****************************************************************************

void GPIO_STM32_WritePin(GPIO_STM32 *self, bool setPinHigh)
{
    if(setPinHigh)
        GPIO_STM32_SetPin(self);
    else
        GPIO_STM32_ClearPin(self);
}

// *****************************************************************************

uint16_t GPIO_STM32_ReadPin(GPIO_STM32 *self)
{
    uint16_t retValue = 0;
    uint32_t modeMask;

    if(self->st_port != NULL)
    {
        modeMask = self->st_port->MODER & (3UL << (self->super->pinNumber * 2));
        modeMask >>= (self->super->pinNumber * 2);

        if(modeMask == 0)
        {
            // digital input
            if(self->st_port->IDR & (1UL << self->super->pinNumber))
                retValue = 1;
        }
        else if(modeMask == 0b11)
        {
            retValue = *(self->super->ptrToAnalogValue);
        }
    }
    return retValue;
}

// *****************************************************************************

bool GPIO_STM32_ReadBool(GPIO_STM32 *self)
{
    bool retValue = false;
    
    if(self->st_port != NULL)
    {
        if(self->st_port->IDR & (1UL << self->super->pinNumber))
            retValue = true;
    }
    return retValue;
}

// *****************************************************************************

void GPIO_STM32_SetType(GPIO_STM32 *self, GPIOType type)
{
    if(self->st_port == NULL)
        return;

    switch(type)
    {
        case GPIO_TYPE_ANALOG:
            self->st_port->MODER |= (3UL << (self->super->pinNumber * 2));
            break;
        case GPIO_TYPE_DIGITAL_INPUT:
            self->st_port->MODER &= ~(3UL << (self->super->pinNumber * 2));
            break;
        case GPIO_TYPE_DIGITAL_OUTPUT:
            self->st_port->OTYPER &= ~(1UL << (self->super->pinNumber));
            self->st_port->MODER &= ~(3UL << (self->super->pinNumber * 2));
            self->st_port->MODER |= (1UL << (self->super->pinNumber * 2));
            break;
        case GPIO_TYPE_OPEN_DRAIN:
            self->st_port->OTYPER |= (1UL << (self->super->pinNumber));
            self->st_port->MODER &= ~(3UL << (self->super->pinNumber * 2));
            self->st_port->MODER |= (1UL << (self->super->pinNumber * 2));
            break;
        default:
            // analog
            self->st_port->MODER |= (3UL << (self->super->pinNumber * 2));
            break;
    }
}

// *****************************************************************************

GPIOType GPIO_STM32_GetType(GPIO_STM32 *self)
{
    GPIOType retVal = GPIO_TYPE_ANALOG; // reset value
    uint32_t modeMask;

    if(self->st_port != NULL)
    {
        modeMask = self->st_port->MODER & (3UL << (self->super->pinNumber * 2));
        modeMask >>= (self->super->pinNumber * 2);

       if(modeMask == 0)
       {
            retVal = GPIO_TYPE_DIGITAL_INPUT;
       }
       else if(modeMask == 1)
       {
            if(self->st_port->OTYPER & (1UL << (self->super->pinNumber)))
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

void GPIO_STM32_SetPull(GPIO_STM32 *self, GPIOPull pullType)
{
    if(self->st_port == NULL)
        return;

    self->st_port->PUPDR &= ~(3UL << (self->super->pinNumber * 2));

    if(pullType == GPIO_PULL_UP)
        self->st_port->PUPDR |= (1UL << (self->super->pinNumber * 2));
    else if(pullType == GPIO_PULL_DOWN)
        self->st_port->PUPDR |= (2UL << (self->super->pinNumber * 2));
}

// *****************************************************************************

GPIOPull GPIO_STM32_GetPull(GPIO_STM32 *self)
{
    GPIOPull retVal = GPIO_PULL_NONE;
    uint32_t pullMask;

    if(self->st_port != NULL)
    {
        pullMask = self->st_port->MODER & (3UL << (self->super->pinNumber * 2));
        pullMask >>= (self->super->pinNumber * 2);

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