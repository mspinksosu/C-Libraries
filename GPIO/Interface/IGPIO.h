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
 * There are two types of objects. One that holds the properties needed for the
 * pin, called GPIO and the other that holds the parameters needed to
 * initialize the pin called GPIOInitType. I did this so that all of the pin's
 * parameters don't have to be stored in memory. If you desire, you can use
 * one init type during initialization, change it's parameters, and call it
 * with the init functions. Once the init function is finished, the init type 
 * variable is destroyed and memory is freed.
 * 
 * A base class must contain at minimum, a pointer to the interface and a void 
 * pointer called "instance". In this particular case, the MCU will never have
 * more than one GPIO interface, so I've ommitted the pointer to the interface.
 * Sub class objects need their own variables which are not in the base class. 
 * This is why the void pointer is used. After the base and sub class are 
 * linked, the void pointer will be set to point to the sub class.
 * 
 * The GPIOInterface or function table tells the interface what functions to 
 * call. When we create the function table, we are initializing its members 
 * (which are function pointers) the our local functions. The function pointers
 * listed in the table will use void pointers for the base class objects. In
 * the implementation, the function signatures will use the sub class objects.
 * Setting the functions in the implementation to the function pointers in the
 * table will require typecasting.
 * 
 * After creating a sub class, it needs to be connected to the base class by
 * using the Create functions. By using a void pointer to point to an instance 
 * of the subclass object, we remove the need for the user to have to do some 
 * ugly typecasting on every single function call. I think the best way to do 
 * this step is to call this function from a sub class create function. Make a 
 * function in your implementation called "create" that uses your sub class 
 * type and the base class type as arguments. This function is not listed
 * in the function table. From within that function, call the base class create 
 * function to finish setting the void pointer. The reason I prefer this method 
 * is it makes the process a little more type-safe by having the base class
 * and sub class as arguments in the function signature.
 * 
 * If you need access to the pins from anywhere besides where the pins were 
 * declared, you can make the base type external. The function table will call 
 * the appropriate sub class function and give it the sub class object. All you 
 * would need is the base class GPIO variable and IGPIO.h Doing it this way 
 * will hide the sub class away and removes the need to include those files, 
 * which would otherwise create a processor specific dependancy.
 * 
 ******************************************************************************/

#ifndef IGPIO_H
#define IGPIO_H

#include <stdint.h>
#include <stdbool.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

typedef struct GPIOInterfaceTag
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
} GPIOInterface;

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
 * GPIOInterface This is the table of functions that need to be implemented
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

void GPIO_DriverSetInterface(GPIOInterface *thisInterface);

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
