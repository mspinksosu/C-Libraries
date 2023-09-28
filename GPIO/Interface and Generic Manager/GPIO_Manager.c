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
 *     I removed the initialization function that was in the GPIO library and 
 * put it here. This gives more flexibility with how we decide to handle pin 
 * organization.
 * 
 * This file is just a suggestion on how to handle initializing your pins.
 * Use it, or make your own to replace it.
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

/* include your processor GPIO implementation header here */
// #include "GPIO_STM32G0.h"

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
    /* The function table is declared in your own GPIO implementation header
    file and defined in your implementation .c file */
    GPIO_DriverSetInterface(&GPIO_FunctionTable);

    GPIOInitType init;

    /* Declare any GPIOInitType subclass variables here */
    GPIOInitType_MCU1 _init;

    /* Set default processor specific init properties. Properties can be 
    changed for each individual pin before calling the GPIO init function. Just 
    remember to reset them before going to the next pin. */
    _init.speed = 0;
    _init.alternate = 0;

    /* Call your sub class create init type function. This part only needs to 
    be done once */
    GPIO_MCU1_CreateInitType(&_init, &init);

// ----- Add your pins --------------------------------------------------

    /* Set the generic property for the pin first, then the processor specific 
    pin properties. Then call the pin init function. */
    init.type = LED1_PIN_TYPE;
    init.pull = LED1_PIN_PULL;
    _init.alternate = 1; // example processor specific property 
    _init.speed = 1;
    GPIO_InitPin(LED1_PIN, &init);
    _init.alternate = 0;
    _init.speed = 0;

    init.type = LED2_PIN_TYPE;
    init.pull = LED2_PIN_PULL;
    GPIO_InitPin(LED2_PIN, &init);

    init.type = GPIO_TYPE_DIGITAL_INPUT;
    init.pull = GPIO_PULL_UP;
    GPIO_InitPin(GPIO_PORTC, 6, &init);

}