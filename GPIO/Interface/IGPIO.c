/***************************************************************************//**
 * @brief GPIO Interface
 * 
 * @author Matthew Spinks
 * 
 * @date February 6, 2022  Original creation
 * 
 * @file IGPIO.c
 * 
 * @details
 *      An interface for a GPIO library to be used with different processors.
 * 
 * Libraries that use this interface must implement the functions listed in the
 * interface, or function table. Since, there should only ever be one single
 * GPIO driver per processor, I am going to make the function table static. 
 * In order to set the function table, call the GPIO_SetDriverInterface 
 * and give it the function table for your processor specific code.
 * 
 * The functions here will use the base class object type. The functions in the
 * sub class will use the sub class object type. When the user calls a function
 * here, the function table, or interface, along with the base class object,
 * will determine which function to call and what sub class to give to the 
 * function.
 * 
 * In the example below, there are two init type objects. These do not have to
 * be kept in memory. They can be declared as local variables and used for each
 * pin's initialization.
 * 
 * Example Code:
 *      GPIO_DriverSetInterface(&MCU1_GPIOInterface);
 *      GPIO pin1;
 *      GPIO_MCU1 myMcuPin1; // extends GPIO pin
 *      GPIOInitType init;
 *      GPIOInitType_MCU1 myMcuInit // extends GPIO init type
 *      init.type = GPIO_TYPE_DIGITAL_OUTPUT;
 *      init.pull = GPIO_PULL_NONE;
 *      myMcuInit.extendedClassMember = 1;
 *      GPIO_MCU1_CreateInitType(&init, &myMcuInit); // connect sub and base
 *      GPIO_MCU1_Create(&pin1, &myMcuPin1); // connect sub class and base
 *      GPIO_InitPin(&pin1, &init);
 *      GPIO_Set(&pin1); // set output high
 *      GPIO_SetType(&pin1, GPIO_TYPE_ANALOG); // ready pin for sleep
 * 
 ******************************************************************************/

#include "IGPIO.h"

// ***** Defines ***************************************************************


// ***** Function Prototypes ***************************************************


// ***** Global Variables ******************************************************

static GPIOInterface *interface;

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Set the interface for this GPIO driver
 * 
 * Because there is usually only one GPIO driver per processor, there doesn't
 * need to be a pointer to the interface in each GPIO object. For this library
 * the interface will be static.
 * 
 * @param thisInterface  the function table you want to use
 */
void GPIO_DriverSetInterface(GPIOInterface *thisInterface)
{
    interface = thisInterface;
}

/***************************************************************************//**
 * @brief Combine the base class and sub class
 * 
 * Links the instance pointer in the base class to the sub class. Because of 
 * the void pointer, my preferred method is to call this function from the sub
 * class constructor. I created a sub class constructor that needs an instance 
 * of the sub class and base class. This makes the create function more type
 * safe.
 * 
 * @param self  pointer to the GPIO you are using
 * 
 * @param instanceOfSubClass  the child object that implements the GPIO
 * 
 */
void GPIO_Create(GPIO *self, void *instanceOfSubClass)
{
    self->instance = instanceOfSubClass;
}

/***************************************************************************//**
 * @brief Combine the base class and sub class
 * 
 * Links the instance pointer in the base class to the sub class. Because of 
 * the void pointer, my preferred method is to call this function from the sub
 * class constructor. I created a sub class constructor that needs an instance 
 * of the sub class and base class. This makes the create function more type
 * safe.
 * 
 * @param params  pointer to the GPIOInit type you are using
 * 
 * @param instanceOfSubClass  the child object that implements the GPIO Init
 */
void GPIO_CreateInitType(GPIOInitType *params, void *instanceOfSubClass)
{
    params->instance = instanceOfSubClass;
}

/***************************************************************************//**
 * @brief Set pointer to analog value
 * 
 * This is useful for mapping analog pins. The calling function doesn't need to
 * know which analog channel a pin is, just the name of the pin. Therefore, 
 * calling functions between two different processors can remain the same.
 * 
 * @param self  pointer to the GPIO you are using
 * 
 * @param ptr  pointer to the analog value
 */
