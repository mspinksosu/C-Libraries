/***************************************************************************//**
 * @brief SPI Manager Header (Non-Processor Specific)
 * 
 * @author Matthew Spinks
 * 
 * @date 8/28/22  Original creation
 * 
 * @file SPI_Manager.h
 * 
 * @details
 *      // TODO
 * 
 ******************************************************************************/

#include "SPI_Manager.h"
#include <stddef.h> // needed for NULL

// ***** Defines ***************************************************************

#define DEFAULT_NUM_PERIPHERALS 4

// ***** Global Variables ******************************************************


// ***** Static Function Prototypes ********************************************

/* Put static function prototypes here */
static void SPI_Manager_DevicePush(SPISlave *self, SPISlave *endOfList);

// *****************************************************************************

void SPI_Manager_Init(SPIManager *self)
{

    /* Initialize the SPI peripherals */

}

// *****************************************************************************

void SPI_Manager_Create(SPIManager *self, SPI *peripheral)
{
    self->peripheral = peripheral;
    self->endOfList = NULL;
    self->busy = false;
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
    slave->state = SPI_SS_IDLE;
    slave->transferFinished = false;

    if(self->endOfList == NULL)
    {
        /* Begin with a new list */
        //ptrToLastDevice = self;
        self->endOfList = slave;

        /* Since the list only contains one entry, the "next" pointer will
        also point to itself */
        self->endOfList->next = self->endOfList;
    }
    else
    {
        SPI_Manager_DevicePush(slave, self->endOfList);
    }
    self->currentDevice = self->endOfList->next;
}

// *****************************************************************************

bool SPI_Manager_IsDeviceBusy(SPISlave *self)
{
    if(self->state == SPI_SS_IDLE)
        return false;
    else
        return true;
}

// *****************************************************************************

void SPI_Manager_BeginTransfer(SPISlave *self, uint16_t numBytesToSend, uint16_t numBytesToRead)
{
     // TODO
    if(self->state != SPI_SS_IDLE || (numBytesToRead == 0 && numBytesToSend == 0))
        return;

    if(numBytesToSend > 0 && self->writeBuffer != NULL)
        self->numBytesToSend = numBytesToSend;

    if(numBytesToRead > 0 && self->readBuffer != NULL)
        self->numBytesToRead = numBytesToRead;

    self->transferFinished = false;
    self->state = SPI_SS_RQ_START; // request start
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
    // TODO I may want to replace "busy" a state for the peripheral
    if(self->busy == false && self->currentDevice != NULL)
    {
        switch(self->currentDevice->state)
        {
            case SPI_SS_RQ_START:
                /* Begin transfer. Set slave select line low */
                if(self->currentDevice->SetSSPin != NULL)
                    (self->currentDevice->SetSSPin)(false, self->currentDevice);
                self->currentDevice->state = SPI_SS_SEND_BYTE;
                break;
            case SPI_SS_SEND_BYTE:
                if(self->currentDevice->readWriteCount < self->currentDevice->numBytesToSend)
                {
                    SPI_TransmitByte(self->peripheral, 
                        self->currentDevice->writeBuffer[self->currentDevice->readWriteCount]);
                }
                else
                {
                    /* Send empty data out for a slave read */
                    SPI_TransmitByte(self->peripheral, 0);
                }
                self->currentDevice->state = SPI_SS_SEND_BYTE;
                break;
            case SPI_SS_RECEIVE_BYTE:
                /* In master mode, there is always a read after a write. */
                if(SPI_IsTransmitRegisterEmpty(self->peripheral))
                {
                    uint8_t data = SPI_GetReceivedByte(self->peripheral);

                    if(self->currentDevice->readWriteCount < self->currentDevice->numBytesToRead)
                    {
                        self->currentDevice->readBuffer[self->currentDevice->readWriteCount] = data;
                    }
                    self->currentDevice->readWriteCount++;
                    
                    if(self->currentDevice->readWriteCount < self->currentDevice->numBytesToSend ||
                        self->currentDevice->readWriteCount < self->currentDevice->numBytesToRead)
                    {
                        /* There are more bytes to send */
                        self->currentDevice->state = SPI_SS_SEND_BYTE;
                    }
                    else
                    {
                        /* Set slave select line high */
                        if(self->currentDevice->SetSSPin != NULL)
                            (self->currentDevice->SetSSPin)(true, self->currentDevice);
                        self->currentDevice->transferFinished = true;
                        self->currentDevice->state = SPI_SS_IDLE;
                    }
                }
                break;
        } // end switch
        self->currentDevice = self->currentDevice->next;
    }
}

// *****************************************************************************

void SPI_Manager_Enable(SPIManager *self)
{

}

// *****************************************************************************

void SPI_Manager_Disable(SPIManager *self)
{

}

// *****************************************************************************

void SPI_Manager_SetSSPinFunc(SPISlave *self, void (*Function)(bool setPinHigh, void *slaveContext))
{
    self->SetSSPin = Function;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Static Functions ****************************************************//
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