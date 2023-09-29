/***************************************************************************//**
 * @brief GPIO Library Manager Header (Non-Processor Specific)
 * 
 * @file GPIO_Manager.h
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 7/24/22   Original creation
 * 
 * @details
 *     This is a simple piece of code that handles initializing all of the pins 
 * on the MCU. It also handles access to the GPIO pins for other files.
 * 
 * The code in the GPIO library should really only deal with modifying the 
 * pins. It should not be concerned with how the user manages their pin 
 * organization. Furthermore, The GPIO implementation will always have some 
 * sort of processor specific dependency. So if the user were to include the
 * header for that file they wouldn't be able to port their code as easily to 
 * a different processor.
 * 
 * Calls to the GPIO functions are done with the base GPIO type. Because of 
 * this, functions that may need access to the pins only need to use the base 
 * class object.
 * 
 * The first step in the initialization process is to set the GPIO driver
 * interface or function table by calling GPIO_SetDriverInterface. This table 
 * is created inside a processor specific GPIO library, which is library that 
 * implements the functions listed in IGPIO.h. After that, each pins properties
 * can be set and the GPIO_Init function can be called once for every pin that 
 * is needed.
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

#ifndef GPIO_MANAGER_H
#define GPIO_MANAGER_H

#include "IGPIO.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Function Prototypes *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Set the GPIO driver interface and initialize all of the pins
 * 
 * Each pin has to be declared as a global variable. However, only the base
 * class GPIO needs to be extern. Anything that needs to access these pins
 * will include this header file which has the extern declaration.
 */
void GPIO_Manager_InitAllPins(void);

#endif  /* GPIO_MANAGER_H */