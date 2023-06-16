/***************************************************************************//**
 * @brief GPIO Interface Header File
 * 
 * @file IGPIO.h
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 2/2/22    Original creation
 * 
 * @details
 *      An interface for a GPIO library to be used with different processors.
 * The object that holds the parameters needed to initialize the pin is called 
 * GPIOInitType.
 * 
 * To make a GPIOInitType object, you will need to make a base class 
 * GPIOInitType object, and a subclass GPIOInitType object The base class will 
 * usually contain at minimum, a pointer to the interface and a void pointer 
 * called "instance". But in this particular case, the MCU will never have more 
 * than one GPIO interface, so I've omitted the pointer to the interface. Sub 
 * class objects need their own variables which are not in the base class. This 
 * is why the void pointer is used. After the base and sub classes are linked, 
 * the void pointer will be set to point to the sub class.
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
 * // TODO add notes about pin type
 * 
 * In the example below, The init type is declared as a local variables and 
 * used for each pin's initialization before being discarded.
 * 
 * @section example_code Example Code
 * // TODO example code
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

#ifndef IGPIO_H
#define IGPIO_H

#include <stdint.h>
#include <stdbool.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

typedef enum GPIOPortTag
{
    GPIO_PORTA = 0,
    GPIO_PORTB,
    GPIO_PORTC,
    GPIO_PORTD,
    GPIO_PORTE,
    GPIO_PORTF,
    GPIO_PORTG,
    GPIO_PORTH,
    GPIO_PORTI,
    GPIO_PORTJ,
    GPIO_PORTK,
    GPIO_PORTL,
    GPIO_PORTM,
    GPIO_PORTN,
} GPIOPort;

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
    /* These are the functions that will be called. You will create your own
    interface object for your class that will have these function signatures 
    References to the base class are replaced with void. You will set each of 
    your functions equal to one of these pointers. Typecasting will be needed */
    void (*GPIO_InitPin)(GPIOPort port, uint8_t pin, void *params);
} GPIOInterface;

typedef struct GPIOInitTypeTag
{
    void *instance;
    GPIOType type;
    GPIOPull pull;
} GPIOInitType;

/**
 * Description of struct members:
 * 
 * GPIOInterface  The table of functions that need to be implemented
 * 
 * GPIOInitType  The base class init object
 * 
 * GPIOPin  A value that refers to a port and a pin. Processor dependent. I've 
 *          made a list of port plus pin names. How you choose to use them is 
 *          up to you. The port is the upper nibble and the pin is the lower 
 *          nibble. The pin number refers to the number inside the port. It is 
 *          not the number written on the schematic, nor is it a mask. Some 
 *          processors will have only 8 pins per port. Others will have 16.
 * 
 * instance  A pointer to the instance of the sub class being used. This will 
 *           be set by means of the base class create function
 * 
 * type  The type of pin, analog input, digital input, etc.
 * 
 * pull  Internal pull-up / pull-down resistors. Default is none
 * 
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
 * the void pointer, my preferred method is to call this function from a sub
 * class constructor. I created a sub class constructor that needs an instance 
 * of the sub class and base class. This makes the create function more type
 * safe.
 * 
 * @param params  pointer to the GPIOInit type you are using
 * 
 * @param instanceOfSubClass  the child object that inherits from GPIOInitType
 */
void GPIO_CreateInitType(GPIOInitType *params, void *instanceOfSubClass);

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions with Inheritance ********************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Initialize a GPIO object
 * 
 * Set the necessary registers for your MCU. When you call this function, you
 * give it a reference to your base class of GPIOInitType which then calls your
 * subclass function.
 * 
 * @param port  port identifier type
 * 
 * @param pin  the pin in the port (not the pin number on the schematic)
 * 
 * @param params  pointer to the GPIOInitType that provides the init parameters
 */
void GPIO_InitPin(GPIOPort port, uint8_t pin, GPIOInitType *params);

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Simple Interface Functions ******************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/* These functions should be implemented for your processor exactly as they are
written. I reason I do this is to increase speed a little bit. There is usually
only one GPIO peripheral on an MCU. Therefore, we shouldn't need multiple
instances of a GPIO object, so the functions can be called directly. */

/***************************************************************************//**
 * @brief Set a pin high
 * 
 * @param port  port identifier type
 * 
 * @param pin  the pin in the port (not the pin number on the schematic)
 */
