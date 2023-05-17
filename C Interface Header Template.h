/***************************************************************************//**
 * @brief Foo Interface Header File
 * 
 * @file IFoo.h
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 12/2/14   Original creation
 * @date 8/6/22    Modified
 * 
 * @details
 *      TODO
 * 
 * @section license License
 * SPDX-FileCopyrightText: © 2019 Matthew Spinks
 * SPDX-License-Identifier: Zlib
 * 
 * This software is released under the Zlib license. You are free alter and
 * redistribute it, but you must not misrepresent the origin of the software.
 * This notice may not be removed. <http://www.zlib.net/zlib_license.html>
 * 
 ******************************************************************************/

#ifndef IFOO_H
#define IFOO_H

#include <stdint.h>
#include <stdbool.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/* Create the function table */
typedef struct FooInterfaceTag
{
    /* These are the functions that will be called. You will create your own
    interface object for your class that will have these function signatures.
    Set each of your functions equal to one of these pointers. The void pointer
    will be set to the sub class object. Typecasting will be needed. */
    void (*Foo_Func)(void *instance);
    uint16_t (*Foo_GetValue)(void *instance);
    void (*Foo_SetValue)(void *instance, uint16_t data);

    // Add more functions below
} FooInterface;

/* Create the base class */
typedef struct FooTag
{
    FooInterface *interface;
    void *instance;

    // Add more necessary members
} Foo;

/**
 * Description of struct members:
 * 
 * FooInterface The table of functions that need to be implemented
 * 
 * Foo          The base class object
 * 
 * interface    A pointer to the interface that will be used. This will be set
 *              to the function table declared in the sub class, by means of
 *              of the base class create function
 * 
 * instance     A pointer to the instance of the sub class being used. This
 *              will be set by means of the base class create function
 */

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Combine the base class, sub class, and function table
 * 
 * My preferred method is to call this function from a sub class constructor. 
 * This makes the create function more type safe. To create a sub class 
 * constructor, make your own Create function that takes your sub class and 
 * base class as arguments. Set your variables then call this function from it.
 * 
 * @param self 
 * @param instanceOfSubClass 
 * @param interface 
 */
void Foo_Create(Foo *self, void *instanceOfSubclass, FooInterface *interface);

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 */
void Foo_Func(Foo *self);

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 * @return uint16_t 
 */
uint16_t Foo_GetValue(Foo *self);

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 * @param data 
 */
void Foo_SetValue(Foo *self, uint16_t data);

#endif  /* IFOO_H */