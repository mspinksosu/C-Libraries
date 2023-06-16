/***************************************************************************//**
 * @brief GPIO Library Manager (STM32G0)
 *  
 * @file GPIO_Manager_STM32G0.c
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 7/23/22   Original creation
 *
 * @details
 *      This is a special version of the GPIO Manager that works with the 
 * STM32G0 implementation. It sets the function table, and initializes all of
 * the pins. This file replaces GPIO_Manager.c.
 * 
 * @section license License
 * SPDX-FileCopyrightText: © 2022 Matthew Spinks
 * SPDX-License-Identifier: Zlib
 * 
 * This software is released under the Zlib license. You are free alter and
 * redistribute it, but you must not misrepresent the origin of the software.
 * This notice may not be removed. <http://www.zlib.net/zlib_license.html>
 * 
 ******************************************************************************/

#include "GPIO_Manager.h"
#include "GPIO_STM32G0.h"

// ***** Defines ***************************************************************

/* I usually place these defines in a separate file due to the size */
#define LED1_PIN        GPIO_PORTA_PIN5
#define LED1_PIN_TYPE   GPIO_TYPE_DIGITAL_OUTPUT
#define LED1_PIN_PULL   GPIO_PULL_NONE

#define LED2_PIN        GPIO_PORTA_PIN15
#define LED2_PIN_TYPE   GPIO_TYPE_DIGITAL_OUTPUT
#define LED2_PIN_PULL   GPIO_PULL_NONE

// ***** Global Variables ******************************************************


//******************************************************************************

void GPIO_Manager_InitAllPins(void)
{
    GPIO_DriverSetInterface(&GPIO_FunctionTable); // defined in GPIO_STM32G0.c

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
    init.type = LED1_PIN_TYPE;
    init.pull = LED1_PIN_PULL;
    _init.alternate = 0;
    GPIO_InitPin(LED1_PIN, &init);
    
    init.type = LED2_PIN_TYPE;
    init.pull = LED2_PIN_PULL;
    _init.speed = 1;
    GPIO_InitPin(LED2_PIN, &init);
    
    init.type = GPIO_TYPE_DIGITAL_INPUT;
    init.pull = GPIO_PULL_UP;
    _init.speed = 0;
    GPIO_InitPin(GPIO_PORTC, 6, &init);

}