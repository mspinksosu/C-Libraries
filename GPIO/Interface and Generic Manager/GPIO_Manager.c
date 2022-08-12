
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
 * put it here. Code in the GPIO library should only be concerned with 
 * modifying the pins. It shouldn't care how the user initializes all of their 
 * pins. This gives more flexibility with how we decide to handle pin 
 * organization.
 * 
 * This file is just a suggestion on how to handle initializing your pins.
 * Use it, or make your own to replace it.
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

GPIO led1, led2;

/* If you have any GPIO sub classes, declare them here */
// GPIO_MCU1 _led1, _led2

//******************************************************************************

void GPIO_Manager_InitAllPins(void)
{
    /* The function table is declared in your own GPIO implementation header
    file and defined in your implementation .c file */
    GPIO_DriverSetInterface(&GPIOFunctionTable);

    GPIOInitType init;

    /* Declare any GPIOInitType subclass variables here */
    // GPIOInitType_MCU1 _init;

    /* Processor specific init properties can be changed for each individual 
    pin before calling the GPIO init function */
    // _init.alternate = 0;

    /* Call your sub class create init type function. This part only needs to 
    be done once */
    // GPIO_MCU1_CreateInitType(&_init, &init);

// ----- Add your pins --------------------------------------------------

    /* Set the generic property for the pin first, then the processor specific 
    pin properties. Next, set any generic init properties and processor 
    specific init properties. Then, call your subclass create function. 
    Finally, call the pin init function. */
    led1.pinNumber = 6;
    init.type = GPIO_TYPE_DIGITAL_OUTPUT;
    init.pull = GPIO_PULL_NONE;
    GPIO_InitPin(&led1, &init);
    
    led2.pinNumber = 15;
    init.type = GPIO_TYPE_DIGITAL_OUTPUT;
    init.pull = GPIO_PULL_NONE;
    GPIO_InitPin(&led2, &init);

}