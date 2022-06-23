/***************************************************************************//**
 * @brief Foo Interface Header File
 * 
 * @author Matthew Spinks
 * 
 * @date 12/2/14  Original creation
 * @date 2/4/22   Modified
 * 
 * @file Foo.h
 * 
 * @details
 *      TODO
 * 
 ******************************************************************************/

#ifndef FOO_H
#define FOO_H

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

// ***** Function Prototypes ***************************************************

void Foo_Create(Foo *self, void *instanceOfSubclass, Foo_Interface *interface);

// ----- Interface Functions ---------------------------------------------------

void Foo_Func(Foo *self);

uint16_t Foo_GetValue(Foo *self);

void Foo_SetValue(Foo *self, uint16_t data);

#endif  /* FOO_H */