void GPIO_SetPin(GPIOPort port, uint8_t pin);

/***************************************************************************//**
 * @brief Clear a pin
 * 
 * @param pin  a combined port/pin identifier type
 */
void GPIO_ClearPin(GPIOPort port, uint8_t pin);

/***************************************************************************//**
 * @brief Invert a pin's value
 * 
 * @param port  port identifier type
 * 
 * @param pin  the pin in the port (not the pin number on the schematic)
 */
void GPIO_InvertPin(GPIOPort port, uint8_t pin);

/***************************************************************************//**
 * @brief Read the output register for a pin
 * 
 * @param port  port identifier type
 * 
 * @param pin  the pin in the port (not the pin number on the schematic)
 * 
 * @return  true if the output is high
 */
bool GPIO_GetPinOutput(GPIOPort port, uint8_t pin);

/***************************************************************************//**
 * @brief Write a value to a pin
 * 
 * @param port  port identifier type
 * 
 * @param pin  the pin in the port (not the pin number on the schematic)
 * 
 * @param setPinHigh  true = high, false = low
 */
void GPIO_WritePin(GPIOPort port, uint8_t pin, bool setPinHigh);

/***************************************************************************//**
 * @brief Read the input value of a pin
 * 
 * @param port  port identifier type
 * 
 * @param pin  the pin in the port (not the pin number on the schematic)
 * 
 * @return bool  true = high, false = low
 */
bool GPIO_ReadPin(GPIOPort port, uint8_t pin);

/***************************************************************************//**
 * @brief Set the pin type
 * 
 * Set the pin's associated registers
 * 
 * @param port  port identifier type
 * 
 * @param pin  the pin in the port (not the pin number on the schematic)
 * 
 * @param type  GPIO_TYPE_ANALOG, GPIO_TYPE_DIGITAL_INPUT, 
 *              GPIO_TYPE_DIGITAL_OUTPUT, GPIO_TYPE_OPEN_DRAIN
 */
void GPIO_SetType(GPIOPort port, uint8_t pin, GPIOType type);

/***************************************************************************//**
 * @brief Get the type of pin
 * 
 * Read the pin's associated registers and determine the pin type
 * 
 * @param port  port identifier type
 * 
 * @param pin  the pin in the port (not the pin number on the schematic)
 * 
 * @return GPIOType  GPIO_TYPE_ANALOG, GPIO_TYPE_DIGITAL_INPUT, 
 *                   GPIO_TYPE_DIGITAL_OUTPUT, GPIO_TYPE_OPEN_DRAIN
 */
GPIOType GPIO_GetType(GPIOPort port, uint8_t pin);

/***************************************************************************//**
 * @brief Set the pull up resistors
 * 
 * Set the pin's associated registers
 * 
 * @param port  port identifier type
 * 
 * @param pin  the pin in the port (not the pin number on the schematic)
 * 
 * @param pullType  GPIO_PULL_UP, GPIO_PULL_DOWN, or GPIO_PULL_NONE
 */
void GPIO_SetPull(GPIOPort port, uint8_t pin, GPIOPull pullType);

/***************************************************************************//**
 * @brief Get the pull up resistors if any
 * 
 * Read the pin's associated registers and determine the pull up type
 * 
 * @param port  port identifier type
 * 
 * @param pin  the pin in the port (not the pin number on the schematic)
 * 
 * @return GPIOPull  GPIO_PULL_UP, GPIO_PULL_DOWN, or GPIO_PULL_NONE
 */
GPIOPull GPIO_GetPull(GPIOPort port, uint8_t pin);

/***************************************************************************//**
 * @brief Write the whole port's output register at once
 * 
 * @param port  the port name
 * 
 * @param portMask  the output value
 */
void GPIO_PortWrite(GPIOPort port, uint16_t portMask);

/***************************************************************************//**
 * @brief Read the whole port's output register
 * 
 * @param port  the port name
 * 
 * @return uint16_t  the current output value
 */
uint16_t GPIO_PortGetOutput(GPIOPort port);

/***************************************************************************//**
 * @brief Read the whole port's input register
 * 
 * @param port  the port name
 * 
 * @return uint16_t  the input value
 */
uint16_t GPIO_PortRead(GPIOPort port);


