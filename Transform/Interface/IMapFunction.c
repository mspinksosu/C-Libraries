/***************************************************************************//**
 * @brief Map Function Interface
 * 
 * @author Matthew Spinks
 * 
 * @date 12/18/21    Original creation
 * 
 * @file IMapFunction.c
 * 
 * @details
 *      The functions here are called using the base class object type. The 
 * actual function calls are done using indirection with the pointers inside 
 * the c-structs. When you call the create functions, you are linking these 
 * pointers to your subclass. These functions will use the pointer to the
 * interface to determine which implementation to call and give it the pointer
 * to the subclass object (instance).
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