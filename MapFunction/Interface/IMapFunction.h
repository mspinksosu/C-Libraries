/***************************************************************************//**
 * @brief Map Function Interface Header File
 * 
 * @file IMapFunction.h
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 12/18/21  Original creation
 * 
 * @details
 *      An interface for different types of map functions. A map function is
 * simply a function that converts one range of values to another range of 
 * values by any means. The original map function equation I used was just an 
 * equation for linear interpolation. I had used it many, many years ago with 
 * the Arduino "map()" function. Hence the name. I used the linear equation 
 * and created my own little ecosystem around to allow different kinds of map 
 * functions to be used. 
 * 
 * Objects that use this interface must implement the functions listed in the 
 * interface. To make a MapFunction, you will need to make a base class 
 * MapFunction object and a subclass MapFunction object. The base class 
 * contains at minimum, a pointer to the sub class's interface and a void 
 * pointer called "instance". Your subclass function will use your subclass 
 * type object, but the function calls will be using the base class type. The 
 * reason the void pointer is used is so that the user does not have to deal 
 * with typecasting every function call. The void pointer will be changed to 
 * point to your subclass object. 
 * 
 * The MFInterface or function table will tell the interface which functions 
 * to call. Your implementation of the MapFunction will have its own functions
 * that use your subclass type as arguments. Declare and initialize a 
 * MFInterface object and set its members (which are function pointers) to 
 * each of your functions. References to the base class are replaced with void.
 * You will need to typecast your functions for this step.
 * 
 * A sub class will contain at minimum, a pointer to the base class named 
 * "super". After creating a sub class, it needs to be connected to the base 
 * class by using the MF_Create function. This function will set the void 
 * pointer for you. I think the best way to do this is to create your own sub
 * class create function that is not part of the interface, and have it accept
 * both the base class and sub class as arguments. Then, inside that function
 * set the "super" pointer and any other variables. Then call MF_Create last.
 * Doing it this way makes the process a little more type-safe by ensuring the
 * void pointer doesn't get set incorrectly.
 * 
 * @section example_code Example Code
 *      MapFunction mapFunction;
 *      SubClassMap linearMapType;
 *      SubClassMap_Create(&mapFunction, &linearMapType, param1, ...)
 *      output = MF_Compute(&mapFunction, input);
 * 
 * @section license License
 * SPDX-FileCopyrightText: © 2021 Matthew Spinks
 * SPDX-License-Identifier: Zlib
 * 
 * This software is released under the Zlib license. You are free alter and
 * redistribute it, but you must not misrepresent the origin of the software.
 * This notice may not be removed. <http://www.zlib.net/zlib_license.html>
 * 
 ******************************************************************************/

#ifndef IMAP_FUNCTION_H
#define IMAP_FUNCTION_H

#include <stdint.h>
#include <stddef.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

typedef struct MFInterfaceTag
{
    /*  These are the functions that will be called. Each object type will have
        its own implementation. The void pointer "instance" will point to the
        child object */ 
    int32_t (*Compute)(void *instance, int32_t input);

} MFInterface;

typedef struct MapFunctionTag
{
    /*  A base class must contain at minimum, a void pointer and a pointer to 
    the sub class's interface */
    void *instance;
    MFInterface *interface;
} MapFunction;

/**
 * MFInterface This is the table of functions that need to be implemented
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

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Combine the base class, sub class, and function table
 * 
 * Links the instance pointer in the base class to the sub class. Because of 
 * the void pointer, my preferred method is to call this function from a sub
 * class constructor. I created a sub class constructor that needs an instance 
 * of the sub class and base class. This makes the create function more type
 * safe.
 * 
 * @param self  pointer to the MapFunction that you are using
 * 
 * @param instanceOfSubClass  the child object that implements the MapFunction
 * 
 * @param interface  the interface or function table to use
 */
void MF_Create(MapFunction *self, void *instanceOfSubClass, MFInterface *interface);

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Call the compute function.
 * 
 * What you are actually doing is calling the compute function of the sub class
 * 
 * @param self  pointer to the MapFunction that you are using
 * 
 * @param input  input to the map function
 * 
 * @return int32_t  output of the function
 */
int32_t MF_Compute(MapFunction *self, int32_t input);

#endif	/* IMAP_FUNCTION_H */