/***************************************************************************//**
 * @brief GPIO Interface Header File
 * 
 * @author Matthew Spinks
 * 
 * @date February 2, 2022  Original creation
 * 
 * @file IGPIO.h
 * 
 * @details
 *      An interface for a GPIO library to be used with different processors.
 * There is a base class for all generic pin parameters such as pin number,
 * and an init class for other parameters such as type, direction, pull-up/down. 
 * The only thing not covered by the base class is the port. Different 
 * processors may have different ways to define a port, so I decided to put 
 * that with the processor specific properties, or sub class.
 * 
 * A base class must contain at minimum, a pointer to the sub class's interface 
 * and a void pointer. The void pointer "instance" will point to whatever sub 
 * class object is created. Sub class types will need their own different 
 * variables. This is why a void pointer is used. After creating the sub class, 
 * the void pointer will be changed to point to it. 
 * 
 * The GPIO_Interface or function table tells the interface what functions to 
 * call. When we create the function table, we are initializing its members 
 * (which are function pointers) the our local functions. Typecasting is 
 * necessary.
 * 
 * The base class "Create" function links the base class with the sub class.
 * I prefer not to call this create function directly though. 
 * Instead I opt to call it from the sub class create function. This makes 
 * calling the base class create function a little more typesafe, by avoiding
 * accidentally setting the void pointer to something it's not supposed to be.
 * 
 ******************************************************************************/

#ifndef IGPIO_H
#define IGPIO_H

#include <stdint.h>
#include <stdbool.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

typedef struct GPIO_InterfaceTag
{
    /*  These are the functions that will be called. You will create your own
    interface object for your class that will have these function signatures 
    References to the base class are replaced with void. You will set each of 
    your functions equal to one of these pointers. Typecasting will be needed */ 
    void (*GPIO_InitPin)(void *instance, void *params);
    void (*GPIO_SetPin)(void *instance);
    void (*GPIO_ClearPin)(void *instance);
    void (*GPIO_InvertPin)(void *instance);
    void (*GPIO_WritePin)(void *instance, bool setPinHigh);
    uint16_t (*GPIO_ReadPin)(void *instance);
    bool (*GPIO_ReadBool)(void *instance);
    void (*GPIO_SetType)(void *instance, GPIOType);
    GPIOType (*GPIO_GetType)(void *instance);
    void (*GPIO_SetPull)(void *instance, GPIOPull);
    GPIOPull (*GPIO_GetPull)(void *instance);
    // Add more functions below
} GPIO_Interface;

typedef enum GPIOTypeTag
{
    GPIO_TYPE_ANALOG,
    GPIO_TYPE_DIGITAL_INPUT,
    GPIO_TYPE_DIGITAL_OUTPUT,
    GPIO_TYPE_OPEN_DRAIN,
} GPIOType;

typedef enum GPIOPullTag
{
    GPIO_PULL_NONE,
    GPIO_PULL_UP,
    GPIO_PULL_DOWN,
} GPIOPull;

typedef struct GPIOTag
{
    void *instance;
    uint16_t pinNumber;
    uint16_t *ptrToAnalogValue;
} GPIO;

typedef struct GPIOInitTypeTag
{
    void *instance;
    GPIOType type;
    GPIOPull pull;
} GPIOInitType;

/**
 * GPIO_Interface This is the table of functions that need to be implemented
 * 
 * GPIO         The base class pin object
 * 
 * GPIOInitType    The base class init object
 * 
 * instance     A pointer to the instance of the sub class being used. This
 *              will be set by means of the base class create function
 * 
 * type         The type of pin, analog input, digital input, etc.
 * 
 * pull         Internal pull-up / pull-down resistors. Default is none
 * 
 * pinNumber    The pin number. Can be the pin number for the port (which needs
 *              to be implemented in the subclass) or just a number. How you
 *              choose to dereference it is up to you.
 * 
 * ptrToAnalogValue An option to connect your pin to an ADC channel's value.
 *                  The read pin function will return this value if the pin
 *                  is an analog type
 */

// ***** Function Prototypes ***************************************************

// ----- Non-Interface Functions -----------------------------------------------

void GPIO_DriverSetInterface(GPIO_Interface *thisInterface);

void GPIO_Create(GPIO *self, void *instanceOfSubClass);

void GPIO_CreateInitType(GPIOInitType *params, void *instanceOfSubClass);

void GPIO_SetPointerToAnalogValue(GPIO *self, uint16_t *ptr);

// ----- Interface Functions ---------------------------------------------------

void GPIO_InitPin(GPIO *self, GPIOInitType *params);

void GPIO_SetPin(GPIO *self);

void GPIO_ClearPin(GPIO *self);

void GPIO_InvertPin(GPIO *self);

void GPIO_WritePin(GPIO *self, bool setPinHigh);

uint16_t GPIO_ReadPin(GPIO *self);

bool GPIO_ReadBool(GPIO *self);

void GPIO_SetType(GPIO *self, GPIOType type);

GPIOType GPIO_GetType(GPIO *self);

void GPIO_SetPull(GPIO *self, GPIOPull pullType);

GPIOPull GPIO_GetPull(GPIO *self);


#endif  /* IGPIO_H */
