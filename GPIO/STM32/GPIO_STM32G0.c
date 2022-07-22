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
 *      There are two sub classes with this library. One that extends the GPIO
 * pin to give access to the STM32 port, and the other which extends the 
 * GPIOInitType to gain access to the alternate pin functions. The GPIOInitType
 * does not have to be preserved in memory. You can create a GPIOInitType 
 * inside an init function to use for all pin initialization. Once the function
 * is finished, that variable is destroyed.
 * 
 * The GPIOInterface or function table tells the interface what functions to 
 * call. When we create the function table, we are initializing its members 
 * (which are function pointers) the our local functions. Typecasting is 
 * necessary.
 * 
 * Since there is only ever going to be one interface for the GPIO driver, 
 * the interface for the GPIO driver is static. In order to set the function
 * table, call the GPIO_SetDriverInterface and give it a pointer to this
 * function table.
 * 
 * The base class and sub class are connected together by calling the Create
 * functions. This step should be done using the sub classes Create functions. 
 * Using the sub class Create function sets the appropriate pointers and adds 
 * some type safety by including the sub class in the function signature.
 * 
 * When using the GPIO interface functions, calls to the base functions are 
 * done with the GPIO type. If you need access to the pins from anywhere 
 * besides where the pins were declared, you can make just the base type 
 * external. The function table will call the appropriate sub class function 
 * and give it the sub class object. All you would need is the base class GPIO
 * variable and IGPIO.h Doing it this way will hide the sub class away and 
 * removes the need to include those files, which would otherwise create a 
 * processor specific dependancy.
 * 
 * Example Code:
 *      GPIO_DriverSetInterface(&GPIOFunctionTable);
 *      GPIO pin1;
 *      GPIO_STM32 _pin1; // extends pin 1
 *      GPIOInitType init;
 *      GPIOInitType_STM32 _init // extends GPIO init type
 *      init.type = GPIO_TYPE_DIGITAL_OUTPUT;
 *      _init.alternate = 2; // STM32 alternate pin function
 *      GPIO_STM32_CreateInitType(&init, &_init); // connect sub and base class
 *      GPIO_STM32_Create(&pin1, &myMcuPin1); // connect sub and base class
 *      GPIO_InitPin(&pin1, &_pin1);
 *      GPIO_Set(&pin1); // set output high
 *      GPIO_SetType(&pin1, GPIO_TYPE_ANALOG); // ready pin for sleep
 * 
 ******************************************************************************/

#include <stddef.h> // needed for NULL
#include "GPIO_STM32G0.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/* Create the function table */
GPIO_Interface GPIOFunctionTable = {
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

// ***** Function Prototypes ***************************************************

/* Put static function prototypes here */


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Set up the GPIO pin and call the constructor
 * 
 * This connects the base class GPIO with the sub class GPIO
 * 
 * @param stm32Pin  pointer to the STM32 GPIO you are using
 *  
 * @param base  pointer to the base object you are inheriting from
 */
void GPIO_STM32_Create(GPIO_STM32 *stm32Pin, GPIO *base)
{
    /* Set the pointer to parent class and then call the base class
    constructor */
    stm32Pin->super = base;
    GPIO_Create(base, stm32Pin);
}

/***************************************************************************//**
 * @brief Set up the GPIO Init type and call the constructor
 * 
 * This connects the base class GPIO with the sub class GPIO
 * 
 * @param stm32Params  pointer to the STM32 GPIO Init type you are using
 *  
 * @param base  pointer to the base object you are inheriting from
 */
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

/***************************************************************************//**
 * @brief Initialize necessary registers for this MCU
 * 
 * @param self  pointer to the STM32 GPIO you are using
 * 
 * @param params  pointer to the STM32 GPIO Init type you are using
 */
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

/***************************************************************************//**
 * @brief Set a pin high
 * 
 * @param self  pointer to the STM32 GPIO you are using
 */
void GPIO_STM32_SetPin(GPIO_STM32 *self)
{
    /* For the BSRR register, the lower 16 bits set the pins. The upper 16 bits
    reset the pins. Zero writes are ignored completely */
    if(self->st_port != NULL && self->super->pinNumber < 16)
        self->st_port->BSRR = (1UL << (self->super->pinNumber));
}

/***************************************************************************//**
 * @brief Clear a pin
 * 
 * @param self  pointer to the STM32 GPIO you are using
 */
void GPIO_STM32_ClearPin(GPIO_STM32 *self)
{
    /* The lower 16 bits of the BRR register reset the pins. Zero writes are
    ignored completely */
    if(self->st_port != NULL)
        self->st_port->BRR = (1UL << (self->super->pinNumber));
}

/***************************************************************************//**
 * @brief Invert a pin's value
 * 
 * @param self  pointer to the STM32 GPIO you are using
 */
void GPIO_STM32_InvertPin(GPIO_STM32 *self)
{
    /* The BSRR register cannot be read. We have to read ODR to get the current
    status of the pin instead */
    uint32_t pinMask = 1UL << (self->super->pinNumber);
    uint32_t pinState = self->st_port->ODR & pinMask;

    if(self->st_port != NULL)
    {
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

/***************************************************************************//**
 * @brief Write a value to a pin
 * 
 * @param self  pointer to the STM32 GPIO you are using
 * 
 * @param setPinHigh  true = high, false = low
 */
void GPIO_STM32_WritePin(GPIO_STM32 *self, bool setPinHigh)
{
    if(setPinHigh)
        GPIO_STM32_SetPin(self);
    else
        GPIO_STM32_ClearPin(self);
}

/***************************************************************************//**
 * @brief Read the value of a pin
 * 
 * If the pin is analog, it should return the analog value if available.
 * Otherwise, return either a 1 or 0. Works with boolean logic also.
 * 
 * @param self  pointer to the STM32 GPIO you are using
 * 
 * @return uint16_t  adc reading if analog, if digital 0 = low, 1 = high
 */
uint16_t GPIO_STM32_ReadPin(GPIO_STM32 *self)
{
    uint16_t retValue = 0;
    uint32_t modeMask = self->st_port->MODER & (3UL << (self->super->pinNumber * 2));
    modeMask >>= (self->super->pinNumber * 2);

    if(self->st_port != NULL)
    {
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

/***************************************************************************//**
 * @brief Read the boolean value of a pin
 * 
 * An alternative to the ReadPin function. If the pin is digital, then return 
 * either true or false
 * 
 * @param self  pointer to the GPIO you are using
 * 
 * @return true if digital and pin is high, otherwise false
 */
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

/***************************************************************************//**
 * @brief Set the pin type
 * 
 * @param self  pointer to the GPIO you are using
 * 
 * @param type  the type of pin, analog input, digital output, etc.
 */
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

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 * 
 * @return GPIOType 
 */
GPIOType GPIO_STM32_GetType(GPIO_STM32 *self)
{
    GPIOType retVal = GPIO_TYPE_ANALOG; // reset value
    uint32_t modeMask = self->st_port->MODER & (3UL << (self->super->pinNumber * 2));
    modeMask >>= (self->super->pinNumber * 2);

    if(self->st_port != NULL)
    {
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

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 * 
 * @param pullType 
 */
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

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 * 
 * @return GPIOPull 
 */
GPIOPull GPIO_STM32_GetPull(GPIO_STM32 *self)
{
    GPIOPull retVal = GPIO_PULL_NONE;
    uint32_t pullMask = self->st_port->MODER & (3UL << (self->super->pinNumber * 2));
    pullMask >>= (self->super->pinNumber * 2);

    if(self->st_port != NULL)
    {
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