/***************************************************************************//**
 * @brief GPIO Library Implementation Header (STM32G0)
 * 
 * @file GPIO_STM32G0.h
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 2/4/22    Original creation
 * 
 * @details
 *      A GPIO library for STM32G0 that implements the IGPIO interface. There 
 * are two sub classes in this library. One that extends the GPIO pin to give 
 * access to the STM32 port, and the other which extends the GPIOInitType to 
 * gain access to the alternate pin functions. The sub classes contain a 
 * pointer to the base class in order to access those variables.
 * 
 * The external interface variable, GPIO_FunctionTable is declared and defined
 * in the .c file. This header file needs to be included wherever the 
 * initialization takes place. However, it is only needed for this intial step. 
 * It is not needed to perform operations on the pins. Any file needing access 
 * to pins can include the IGPIO and use the interface. This removes the need
 * to have processor specific dependancies anywhere else. The GPIO objects will
 * need to be made extern for any other file to use them.
 * 
 * There are two sub class create functions which are not included as part of
 * the interface. The base class and sub class are connected together by 
 * calling these functions. The Create functions will set the pointer to the 
 * base class and then call the base class Create function. Using these sub 
 * class create functions adds some type safety by including the sub class in 
 * the function signature.
 * 
 * Refer to the GPIO interface for description of what each of the other
 * functions should do.
 * 
 * In the example below, the variables "init" and "_init" can be made local
 * and modified for each pin's initialization.
 * 
 * @section example_code Example Code
 *      GPIO_DriverSetInterface(&GPIO_FunctionTable);
 *      GPIO led1;
 *      GPIO_STM32 _led1; // extends led 1
 *      GPIOInitType init;
 *      GPIOInitType_STM32 _init // extends GPIO init type
 *      init.type = GPIO_TYPE_DIGITAL_OUTPUT;
 *      _init.alternate = 2; // STM32 alternate pin function
 *      GPIO_STM32_CreateInitType(&init, &_init); // connect sub and base class
 *      GPIO_STM32_Create(&led1, &myMcuPin1); // connect sub and base class
 *      GPIO_InitPin(&led1, &_led1);
 *      GPIO_Set(&led1); // set output high
 *      GPIO_SetType(&led1, GPIO_TYPE_ANALOG); // ready pin for sleep
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

#ifndef GPIO_STM32G0_H
#define GPIO_STM32G0_H

#include "IGPIO.h"

/* Include processor specific header files here if needed */
#include "stm32g071xx.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

extern GPIOInterface GPIO_FunctionTable;

typedef struct GPIOInitType_STM32Tag
{
    GPIOInitType *super;

    /* Add any processor specific variables you need here */
    uint32_t speed;
    uint32_t alternate;
} GPIOInitType_STM32;

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Connects the sub class to the base class
 * 
 * Calls the base class GPIO_CreateInitType function. Each sub class must have 
 * a base class. However, you do not need an InitType for every single pin. You
 * can create one init type during your initialization and use it repeatedly
 * until all of your pins are initialized.
 * 
 * @param self  pointer to the GPIOInitType_STM32 subclass
 * 
 * @param base  pointer to the GPIOInitType
 */
void GPIO_STM32_CreateInitType(GPIOInitType_STM32 *self, GPIOInitType *base);

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions with Inheritance  *******************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void GPIO_STM32_InitPin(GPIOPort port, uint8_t pin, GPIOInitType_STM32 *params);

#endif  /* GPIO_STM32G0_H */
