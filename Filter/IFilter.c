/***************************************************************************//**
 * @brief Foo Interface
 * 
 * @author Matthew Spinks
 * 
 * @date 12/18/22  Original creation
 * 
 * @file IFilter.c
 * 
 * @details
 *      TODO
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