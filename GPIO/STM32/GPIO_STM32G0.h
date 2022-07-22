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
 *      A library that handles basic features of a pin. This is an extension
 * of both the GPIO pin and GPIOInitType for the STM32 processor. The sub
 * classes need a pointer to the base class in order to access those variables.
 * 
 * The base class and sub class are connected together by calling the Create
 * functions. The Create functions will set the pointer to the base class and
 * then call the base class Create function. Using this sub class Create
 * function adds some type safety by including the sub class in the function
 * signature.
 * 
 ******************************************************************************/

#ifndef GPIO_STM32G0_H
#define GPIO_STM32G0_H

#include "IGPIO.h"

/* Include processor specific header files here if needed */
#include "stm32g071xx.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/* Class specific variables */
extern GPIO_Interface GPIOFunctionTable;

/* Create your processor specific class. */
typedef struct GPIO_STM32Tag
{
    GPIO *super; // include the base class first
    
    /* Add any processor specific variables you need here */
    GPIO_TypeDef *st_port;
} GPIO_STM32;

typedef struct GPIOInitType_STM32Tag
{
    GPIOInitType *super; // include the base class first

    /* Add any processor specific variables you need here */
    uint32_t speed;
    uint32_t alternate;
} GPIOInitType_STM32;

// ***** Function Prototypes ***************************************************

// ----- Non-Interface Functions -----------------------------------------------

//void GPIO_STM32_InitAllPins(void);

void GPIO_STM32_Create(GPIO_STM32 *self, GPIO *base);

void GPIO_STM32_CreateInitType(GPIOInitType_STM32 *self, GPIOInitType *base);

// ----- Interface Functions ---------------------------------------------------

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
