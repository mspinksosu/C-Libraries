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
 * initialize the pin called GPIOInitType.
 * 
 * To make a GPIO object, you will need to make a base class GPIO object, and
 * a subclass GPIO object. You will also need a base class GPIOInitType object
 * and possibly a subclass GPIOInitType object. The base class will usually 
 * contain at minimum, a pointer to the interface and a void pointer called 
 * "instance". But in this particular case, the MCU will never have more than 
 * one GPIO interface, so I've omitted the pointer to the interface. Sub class 
 * objects need their own variables which are not in the base class. This is 
 * why the void pointer is used. After the base and sub classes are linked, the 
 * void pointer will be set to point to the sub class.
 * 
 * The GPIOInterface or function table tells the interface what functions to 
 * call. Declare your GPIOInterface object as extern in your GPIO 
 * implementation's header file. Then in your implementation's .c file, declare
 * and initialize your GPIOInterface object. Set each of its members to the
 * functions in your implementation. Typecasting will be required. The function
 * pointers in the table will be using void pointers in place of the base class
 * types. In your implementation, your functions will be using your own sub 
 * class types.
 * 
 * A sub class will contain at minimum, a pointer to the base class named
 * "super". After creating a sub class, the base class will be connected to 
 * it by using the base class's Create function. These functions will set the 
 * void pointers for you. By using a void pointer to point to an instance 
 * of the subclass object, we remove the need for the user to have to do some 
 * ugly typecasting on every single function call. You will also need to set
 * your pointer to the base class ("super") to point to whatever base class
 * object you created.
 * 
 * I think the best way to do the previous step is to call the Create function 
 * from a sub class create function. Make your own implementation of the Create 
 * function that uses your sub class type and the base class type as arguments. 
 * This function will not be listed in the function table. From within that 
 * function, set your pointer to the base class ("super"), and then call the 
 * base class create function to finish setting the void pointer. The reason I 
 * prefer this method is it makes the process a little more type-safe by having 
 * the base class and sub class as arguments in the function signature.
 * 
 * If you need access to the pins from anywhere besides where the pins were 
 * declared, you can make the base class external. The function table will call 
 * the appropriate sub class function and give it the sub class object. All you 
 * would need is the base class GPIO variable and IGPIO.h. Doing it this way 
 * hides the sub class away and removes the need to include those files, which 
 * would otherwise create a processor specific dependency.
 * 
 * In the example below, there are two init type objects. These do not have to
 * be kept in memory. They can be declared as local variables and used for each
 * pin's initialization.
 * 
 * Example Code:
 *      GPIO_DriverSetInterface(&MCU1_GPIOInterface);
 *      GPIO led1;
 *      GPIO_MCU1 myMcuLED1; // extends GPIO pin
 *      GPIOInitType init;
 *      GPIOInitType_MCU1 myMcuInit // extends GPIO init type
 *      init.type = GPIO_TYPE_DIGITAL_OUTPUT;
 *      init.pull = GPIO_PULL_NONE;
 *      myMcuInit.extendedClassMember = 1;
 *      GPIO_MCU1_CreateInitType(&init, &myMcuInit); // connect sub and base
 *      GPIO_MCU1_Create(&led1, &myMcuLED1); // connect sub class and base
 *      GPIO_InitPin(&led1, &init);
 *      GPIO_Set(&led1); // set output high
 *      GPIO_SetType(&led1, GPIO_TYPE_ANALOG); // ready pin for sleep
 * 
 ******************************************************************************/

#ifndef IGPIO_H
#define IGPIO_H

#include <stdint.h>
#include <stdbool.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

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
} GPIOInterface;

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
 * Description of struct members. You shouldn't really mess with any of these
 * variables directly. That is why I made functions for you to use.
 * 
 * GPIOInterface The table of functions that need to be implemented
 * 
 * GPIO  The base class pin object
 * 
 * GPIOInitType  The base class init object
 * 
 * instance  A pointer to the instance of the sub class being used. This will 
 *           be set by means of the base class create function
 * 
 * type  The type of pin, analog input, digital input, etc.
 * 
 * pull  Internal pull-up / pull-down resistors. Default is none
 * 
 * pinNumber  The pin number. Can be the pin number for the port (which needs
 *            to be implemented in the subclass) or just a number. How you
 *            choose to dereference it is up to you.
 * 
 * ptrToAnalogValue  An option to connect your pin to an ADC channel's value.
 *                   The read pin function will return this value if the pin
 *                   is an analog type
 */

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
 * @param thisInterface  pointer to the function table you want to use
 */
