/***************************************************************************//**
 * @brief Foo Interface Header File
 * 
 * @author Matthew Spinks
 * 
 * @date 12/2/14  Original creation
 * @date 8/6/22   Modified
 * 
 * @file IFoo.h
 * 
 * @details
 *      TODO
 * 
 ******************************************************************************/

#ifndef IFOO_H
#define IFOO_H

#include <stdint.h>
#include <stdbool.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/* Create the function table */
typedef struct Foo_InterfaceTag
{
    /*  These are the functions that will be called. You will create your own
        function table for your class that will have these function signatures 
        Replace references to the base class with void. The void pointer
        "instance" will be set to the sub class object */
    void (*Foo_Func)(void *instance);
    uint16_t (*Foo_GetValue)(void *instance);
    void (*Foo_SetValue)(void *instance, uint16_t data);

    // Add more functions below
} Foo_Interface;

/* Create the base class */
typedef struct FooTag
{
    Foo_Interface *interface;
    void *instance;

    // Add more necessary members
} Foo;

/**
 * Foo_Interface The table of functions that need to be implemented
 * 
 * Foo          The base class object
 * 
 * interface    A pointer to the interface that will be used. This will be set
 *              to the function table declared in the sub class, by means of
 *              of the base class create function
 * 
 * instance     A pointer to the instance of the sub class being used. This
 *              will be set by means of the base class create function
 * 
 */

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Combine the base class, sub class, and function table
 * 
 * This function shouldn't be called directly. It is preferred that it is 
 * called from within the sub class constructor. This makes the function more 
 * type safe with the use of the void pointer
 * 
 * @param self 
 * @param instanceOfSubClass 
 * @param interface 
 */
void Foo_Create(Foo *self, void *instanceOfSubclass, Foo_Interface *interface);

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