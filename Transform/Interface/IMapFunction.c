/***************************************************************************//**
 * @brief Map Function Interface
 * 
 * @author Matthew Spinks
 * 
 * @date December 20, 2021  Original creation
 * 
 * @file IMapFunction.c
 * 
 * @details
 *      An interface for different types of map functions. Objects that 
 * use this interface must implement the functions listed in the interface. A 
 * map function in this case is simply a function to convert a range of numbers 
 * from one range to another, either by linear interpolation or other means.
 * 
 * After creating a sub class, it needs to be connected to the base class by
 * using the MF_Create function. The best way to do this is call this function
 * from the sub class's create function. This makes so the user doesn't need to
 * deal with messing around with void pointers, and removes one more step from
 * their process. The void pointer is used point to an instance of the subclass 
 * object. Otherwise, the user would have to do some ugly typecasting.
 * 
 * The actual function calls are done using indirection with pointers inside 
 * the c-struct objects. The functions here will use the base class object
 * type. The functions implemented in the sub classes will use the sub class
 * object types. This is where the linking from the sub class to base class
 * becomes important. Doing this allows the user to call the base class 
 * function and the object itself determines which implementation to call.
 * 
 * There is one small caveat. I do not use malloc for my micro, therefore the
 * user will need to declare a base class object and a sub class object before
 * calling the sub class create function.
 * 
 * Example Code:
 *      MapFunction baseTriggerMap;
 *      SubClassMap subTriggerMap;
 *      SubClassMap_Create(&baseTriggerMap, &subTriggerMap, subClassParam1, ...)
 *       
 ******************************************************************************/

#include "IMapFunction.h"

// ***** Defines ***************************************************************


// ***** Function Prototypes ***************************************************


// ***** Global Variables ******************************************************


/***************************************************************************//**
 * @brief Combine the base class, sub class, and function table
 * 
 * Links the instance pointer in the base class to the sub class. Because of 
 * the void pointer, my preferred method is to call this function from the sub
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
void MF_Create(MapFunction *self, void *instanceOfSubClass, MF_Interface *interface)
{
    self->instance = instanceOfSubClass;
    self->interface = interface;
}

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