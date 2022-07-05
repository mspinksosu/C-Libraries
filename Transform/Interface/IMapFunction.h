/***************************************************************************//**
 * @brief Map Function Interface Header File
 * 
 * @author Matthew Spinks
 * 
 * @date December 20, 2021  Original creation
 * 
 * @file IMapFunction.h
 * 
 * @details
 *      An interface for different types of map functions. Objects that 
 * use this interface must implement the functions listed in the interface. A 
 * map function in this case is simply a function to convert a range of numbers 
 * from one range to another, either by linear interpolation or other means.
 * 
 * A base class must contain at minimum, a pointer to the sub class's interface 
 * and a void pointer. The void pointer "instance" will point to whatever sub 
 * class object is created. Sub class types will need their own different 
 * variables. This is why a void pointer is used. After creating the sub class, 
 * the void pointer will be changed to point to it. 
 * 
 * The MF_Interface or function table tells the interface what functions to 
 * call. When we create the function table, we are initializing its members 
 * (which are function pointers) the our local functions. Typecasting is 
 * necessary. When a new sub class object is created, its interface member is 
 * set to this table.
 * 
 ******************************************************************************/

#ifndef IMAP_FUNCTION_H
#define IMAP_FUNCTION_H

#include <stdint.h>
#include <stddef.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

typedef struct MF_InterfaceTag
{
    /*  These are the functions that will be called. Each object type will have
        its own implementation. The void pointer "instance" will point to the
        child object */ 
    int32_t (*Compute)(void *instance, int32_t input);
    // Add more functions below
} MF_Interface;

typedef struct MapFunctionTag
{
    /*  A base class must contain at minimum, a pointer to the sub class's 
        interface and a void pointer. */
    MF_Interface *interface;
    void *instance;
    // Add any more base class specific variables
} MapFunction;

/**
 * MF_Interface This is the table of functions that need to be implemented
 * 
 * MapFunction  This is the base class object
 * 
 * interface    A pointer to the interface that will be used. This will be set
 *              to the function table declared in the sub class, by means of
 *              of the base class create function
 * 
 * instance     A pointer to the instance of the sub class being used. This
 *              will be set by means of the base class create function 
 */

// ***** Function Prototypes ***************************************************

void MF_Create(MapFunction *self, void *instanceOfSubClass, MF_Interface *interface);

int32_t MF_Compute(MapFunction *self, int32_t input);

#endif	/* IMAP_FUNCTION_H */