/***************************************************************************//**
 * @brief Map Function Interface
 * 
 * @file IMapFunction.c
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 12/18/21  Original creation
 * 
 * @details
 *      The functions here are called using the base class object type. The 
 * actual function calls are done using indirection with the pointers inside 
 * the c-structs. When you call the create functions, you are linking these 
 * pointers to your subclass. These functions will use the pointer to the
 * interface to determine which implementation to call and give it the pointer
 * to the subclass object (instance).
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

#include "IMapFunction.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************


// ***** Static Function Prototypes ********************************************


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void MF_Create(MapFunction *self, void *instanceOfSubClass, MFInterface *interface)
{
    self->instance = instanceOfSubClass;
    self->interface = interface;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

int32_t MF_Compute(MapFunction *self, int32_t input)
{
    if(self->interface->Compute != NULL && self->instance != NULL)
    {
        /* Dispatch the function using indirection */
        return (self->interface->Compute)(self->instance, input);
    }
    else
    {
        return 0;
    }
}

/*
 End of File
 */