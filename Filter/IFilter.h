/***************************************************************************//**
 * @brief Filter Interface Header File
 * 
 * @author Matthew Spinks
 * 
 * @date 12/2/14   Original creation
 * 
 * @file IFilter.h
 * 
 * @details
 *      TODO
 * 
 ******************************************************************************/

#ifndef IFILTER_H
#define IFILTER_H

#include <stdint.h>
#include <stdbool.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/* Create the function table */
typedef struct FilterInterfaceTag
{
    /* These are the functions that will be called. You will create your own
    interface object for your class that will have these function signatures.
    Set each of your functions equal to one of these pointers. The void pointer
    will be set to the sub class object. Typecasting will be needed. */
    uint16_t (*Filter_ComputeU16)(void *instance, uint16_t input);

    // TODO add float?
} FilterInterface;

/* Create the base class */
typedef struct FilterTag
{
    FilterInterface *interface;
    void *instance;

    // Add more necessary members
} Filter;

/**
 * Description of struct members:
 * 
 * FilterInterface  The table of functions that need to be implemented
 * 
 * Filter       The base class object
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
void Filter_Create(Filter *self, void *instanceOfSubclass, FilterInterface *interface);

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Compute the output of the filter with a given input
 * 
 * This function will need to be called periodically. How often you call it
 * is up to you.
 * 
 * @param self  pointer to the Filter you are using
 * 
 * @param input  input to the filter
 * 
 * @return uint16_t  output of the filter
 */
uint16_t Filter_ComputeU16(Filter *self, uint16_t input);

#endif  /* IFILTER_H */