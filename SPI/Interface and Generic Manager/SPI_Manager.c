/***************************************************************************//**
 * @brief SPI Manager Header (Non-Processor Specific)
 * 
 * @file SPI_Manager.h
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 8/28/22   Original creation
 * 
 * @details
 *      // TODO details. Right now, this handles master only
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

#include "SPI_Manager.h"
#include <stddef.h> // needed for NULL

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

static bool spiManagerEnabled; // TODO enable/disable

// ***** Static Function Prototypes ********************************************

static void SPI_Manager_DevicePush(SPISlave *self, SPISlave *endOfList);

// *****************************************************************************

void SPI_Manager_Create(SPIManager *self, SPI *peripheral)
{
    self->peripheral = peripheral;
    self->endOfList = NULL;
    self->device = NULL;
    self->busy = false; // TODO busy flag isn't used right now
}

// *****************************************************************************

void SPI_Manager_AddSlave(SPIManager *self, SPISlave *slave, uint8_t *writeBuffer, uint8_t *readBuffer)
{
    slave->manager = self;
    slave->writeBuffer = writeBuffer;
    slave->readBuffer = readBuffer;
    slave->numBytesToRead = 0;
    slave->numBytesToSend = 0;
    slave->readWriteCount = 0;
    slave->state = SPI_STATE_IDLE;
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
        SPI_Manager_DevicePush(slave, self->endOfList);
    }
    self->device = self->endOfList->next; // reset the index
}

// *****************************************************************************

bool SPI_Manager_IsDeviceBusy(SPISlave *self)
{
    if(self->state == SPI_STATE_IDLE)
        return false;
    else
        return true;
}

// *****************************************************************************

void SPI_Manager_BeginTransfer(SPISlave *self, uint16_t numBytesToSend, uint16_t numBytesToRead)
{
    if(self->state != SPI_STATE_IDLE || (numBytesToRead == 0 && numBytesToSend == 0))
        return;

    if(self->writeBuffer != NULL)
        self->numBytesToSend = numBytesToSend;

    if(self->readBuffer != NULL)
        self->numBytesToRead = numBytesToRead;

    self->transferFinished = false;
    self->state = SPI_STATE_RQ_START; // request start
}

// *****************************************************************************

bool SPI_Manager_IsTransferFinished(SPISlave *self)
{
    return self->transferFinished;
}

// *****************************************************************************

void SPI_Manager_Process(SPIManager *self)
{
    /* Go round-robin through the list of devices. Right now, I'm only going to 
    deal with SPI master mode. */
    // TODO add check for master mode, and eventually add slave mode
    // TODO busy flag isn't used yet. I may want to replace it with a state for the peripheral instead
    if(self->device != NULL)
    {
        switch(self->device->state)
        {
            case SPI_STATE_RQ_START:
                /* Begin transfer. Set slave select line low */
                if(self->device->SetSSPin != NULL)
                    (self->device->SetSSPin)(false, self->device);
                self->device->state = SPI_STATE_SEND_BYTE;
                break;
            case SPI_STATE_SEND_BYTE:
                if(self->device->readWriteCount < self->device->numBytesToSend)
                {
                    SPI_TransmitByte(self->peripheral, 
                        self->device->writeBuffer[self->device->readWriteCount]);
                }
                else
                {
                    /* Send empty data out for a slave read */
                    SPI_TransmitByte(self->peripheral, 0);
                }
                self->device->state = SPI_STATE_RECEIVE_BYTE;
                break;
            case SPI_STATE_RECEIVE_BYTE:
                /* In master mode there is always a receive after a send, so 
                I can use a single read/write index count. However, we do have 
                to wait until the transmission is fully finished first before 
                reading the data. If there is no master input, the get received 
                byte function will just return zero. Which is what it would do 
                if there was no data anyway. */

                /* TODO try by just checking using the getreceivedbyte function 
                by itself. As long as RXNE is cleared beforehand, we should be 
                able to just watch it. */

                /* Add option for no read buffer */
                if(SPI1_IsReceiveRegisterFull(self->peripheral))
                {
                    uint8_t data = SPI_GetReceivedByte(self->peripheral);

                    if(self->device->readWriteCount < self->device->numBytesToRead)
                    {
                        self->device->readBuffer[self->device->readWriteCount] = data;
                    }
                    self->device->readWriteCount++;
                    
                    if(self->device->readWriteCount < self->device->numBytesToSend ||
                        self->device->readWriteCount < self->device->numBytesToRead)
                    {
                        /* There are more bytes to send */
                        self->device->state = SPI_STATE_SEND_BYTE;
                    }
                    else
                    {
                        /* Transfer finished */
                        if(self->device->SetSSPin != NULL)
                            (self->device->SetSSPin)(true, self->device);
                        
                        self->device->transferFinished = true;
                        self->device->state = SPI_STATE_IDLE;
                        self->device = self->device->next;
                    }
                }
                // TODO get received byte try again count?
                break;
        } // end switch
    }
    else
    {
        /* No device found */
        self->device = self->device->next;
    }
}

// *****************************************************************************

void SPI_Manager_Enable(SPIManager *self)
{
    if(!SPI_IsEnabled())
        SPI_Enable(self->peripheral);
    
    spiManagerEnabled = true;
}

// *****************************************************************************

void SPI_Manager_Disable(SPIManager *self)
{
    spiManagerEnabled = false;
}

// *****************************************************************************

void SPI_Manager_SetSSPinFunc(SPISlave *self, void (*Function)(bool setPinHigh, void *slaveContext))
{
    self->SetSSPin = Function;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Local Functions *****************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

static void SPI_Manager_DevicePush(SPISlave *self, SPISlave *endOfList)
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