// ----- Pin Type Names --------------------------------------------------------

#define GPIO_NONE 0x0F, 0 // a default value you can use that doesn't exist
#define GPIO_PORTA_PIN0 GPIO_PORTA, 0
#define GPIO_PORTA_PIN1 GPIO_PORTA, 1
#define GPIO_PORTA_PIN2 GPIO_PORTA, 2
#define GPIO_PORTA_PIN3 GPIO_PORTA, 3
#define GPIO_PORTA_PIN4 GPIO_PORTA, 4
#define GPIO_PORTA_PIN5 GPIO_PORTA, 5
#define GPIO_PORTA_PIN6 GPIO_PORTA, 6
#define GPIO_PORTA_PIN7 GPIO_PORTA, 7
#define GPIO_PORTA_PIN8 GPIO_PORTA, 8
#define GPIO_PORTA_PIN9 GPIO_PORTA, 9
#define GPIO_PORTA_PIN10 GPIO_PORTA, 10
#define GPIO_PORTA_PIN11 GPIO_PORTA, 11
#define GPIO_PORTA_PIN12 GPIO_PORTA, 12
#define GPIO_PORTA_PIN13 GPIO_PORTA, 13
#define GPIO_PORTA_PIN14 GPIO_PORTA, 14
#define GPIO_PORTA_PIN15 GPIO_PORTA, 15
#define GPIO_PORTB_PIN0 GPIO_PORTB, 0
#define GPIO_PORTB_PIN1 GPIO_PORTB, 1
#define GPIO_PORTB_PIN2 GPIO_PORTB, 2
#define GPIO_PORTB_PIN3 GPIO_PORTB, 3
#define GPIO_PORTB_PIN4 GPIO_PORTB, 4
#define GPIO_PORTB_PIN5 GPIO_PORTB, 5
#define GPIO_PORTB_PIN6 GPIO_PORTB, 6
#define GPIO_PORTB_PIN7 GPIO_PORTB, 7
#define GPIO_PORTB_PIN8 GPIO_PORTB, 8
#define GPIO_PORTB_PIN9 GPIO_PORTB, 9
#define GPIO_PORTB_PIN10 GPIO_PORTB, 10
#define GPIO_PORTB_PIN11 GPIO_PORTB, 11
#define GPIO_PORTB_PIN12 GPIO_PORTB, 12
#define GPIO_PORTB_PIN13 GPIO_PORTB, 13
#define GPIO_PORTB_PIN14 GPIO_PORTB, 14
#define GPIO_PORTB_PIN15 GPIO_PORTB, 15
#define GPIO_PORTC_PIN0 GPIO_PORTC, 0
#define GPIO_PORTC_PIN1 GPIO_PORTC, 1
#define GPIO_PORTC_PIN2 GPIO_PORTC, 2
#define GPIO_PORTC_PIN3 GPIO_PORTC, 3
#define GPIO_PORTC_PIN4 GPIO_PORTC, 4
#define GPIO_PORTC_PIN5 GPIO_PORTC, 5
#define GPIO_PORTC_PIN6 GPIO_PORTC, 6
#define GPIO_PORTC_PIN7 GPIO_PORTC, 7
#define GPIO_PORTC_PIN8 GPIO_PORTC, 8
#define GPIO_PORTC_PIN9 GPIO_PORTC, 9
#define GPIO_PORTC_PIN10 GPIO_PORTC, 10
#define GPIO_PORTC_PIN11 GPIO_PORTC, 11
#define GPIO_PORTC_PIN12 GPIO_PORTC, 12
#define GPIO_PORTC_PIN13 GPIO_PORTC, 13
#define GPIO_PORTC_PIN14 GPIO_PORTC, 14
#define GPIO_PORTC_PIN15 GPIO_PORTC, 15
#define GPIO_PORTD_PIN0 GPIO_PORTD, 0
#define GPIO_PORTD_PIN1 GPIO_PORTD, 1
#define GPIO_PORTD_PIN2 GPIO_PORTD, 2
#define GPIO_PORTD_PIN3 GPIO_PORTD, 3
#define GPIO_PORTD_PIN4 GPIO_PORTD, 4
#define GPIO_PORTD_PIN5 GPIO_PORTD, 5
#define GPIO_PORTD_PIN6 GPIO_PORTD, 6
#define GPIO_PORTD_PIN7 GPIO_PORTD, 7
#define GPIO_PORTD_PIN8 GPIO_PORTD, 8
#define GPIO_PORTD_PIN9 GPIO_PORTD, 9
#define GPIO_PORTD_PIN10 GPIO_PORTD, 10
#define GPIO_PORTD_PIN11 GPIO_PORTD, 11
#define GPIO_PORTD_PIN12 GPIO_PORTD, 12
#define GPIO_PORTD_PIN13 GPIO_PORTD, 13
#define GPIO_PORTD_PIN14 GPIO_PORTD, 14
#define GPIO_PORTD_PIN15 GPIO_PORTD, 15
#define GPIO_PORTE_PIN0 GPIO_PORTE, 0
#define GPIO_PORTE_PIN1 GPIO_PORTE, 1
#define GPIO_PORTE_PIN2 GPIO_PORTE, 2
#define GPIO_PORTE_PIN3 GPIO_PORTE, 3
#define GPIO_PORTE_PIN4 GPIO_PORTE, 4
#define GPIO_PORTE_PIN5 GPIO_PORTE, 5
#define GPIO_PORTE_PIN6 GPIO_PORTE, 6
#define GPIO_PORTE_PIN7 GPIO_PORTE, 7
#define GPIO_PORTE_PIN8 GPIO_PORTE, 8
#define GPIO_PORTE_PIN9 GPIO_PORTE, 9
#define GPIO_PORTE_PIN10 GPIO_PORTE, 10
#define GPIO_PORTE_PIN11 GPIO_PORTE, 11
#define GPIO_PORTE_PIN12 GPIO_PORTE, 12
#define GPIO_PORTE_PIN13 GPIO_PORTE, 13
#define GPIO_PORTE_PIN14 GPIO_PORTE, 14
#define GPIO_PORTE_PIN15 GPIO_PORTE, 15
#define GPIO_PORTF_PIN0 GPIO_PORTF, 0
#define GPIO_PORTF_PIN1 GPIO_PORTF, 1
#define GPIO_PORTF_PIN2 GPIO_PORTF, 2
#define GPIO_PORTF_PIN3 GPIO_PORTF, 3
#define GPIO_PORTF_PIN4 GPIO_PORTF, 4
#define GPIO_PORTF_PIN5 GPIO_PORTF, 5
#define GPIO_PORTF_PIN6 GPIO_PORTF, 6
#define GPIO_PORTF_PIN7 GPIO_PORTF, 7
#define GPIO_PORTF_PIN8 GPIO_PORTF, 8
#define GPIO_PORTF_PIN9 GPIO_PORTF, 9
#define GPIO_PORTF_PIN10 GPIO_PORTF, 10
#define GPIO_PORTF_PIN11 GPIO_PORTF, 11
#define GPIO_PORTF_PIN12 GPIO_PORTF, 12
#define GPIO_PORTF_PIN13 GPIO_PORTF, 13
#define GPIO_PORTF_PIN14 GPIO_PORTF, 14
#define GPIO_PORTF_PIN15 GPIO_PORTF, 15
#define GPIO_PORTG_PIN0 GPIO_PORTG, 0
#define GPIO_PORTG_PIN1 GPIO_PORTG, 1
#define GPIO_PORTG_PIN2 GPIO_PORTG, 2
#define GPIO_PORTG_PIN3 GPIO_PORTG, 3
#define GPIO_PORTG_PIN4 GPIO_PORTG, 4
#define GPIO_PORTG_PIN5 GPIO_PORTG, 5
#define GPIO_PORTG_PIN6 GPIO_PORTG, 6
#define GPIO_PORTG_PIN7 GPIO_PORTG, 7
#define GPIO_PORTG_PIN8 GPIO_PORTG, 8
#define GPIO_PORTG_PIN9 GPIO_PORTG, 9
#define GPIO_PORTG_PIN10 GPIO_PORTG, 10
#define GPIO_PORTG_PIN11 GPIO_PORTG, 11
#define GPIO_PORTG_PIN12 GPIO_PORTG, 12
#define GPIO_PORTG_PIN13 GPIO_PORTG, 13
#define GPIO_PORTG_PIN14 GPIO_PORTG, 14
#define GPIO_PORTG_PIN15 GPIO_PORTG, 15
#define GPIO_PORTH_PIN0 GPIO_PORTH, 0
#define GPIO_PORTH_PIN1 GPIO_PORTH, 1
#define GPIO_PORTH_PIN2 GPIO_PORTH, 2
#define GPIO_PORTH_PIN3 GPIO_PORTH, 3
#define GPIO_PORTH_PIN4 GPIO_PORTH, 4
#define GPIO_PORTH_PIN5 GPIO_PORTH, 5
#define GPIO_PORTH_PIN6 GPIO_PORTH, 6
#define GPIO_PORTH_PIN7 GPIO_PORTH, 7
#define GPIO_PORTH_PIN8 GPIO_PORTH, 8
#define GPIO_PORTH_PIN9 GPIO_PORTH, 9
#define GPIO_PORTH_PIN10 GPIO_PORTH, 10
#define GPIO_PORTH_PIN11 GPIO_PORTH, 11
#define GPIO_PORTH_PIN12 GPIO_PORTH, 12
#define GPIO_PORTH_PIN13 GPIO_PORTH, 13
#define GPIO_PORTH_PIN14 GPIO_PORTH, 14
#define GPIO_PORTH_PIN15 GPIO_PORTH, 15
#define GPIO_PORTI_PIN0 GPIO_PORTI, 0
#define GPIO_PORTI_PIN1 GPIO_PORTI, 1
#define GPIO_PORTI_PIN2 GPIO_PORTI, 2
#define GPIO_PORTI_PIN3 GPIO_PORTI, 3
#define GPIO_PORTI_PIN4 GPIO_PORTI, 4
#define GPIO_PORTI_PIN5 GPIO_PORTI, 5
#define GPIO_PORTI_PIN6 GPIO_PORTI, 6
#define GPIO_PORTI_PIN7 GPIO_PORTI, 7
#define GPIO_PORTI_PIN8 GPIO_PORTI, 8
#define GPIO_PORTI_PIN9 GPIO_PORTI, 9
#define GPIO_PORTI_PIN10 GPIO_PORTI, 10
#define GPIO_PORTI_PIN11 GPIO_PORTI, 11
#define GPIO_PORTI_PIN12 GPIO_PORTI, 12
#define GPIO_PORTI_PIN13 GPIO_PORTI, 13
#define GPIO_PORTI_PIN14 GPIO_PORTI, 14
#define GPIO_PORTI_PIN15 GPIO_PORTI, 15
#define GPIO_PORTJ_PIN0 GPIO_PORTJ, 0
#define GPIO_PORTJ_PIN1 GPIO_PORTJ, 1
#define GPIO_PORTJ_PIN2 GPIO_PORTJ, 2
#define GPIO_PORTJ_PIN3 GPIO_PORTJ, 3
#define GPIO_PORTJ_PIN4 GPIO_PORTJ, 4
#define GPIO_PORTJ_PIN5 GPIO_PORTJ, 5
#define GPIO_PORTJ_PIN6 GPIO_PORTJ, 6
#define GPIO_PORTJ_PIN7 GPIO_PORTJ, 7
#define GPIO_PORTJ_PIN8 GPIO_PORTJ, 8
#define GPIO_PORTJ_PIN9 GPIO_PORTJ, 9
#define GPIO_PORTJ_PIN10 GPIO_PORTJ, 10
#define GPIO_PORTJ_PIN11 GPIO_PORTJ, 11
#define GPIO_PORTJ_PIN12 GPIO_PORTJ, 12
#define GPIO_PORTJ_PIN13 GPIO_PORTJ, 13
#define GPIO_PORTJ_PIN14 GPIO_PORTJ, 14
#define GPIO_PORTJ_PIN15 GPIO_PORTJ, 15
#define GPIO_PORTK_PIN0 GPIO_PORTK, 0
#define GPIO_PORTK_PIN1 GPIO_PORTK, 1
#define GPIO_PORTK_PIN2 GPIO_PORTK, 2
#define GPIO_PORTK_PIN3 GPIO_PORTK, 3
#define GPIO_PORTK_PIN4 GPIO_PORTK, 4
#define GPIO_PORTK_PIN5 GPIO_PORTK, 5
#define GPIO_PORTK_PIN6 GPIO_PORTK, 6
#define GPIO_PORTK_PIN7 GPIO_PORTK, 7
#define GPIO_PORTK_PIN8 GPIO_PORTK, 8
#define GPIO_PORTK_PIN9 GPIO_PORTK, 9
#define GPIO_PORTK_PIN10 GPIO_PORTK, 10
#define GPIO_PORTK_PIN11 GPIO_PORTK, 11
#define GPIO_PORTK_PIN12 GPIO_PORTK, 12
#define GPIO_PORTK_PIN13 GPIO_PORTK, 13
#define GPIO_PORTK_PIN14 GPIO_PORTK, 14
#define GPIO_PORTK_PIN15 GPIO_PORTK, 15
#define GPIO_PORTL_PIN0 GPIO_PORTL, 0
#define GPIO_PORTL_PIN1 GPIO_PORTL, 1
#define GPIO_PORTL_PIN2 GPIO_PORTL, 2
#define GPIO_PORTL_PIN3 GPIO_PORTL, 3
#define GPIO_PORTL_PIN4 GPIO_PORTL, 4
#define GPIO_PORTL_PIN5 GPIO_PORTL, 5
#define GPIO_PORTL_PIN6 GPIO_PORTL, 6
#define GPIO_PORTL_PIN7 GPIO_PORTL, 7
#define GPIO_PORTL_PIN8 GPIO_PORTL, 8
#define GPIO_PORTL_PIN9 GPIO_PORTL, 9
#define GPIO_PORTL_PIN10 GPIO_PORTL, 10
#define GPIO_PORTL_PIN11 GPIO_PORTL, 11
#define GPIO_PORTL_PIN12 GPIO_PORTL, 12
#define GPIO_PORTL_PIN13 GPIO_PORTL, 13
#define GPIO_PORTL_PIN14 GPIO_PORTL, 14
#define GPIO_PORTL_PIN15 GPIO_PORTL, 15
#define GPIO_PORTM_PIN0 GPIO_PORTM, 0
#define GPIO_PORTM_PIN1 GPIO_PORTM, 1
#define GPIO_PORTM_PIN2 GPIO_PORTM, 2
#define GPIO_PORTM_PIN3 GPIO_PORTM, 3
#define GPIO_PORTM_PIN4 GPIO_PORTM, 4
#define GPIO_PORTM_PIN5 GPIO_PORTM, 5
#define GPIO_PORTM_PIN6 GPIO_PORTM, 6
#define GPIO_PORTM_PIN7 GPIO_PORTM, 7
#define GPIO_PORTM_PIN8 GPIO_PORTM, 8
#define GPIO_PORTM_PIN9 GPIO_PORTM, 9
#define GPIO_PORTM_PIN10 GPIO_PORTM, 10
#define GPIO_PORTM_PIN11 GPIO_PORTM, 11
#define GPIO_PORTM_PIN12 GPIO_PORTM, 12
#define GPIO_PORTM_PIN13 GPIO_PORTM, 13
#define GPIO_PORTM_PIN14 GPIO_PORTM, 14
#define GPIO_PORTM_PIN15 GPIO_PORTM, 15
#define GPIO_PORTN_PIN0 GPIO_PORTN, 0
#define GPIO_PORTN_PIN1 GPIO_PORTN, 1
#define GPIO_PORTN_PIN2 GPIO_PORTN, 2
#define GPIO_PORTN_PIN3 GPIO_PORTN, 3
#define GPIO_PORTN_PIN4 GPIO_PORTN, 4
#define GPIO_PORTN_PIN5 GPIO_PORTN, 5
#define GPIO_PORTN_PIN6 GPIO_PORTN, 6
#define GPIO_PORTN_PIN7 GPIO_PORTN, 7
#define GPIO_PORTN_PIN8 GPIO_PORTN, 8
#define GPIO_PORTN_PIN9 GPIO_PORTN, 9
#define GPIO_PORTN_PIN10 GPIO_PORTN, 10
#define GPIO_PORTN_PIN11 GPIO_PORTN, 11
#define GPIO_PORTN_PIN12 GPIO_PORTN, 12
#define GPIO_PORTN_PIN13 GPIO_PORTN, 13
#define GPIO_PORTN_PIN14 GPIO_PORTN, 14
#define GPIO_PORTN_PIN15 GPIO_PORTN, 15

#endif  /* IGPIO_H */