void GPIO_DriverSetInterface(GPIOInterface *thisInterface);

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
void GPIO_Create(GPIO *self, void *instanceOfSubClass);

/***************************************************************************//**
 * @brief Combine the base class and sub class
 * 
 * Links the instance pointer in the base class to the sub class. Because of 
 * the void pointer, my preferred method is to call this function from a sub
 * class constructor. I created a sub class constructor that needs an instance 
 * of the sub class and base class. This makes the create function more type
 * safe.
 * 
 * @param params  pointer to the GPIOInit type you are using
 * 
 * @param instanceOfSubClass  the child object that implements the GPIOInit
 */
void GPIO_CreateInitType(GPIOInitType *params, void *instanceOfSubClass);

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
void GPIO_SetPointerToAnalogValue(GPIO *self, uint16_t *ptr);

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Initialize a GPIO object
 * 
 * Set the necessary registers for your MCU
 * 
 * @param self  pointer to the GPIO you are using
 * 
 * @param params  pointer to the GPIOInitType that provides the init parameters
 */
void GPIO_InitPin(GPIO *self, GPIOInitType *params);

/***************************************************************************//**
 * @brief Set a pin high
 * 
 * @param self  pointer to the GPIO you are using
 */
void GPIO_SetPin(GPIO *self);

/***************************************************************************//**
 * @brief Clear a pin
 * 
 * @param self  pointer to the GPIO you are using
 */
void GPIO_ClearPin(GPIO *self);

/***************************************************************************//**
 * @brief Invert a pin's value
 * 
 * @param self  pointer to the GPIO you are using
 */
void GPIO_InvertPin(GPIO *self);

/***************************************************************************//**
 * @brief Write a value to a pin
 * 
 * @param self  pointer to the GPIO you are using
 * 
 * @param setPinHigh  true = high, false = low
 */
void GPIO_WritePin(GPIO *self, bool setPinHigh);

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
uint16_t GPIO_ReadPin(GPIO *self);

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
bool GPIO_ReadBool(GPIO *self);

/***************************************************************************//**
 * @brief Set the pin type
 * 
 * Set the pin's associated registers
 * 
 * @param self  pointer to the GPIO you are using
 * 
 * @param type  GPIO_TYPE_ANALOG, GPIO_TYPE_DIGITAL_INPUT, 
 *              GPIO_TYPE_DIGITAL_OUTPUT, GPIO_TYPE_OPEN_DRAIN
 */
void GPIO_SetType(GPIO *self, GPIOType type);

/***************************************************************************//**
 * @brief Get the type of pin
 * 
 * Read the pin's associated registers and determine the pin type
 * 
 * @param self  pointer to the GPIO you are using
 * 
 * @return GPIOType  GPIO_TYPE_ANALOG, GPIO_TYPE_DIGITAL_INPUT, 
 *                   GPIO_TYPE_DIGITAL_OUTPUT, GPIO_TYPE_OPEN_DRAIN
 */
GPIOType GPIO_GetType(GPIO *self);

/***************************************************************************//**
 * @brief Set the pull up resistors
 * 
 * Set the pin's associated registers
 * 
 * @param self  pointer to the GPIO you are using
 * 
 * @param pullType  GPIO_PULL_UP, GPIO_PULL_DOWN, or GPIO_PULL_NONE
 */
void GPIO_SetPull(GPIO *self, GPIOPull pullType);

/***************************************************************************//**
 * @brief Get the pull up resistors if any
 * 
 * Read the pin's associated registers and determine the pull up type
 * 
 * @param self  pointer to the GPIO you are using
 * 
 * @return GPIOPull  GPIO_PULL_UP, GPIO_PULL_DOWN, or GPIO_PULL_NONE
 */
GPIOPull GPIO_GetPull(GPIO *self);


#endif  /* IGPIO_H */
