/***************************************************************************//**
 * @brief GPIO Library Implementation Header (STM32G0)
 * 
 * @author Matthew Spinks
 * 
 * @date February 4, 2022  Original creation
 * 
 * @file GPIO_STM32G0.h
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
 * Example Code:
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
 ******************************************************************************/

#ifndef GPIO_STM32G0_H
#define GPIO_STM32G0_H

#include "IGPIO.h"

/* Include processor specific header files here if needed */
#include "stm32g071xx.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

extern GPIOInterface GPIO_FunctionTable;

/* Processor specific class */
typedef struct GPIO_STM32Tag
{
    GPIO *super;
    
    /* Add any processor specific variables you need here */
    GPIO_TypeDef *st_port;
} GPIO_STM32;

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
 * Calls the base class GPIO_Create function. Each sub class object must have 
 * a base class.
 * 
 * @param self  pointer to the GPIO_STM32 subclass that you are using
 * 
 * @param base  pointer to the base class GPIO object
 */
void GPIO_STM32_Create(GPIO_STM32 *self, GPIO *base);

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
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void GPIO_STM32_InitPin(GPIO_STM32 *self, GPIOInitType_STM32 *params);

void GPIO_STM32_SetPin(GPIO_STM32 *self);

void GPIO_STM32_ClearPin(GPIO_STM32 *self);

void GPIO_STM32_InvertPin(GPIO_STM32 *self);

void GPIO_STM32_WritePin(GPIO_STM32 *self, bool setPinHigh);

uint16_t GPIO_STM32_ReadPin(GPIO_STM32 *self);

bool GPIO_STM32_ReadBool(GPIO_STM32 *self);

void GPIO_STM32_SetType(GPIO_STM32 *self, GPIOType type);

GPIOType GPIO_STM32_GetType(GPIO_STM32 *self);

void GPIO_STM32_SetPullType(GPIO_STM32 *self, GPIOPull pullType);

GPIOPull GPIO_STM32_GetPullType(GPIO_STM32 *self);

#endif  /* GPIO_STM32G0_H */
