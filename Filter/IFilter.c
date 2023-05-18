/***************************************************************************//**
 * @brief Filter Interface
 * 
 * @file IFilter.c
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 12/18/22  Original creation
 * 
 * @details
 *      TODO
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

#include "IFilter.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************


// ***** Static Function Prototypes ********************************************


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void Filter_Create(Filter *self, void *instanceOfSubclass, FilterInterface *interface)
{
    self->instance = instanceOfSubclass;
    self->interface = interface;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

uint16_t Filter_ComputeU16(Filter *self, uint16_t input)
{
    if(self->interface->Filter_ComputeU16 != NULL && self->instance != NULL)
    {
        /* Dispatch the function using indirection */
        return (self->interface->Filter_ComputeU16)(self->instance, input);
    }
    else
    {
        return 0;
    }
}

/*
 End of File
 */