/***************************************************************************//**
 * @brief I2C Manager (Non-Processor Specific)
 * 
 * @file I2C_Manager.c
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 2/27/25   Original creation
 * 
 * @details
 *      // @todo details
 * 
 * @section license License
 * SPDX-FileCopyrightText: © 2025 Matthew Spinks
 * SPDX-License-Identifier: Zlib
 * 
 * This software is released under the Zlib license. You are free alter and
 * redistribute it, but you must not misrepresent the origin of the software.
 * This notice may not be removed. <http://www.zlib.net/zlib_license.html>
 * 
 ******************************************************************************/

#include "I2C_Manager.h"
#include <stddef.h> // needed for NULL

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

static bool I2CManagerEnabled; // @todo enable/disable

// ***** Static Function Prototypes ********************************************

static void I2C_Manager_DevicePush(I2CSlave *self, I2CSlave *endOfList);

// *****************************************************************************

void I2C_Manager_Create(I2CManager *self, I2C *peripheral)
{
    self->peripheral = peripheral;
    self->endOfList = NULL;
    self->device = NULL;
    self->busy = false; // @todo busy flag isn't used right now
}

// *****************************************************************************

void I2C_Manager_AddSlave(I2CManager *self, I2CSlave *slave, uint8_t *writeBuffer, uint8_t *readBuffer)
{
    slave->manager = self;
    slave->writeBuffer = writeBuffer;
    slave->readBuffer = readBuffer;
    slave->numBytesToRead = 0;
    slave->numBytesToSend = 0;
    slave->readCount = 0;
    slave->writeCount = 0;
    slave->state = I2C_STATE_IDLE;
    slave->transferFinished = false;

    if(self->endOfList == NULL)
    {
        /* Begin with a new list */
        self->endOfList = slave;

        /* Since the list only contains one entry, the "next" pointer will
        also point to itself */
        self->endOfList->next = self->endOfList;
    }
    else
    {
        I2C_Manager_DevicePush(slave, self->endOfList);
    }
    self->device = self->endOfList->next; // reset the index
}

// *****************************************************************************

void I2C_Manager_Process(I2CManager *self)
{


}

// *****************************************************************************

void I2C_Manager_Enable(I2CManager *self)
{
    if(!I2C_IsEnabled())
        I2C_Enable(self->peripheral);
    
    I2CManagerEnabled = true;
}

// *****************************************************************************

void I2C_Manager_Disable(I2CManager *self)
{
    I2CManagerEnabled = false;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Local Functions *****************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

static void I2C_Manager_DevicePush(I2CSlave *self, I2CSlave *endOfList)
{
    /* Add the new entry to the beginning of the list. Make the "next" pointer
    point to the head */
    self->next = endOfList->next;
    /* Update the beginning of the list to point to the new beginning */
    endOfList->next = self;
}

/*
 End of File
 */