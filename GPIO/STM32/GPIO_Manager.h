/***************************************************************************//**
 * @brief GPIO Library Manager Header (Non-Processor Specific)
 * 
 * @author Matthew Spinks
 * 
 * @date 7/24/22   Original creation
 * 
 * @file GPIO_Manager.h
 * 
 * @details
 *     I removed the initialization function that was in the GPIO library and 
 * put it here. Code in the GPIO library should only be concerned with pins. 
 * It shouldn't care how the user initializes all their pins. This gives more 
 * flexibility with how we decide to handle pin organization.
 * 
 ******************************************************************************/

#ifndef GPIO_MANAGER_H
#define GPIO_MANAGER_H

#include "IGPIO.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/* Any file that includes this header can have access to these objects to use 
with the GPIO library. Only the GPIO type needs to be external. Each external 
variable will need a matching declaration and initialization. This will be done 
in your processor specific GPIO implementation. */

// ----- Declare GPIO pins starting here. Declare as extern  -------------------

extern GPIO pin1, pin2;


// ***** Function Prototypes ***************************************************

void GPIO_Manager_InitAllPins(void);

#endif  /* GPIO_MANAGER_H */