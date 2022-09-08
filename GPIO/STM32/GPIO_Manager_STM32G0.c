
/***************************************************************************//**
 * @brief GPIO Library Manager (STM32G0)
 * 
 * @author Matthew Spinks
 * 
 * @date 7/23/22   Original creation
 * 
 * @file GPIO_Manager_STM32G0.c
 * 
 * @details
 *      This is a special version of the GPIO Manager that works with the 
 * STM32G0 implementation. It sets the function table, and initializes all of
 * the pins. This file replaces GPIO_Manager.c.
 * 
 ******************************************************************************/

#include "GPIO_Manager.h"
#include "GPIO_STM32G0.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/* Declare GPIO pins starting here. Each GPIO variable should have a 
matching extern declaration in a header file. Only the GPIO variable needs to
be made extern. Use a memorable name. This is the object you will use for 
library function calls. */
//------------------------------------------------------------------------------

GPIO led1, led2;
GPIO_STM32 _led1, _led2;

//------------------------------------------------------------------------------


/***************************************************************************//**
 * @brief Initialize all pins
 * 
 * Each pin has to be declared as a global variable. However, only the base
 * class GPIO needs to be extern. Anything that needs to access these pins
 * will include the header file which has the extern declaration.
 */
void GPIO_Manager_InitAllPins(void)
{
    GPIO_DriverSetInterface(&GPIO_FunctionTable); // defined in GPIO_STM32G0.h

    GPIOInitType init;
    GPIOInitType_STM32 _init;

    /* Processor specific init properties can be changed for each individual 
    pin before calling the GPIO init function */
    _init.speed = 0;
    _init.alternate = 0;

    /* This part only needs to be done once */
    GPIO_STM32_CreateInitType(&_init, &init);

// ----- Add your pins --------------------------------------------------

    /* Set the generic property for the pin first, then the processor specific 
    pin properties. Next, set any generic init properties and processor 
    specific init properties. Then, call your subclass create function. 
    Finally, call the pin init function. */
    led1.pinNumber = 6;
    _led1.st_port = GPIOC;
    init.type = GPIO_TYPE_DIGITAL_OUTPUT;
    init.pull = GPIO_PULL_NONE;
    _init.alternate = 0;
    GPIO_STM32_Create(&_led1, &led1);
    GPIO_InitPin(&led1, &init);
    
    led2.pinNumber = 15;
    _led2.st_port = GPIOA;
    init.type = GPIO_TYPE_DIGITAL_OUTPUT;
    init.pull = GPIO_PULL_NONE;
    _init.speed = 0;
    GPIO_STM32_Create(&_led2, &led2);
    GPIO_InitPin(&led2, &init);

}