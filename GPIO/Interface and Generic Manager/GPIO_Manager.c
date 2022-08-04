
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
 *     I removed the initialization function that was in the GPIO library and 
 * put it here. Code in the GPIO library should only be concerned with pins. 
 * It shouldn't care how the user initializes all their pins. This gives more 
 * flexibility with how we decide to handle pin organization.
 * 
 ******************************************************************************/

#include "GPIO_Manager.h"

/* include your GPIO implementation here */

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/* Declare GPIO pins starting here. Each GPIO variable should have a 
matching extern declaration in a header file. Only the GPIO variable needs to
be made extern. Use a memorable name. This is the object you will use for 
library function calls. */
//------------------------------------------------------------------------------

GPIO pin1, pin2;

/* If you have any GPIO sub classes, declare them here */

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
    /* The function table is declared in your own GPIO implementation header
    file and defined in your implementation .c file */
    GPIO_DriverSetInterface(&GPIOFunctionTable);

    GPIOInitType init;

    /* Declare any GPIOInitType subclasses here */

    /* Processor specific init properties can be changed for each individual 
    pin before calling the GPIO init function */

    /* Call your sub class create init type function. This part only needs to 
    be done once */
    GPIO_STM32_CreateInitType(&_init, &init);

// ----- Add your pins --------------------------------------------------

    /* Set the generic property for the pin first, then the processor specific 
    pin properties. Next, set any generic init properties and processor 
    specific init properties. Then, call your subclass create function. 
    Finally, call the pin init function. */
    pin1.pinNumber = 6;
    init.type = GPIO_TYPE_DIGITAL_OUTPUT;
    init.pull = GPIO_PULL_NONE;
    GPIO_InitPin(&pin1, &init);
    
    pin2.pinNumber = 15;
    init.type = GPIO_TYPE_DIGITAL_OUTPUT;
    init.pull = GPIO_PULL_NONE;
    _init.speed = 0;
    GPIO_InitPin(&pin2, &init);

}