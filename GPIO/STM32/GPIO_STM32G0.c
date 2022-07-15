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
 * 
 * GPIO_STM32_InitAllPins declares and initializes the pin objects at the same 
 * time. There are two struct objects, the base class and the sub class. The 
 * base class struct contains generic properities and will be made external. 
 * Access to pins will be done using this variable and the functions in the 
 * interface IGPIO.h.
 * 
 * The second struct holds the processor specific parameters needed for 
 * initialization. I've decided to put the port in the processor specific
 * section, because different processors may have different ways to define 
 * a port. After declaring and initializing the two types, a call to the 
 * STM32_CreatePin function will link them together.
 * 
 * The GPIOInterface or function table tells the interface what functions to 
 * call. When we create the function table, we are initializing its members 
 * (which are function pointers) the our local functions. Typecasting is 
 * necessary.
 * 
 * Since there is only ever going to be one interface for the GPIO driver, 
 * the interface for the GPIO driver is static. In order to set the function
 * table, call the GPIO_SetDriverInterface and give it a pointer to this
 * function table
 * 
 * Remember that calls to the base functions are done with the GPIO type, so
 * only the base class object needs to be external. Then the function table 
 * will call the appropriate sub class function and give it the sub class
 * object.
 * 
 * Example Code:
 *      GPIO_DriverSetInterface(&GPIOFunctionTable);
 *      extern GPIO Pin1;
 *      GPIO_STM32 _Pin1;
 *      GPIO_STM32_InitAllPins(); // Pin 1 set to digital output
 *      GPIO_Set(&Pin1); // set output high
 *      GPIO_SetType(&Pin1, GPIO_TYPE_ANALOG); // ready pin for sleep
 *      GPIO_SetDirection(&Pin1, GPIO_DIR_INPUT); // ready pin for sleep
 * 
 ******************************************************************************/

#include <stddef.h>
#include "GPIO_STM32G0.h"

// ***** Global Variables ******************************************************

/* Create the function table */
static GPIO_Interface GPIOFunctionTable = {
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
// ***** Initialization Pin Structs ******************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/* Declare GPIO pins starting here. Each GPIO variable should have a 
matching extern declaration in a header file. Only the GPIO variable needs to
be made extern. Use a memorable name. This is the object you will use for 
library function calls. */
//------------------------------------------------------------------------------

GPIO pin1, pin2;
GPIO_STM32 _pin1, _pin2;

//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Initialize all pins
 * 
 * Each pin has to be declared as a global variable. However, only the base
 * class GPIO needs to be extern. Anything that needs to access these pins
 * will include the header file which has the extern declaration. 
 */
void GPIO_STM32_InitAllPins(void)
{
    GPIOInitType init;
    GPIOInitType_STM32 stm32Init;

    /* Processor specific properties. Can be changed for each individual pin
    before calling the GPIO init function */
    stm32Init.speed = 0;
    stm32Init.alternate = 0;

    /* This part only needs to be done once */
    GPIO_STM32_CreateInitType(&stm32Init, &init);

// ----- Add your pins --------------------------------------------------
    
    /* Set the generic property for the pin first, then the processor specific 
    pin properties. Next, set any generic init properties and processor 
    specific init properties. Lastly call the pin create and init functions. */
    pin1.pinNumber = 6;
    _pin1.st_port = GPIOC;
    init.type = GPIO_TYPE_DIGITAL_OUTPUT;
    init.pull = GPIO_PULL_NONE;
    GPIO_STM32_Create(&_pin1, &pin1);
    GPIO_InitPin(&pin1, &init);
    
    pin2.pinNumber = 15;
    _pin2.st_port = GPIOA;
    init.type = GPIO_TYPE_DIGITAL_OUTPUT;
    init.pull = GPIO_PULL_NONE;
    GPIO_STM32_Create(&_pin2, &pin2);
    GPIO_InitPin(&pin2, &init);

}

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