void GPIO_SetPointerToAnalogValue(GPIO *self, uint16_t *ptr)
{
    self->ptrToAnalogValue = ptr;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Initialize a GPIO object
 * 
 * @param self  pointer to the GPIO you are using
 * 
 * @param params  pointer to the GPIO Init type you are using
 */
void GPIO_InitPin(GPIO *self, GPIOInitType *params)
{
    if(interface->GPIO_InitPin != NULL && self->instance != NULL 
            && params->instance != NULL)
    {
        (interface->GPIO_InitPin)(self->instance, params->instance);
    }
}

/***************************************************************************//**
 * @brief Set a pin high
 * 
 * @param self  pointer to the GPIO you are using
 */
void GPIO_SetPin(GPIO *self)
{
    if(interface->GPIO_SetPin != NULL && self->instance != NULL)
    {
        (interface->GPIO_SetPin)(self->instance);
    }
}

/***************************************************************************//**
 * @brief Clear a pin
 * 
 * @param self  pointer to the GPIO you are using
 */
void GPIO_ClearPin(GPIO *self)
{
    if(interface->GPIO_ClearPin != NULL && self->instance != NULL)
    {
        (interface->GPIO_ClearPin)(self->instance);
    }
}

/***************************************************************************//**
 * @brief Invert a pin's value
 * 
 * @param self  pointer to the GPIO you are using
 */
void GPIO_InvertPin(GPIO *self)
{
    if(interface->GPIO_InvertPin != NULL && self->instance != NULL)
    {
        (interface->GPIO_InvertPin)(self->instance);
    }
}

/***************************************************************************//**
 * @brief Write a value to a pin
 * 
 * @param self  pointer to the GPIO you are using
 * 
 * @param setPinHigh  true = high, false = low
 */
void GPIO_WritePin(GPIO *self, bool setPinHigh)
{
    if(interface->GPIO_WritePin != NULL && self->instance != NULL)
    {
        (interface->GPIO_WritePin)(self->instance, setPinHigh);
    }
}

/***************************************************************************//**
 * @brief Read the value of a pin
 * 
 * If the pin is analog and the pointer is set, it will return the analog value.
 * Otherwise, return either a 1 or 0. Works with boolean logic.
 * 
 * @param self  pointer to the GPIO you are using
 * 
 * @return uint16_t  adc reading if analog, if digital 0 = low, 1 = high
 */
uint16_t GPIO_ReadPin(GPIO *self)
{
    if(interface->GPIO_ReadPin != NULL && self->instance != NULL)
    {
        return (interface->GPIO_ReadPin)(self->instance);
    }
    else
    {
        return 0;
    }
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
bool GPIO_ReadBool(GPIO *self)
{
    if(interface->GPIO_ReadBool != NULL && self->instance != NULL)
    {
        return (interface->GPIO_ReadBool)(self->instance);
    }
    else
    {
        return false;
    }
}

/***************************************************************************//**
 * @brief Set the pin type
 * 
 * Set the pin's asscociated registers
 * 
 * @param self  pointer to the GPIO you are using
 * 
 * @param type  GPIO_TYPE_ANALOG or GPIO_TYPE_DIGITAL
 */
void GPIO_SetType(GPIO *self, GPIOType type)
{
    if(interface->GPIO_SetType != NULL && self->instance != NULL)
    {
        (interface->GPIO_SetType)(self->instance, type);
    }
}

/***************************************************************************//**
 * @brief Get the type of pin
 * 
 * Read the pin's asscociated registers and determine the pin type
 * 
 * @param self  pointer to the GPIO you are using
 * 
 * @return GPIOType  GPIO_TYPE_ANALOG or GPIO_TYPE_DIGITAL
 */
GPIOType GPIO_GetType(GPIO *self)
{
    if(interface->GPIO_GetType != NULL && self->instance != NULL)
    {
        return (interface->GPIO_GetType)(self->instance);
    }
    else
    {
        return 0;
    }
}

/***************************************************************************//**
 * @brief Set the pin direction
 * 
 * Set the pin's asscociated registers
 * 
 * @param self  pointer to the GPIO you are using
 * 
 * @param direction  GPIO_DIR_INPUT, GPIO_DIR_OUTPUT, GPIO_DIR_OPEN_DRAIN
 */
void GPIO_SetDirection(GPIO *self, GPIODirection direction)
{
    if(interface->GPIO_SetDirection != NULL && self->instance != NULL)
    {
        (interface->GPIO_SetDirection)(self->instance, direction);
    }
}

/***************************************************************************//**
 * @brief Get the direction of the pin
 * 
 * Read the pin's associated registers and determine the pin direction
 * 
 * @param self  pointer to the GPIO you are using
 * 
 * @return GPIODirection  GPIO_DIR_INPUT, GPIO_DIR_OUTPUT, GPIO_DIR_OPEN_DRAIN
 */
GPIODirection GPIO_GetDirection(GPIO *self)
{
    if(interface->GPIO_GetDirection != NULL && self->instance != NULL)
    {
        return (interface->GPIO_GetDirection)(self->instance);
    }
    else
    {
        return 0;
    }
}

/***************************************************************************//**
 * @brief Set the pull up resistors
 * 
 * Set the pin's asscociated registers
 * 
 * @param self  pointer to the GPIO you are using
 * 
 * @param pullType  GPIO_PULL_UP, GPIO_PULL_DOWN, or GPIO_PULL_NONE
 */
void GPIO_SetPull(GPIO *self, GPIOPull pullType)
{
    if(interface->GPIO_SetPull != NULL && self->instance != NULL)
    {
        (interface->GPIO_SetPull)(self->instance, pullType);
    }
}

/***************************************************************************//**
 * @brief Get the pull up resistors if any
 * 
 * Read the pin's associated registers and determine the pull up type
 * 
 * @param self  pointer to the GPIO you are using
 * 
 * @return GPIOPull  GPIO_PULL_UP, GPIO_PULL_DOWN, or GPIO_PULL_NONE
 */
GPIOPull GPIO_GetPull(GPIO *self)
{
    if(interface->GPIO_GetPull != NULL && self->instance != NULL)
    {
        return (interface->GPIO_GetPull)(self->instance);
    }
    else
    {
        return 0;
    }
}

/*
 End of File
*/