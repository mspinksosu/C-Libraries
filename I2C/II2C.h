/***************************************************************************//**
 * @brief Basic I2C Interface Header
 * 
 * @file IC21.h 
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 10/2/16   Original Creation
 * @date 2/21/22   Added Doxygen
 * @date 2/27/25   Refactored
 * 
 * @details
 *      @todo details. Reorganizing code based on my newer SPI manager code.
 *      master is only supported right now
 * 
 * @section license License
 * SPDX-FileCopyrightText: © 2016 Matthew Spinks
 * SPDX-License-Identifier: Zlib
 * 
 * This software is released under the Zlib license. You are free alter and
 * redistribute it, but you must not misrepresent the origin of the software.
 * This notice may not be removed. <http://www.zlib.net/zlib_license.html>
 * 
*******************************************************************************/

#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include <stdbool.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

typedef enum I2CRoleTag
{
    I2C_ROLE_UNKNOWN = 0,
    I2C_ROLE_MASTER,
    I2C_ROLE_SLAVE,
} I2CRole;

// @todo add modes if needed, or remove
typedef enum I2CModeTag
{
    I2C_MODE_0 = 0,
    I2C_MODE_1,
    I2C_MODE_2,
    I2C_MODE_3
} I2CMode;

// @todo make error codes enum?

// @todo add status bits if needed for the peripheral level
typedef struct I2CStatusBitsTag
{
    union {
        struct {
            unsigned busy             :1;
            unsigned txEmpty          :1; // tx register empty
            unsigned rxNotEmpty       :1; // rx register not empty
            unsigned transmitFinished :1;
            unsigned fault            :1; // mode fault or frame error
            unsigned overflow         :1;
            unsigned                  :2;
        };
        uint8_t all;
    };
} I2CStatusBits;

typedef struct I2CInitTypeTag
{
    I2CRole role;
    I2CMode mode;
    bool useTxInterrupt;
    bool useRxInterrupt;
} I2CInitType;

typedef struct I2CInterfaceTag
{
    /*  These are the functions that will be called. You will create your own
    interface object for your class that will have these function signatures.
    Set each of your functions equal to one of these pointers */
    void (*I2C_Init)(I2CInitType *params);
    void (*I2C_Enable)(void);
    void (*I2C_Disable)(void);

    // add more
} I2CInterface;

typedef struct I2CTag
{
    I2CInterface *interface;
} I2C;

/** 
 * Description of struct members: // TODO description
 * 
 * member1      description of variable member1
 * 
 */

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

// @todo make calculate BRG function
// @todo add more functions

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////



#endif /* I2C_H */
