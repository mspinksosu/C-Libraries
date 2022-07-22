
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
 *     TODO I removed the initialization function that was in the GPIO library 
 * and put it here. Code in the GPIO library should only be concerned with pins. 
 * It shouldn't care how the user initializes all their pins. This gives more 
 * flexibility with how we decided to handle pin organization.
 * 
 ******************************************************************************/

#include "GPIO_Manager.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/* Declare GPIO pins starting here. Each GPIO variable should have a 
matching extern declaration in a header file. Only the GPIO variable needs to
be made extern. Use a memorable name. This is the object you will use for 
library function calls. */
//------------------------------------------------------------------------------

GPIO pin1, pin2;
GPIO_STM32 _pin1, _pin2;

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