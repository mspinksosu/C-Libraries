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

/* If there are more peripherals added than exist in the array, then we start 
going through the device list to check which ones are currently busy. This 
number can be reduced to save a few bytes if you need to. */
#define DEFAULT_NUM_PERIPHERALS 4

// ***** Global Variables ******************************************************

static SPISlave *ptrToLastDevice = NULL;  // circular linked list
static SPISlave *currentDevice = NULL;  // index for linked list
static SPIEntry peripheralArray[DEFAULT_NUM_PERIPHERALS];
static numPeripherals = 0;

// ***** Static Function Prototypes ********************************************

/* Put static function prototypes here */
static void SPI_Manager_AddPeripheral(SPI *newPeripheral);
static void SPI_Manager_DevicePush(SPISlave *self);
static bool SPI_Manager_CheckPeripheralBusy(SPISlave *slave);

// *****************************************************************************

void SPI_Manager_Init(void)
{

    /* Initialize the SPI peripherals */

}

// *****************************************************************************

void SPI_Manager_CreateSlave(SPISlave *self, SPI *peripheral, uint8_t *writeBuffer, uint8_t *readBuffer)
{
    self->peripheral = peripheral;
    self->writeBuffer = writeBuffer;
    self->readBuffer = readBuffer;
    self->numBytesToSend = 0;
    self->numBytesToRead = 0;
    self->readWriteCount = 0;
}

// *****************************************************************************

void SPI_Manager_AddSlave(SPISlave *self)
{
    if(ptrToLastDevice == NULL)
    {
        /* Begin with a new list */
        ptrToLastDevice = self;

        /* Since the list only contains one entry, the "next" pointer will
        also point to itself */
        self->next = ptrToLastDevice;

        /* Set index to the beginning */
        currentDevice = ptrToLastDevice->next;
    }
    else
    {
        /* Add the slave device to the list */
        SPI_Manager_DevicePush(self);
        currentDevice = ptrToLastDevice->next; // reset the index
    }

    /* Add the SPI peripheral to the peripheral list */
    //SPI_Manager_AddPeripheral(self->peripheral);
}

// *****************************************************************************

void SPI_Manager_BeginTransfer(SPISlave *self, uint16_t numBytesToSend, uint16_t numBytesToRead)
{

}

// *****************************************************************************

void SPI_Manager_Process(void)
{
    /* Go round-robin through the list of devices */
    if(currentDevice != NULL && currentDevice->peripheral != NULL)
    {
        bool devicePeripheralBusy = SPI_Manager_CheckPeripheralBusy(currentDevice);

        switch(currentDevice->state)
        {
            case SPI_SS_BEGIN:
                if(!devicePeripheralBusy)
                {
                    /* Begin transfer. Set slave select line low */
                    if(currentDevice->SetSSPin != NULL)
                        currentDevice->SetSSPin(false, currentDevice);

                    /* Send the first byte */
                    if(currentDevice->numBytesToSend > 0)
                    {
                        SPI_TransmitByte(currentDevice->peripheral, 
                        currentDevice->writeBuffer[currentDevice->readWriteCount]);
                    }
                    else if(currentDevice->numBytesToRead > 0)
                    {
                        /* Send empty data out for a slave read */
                        SPI_TransmitByte(currentDevice->peripheral, 0);
                    }
                    currentDevice->state = SPI_SS_SEND_BYTE;
                }
                break;
            case SPI_SS_SEND_BYTE:

                break;
            case SPI_SS_RECEIVE_BYTE:

                break;
            case SPI_SS_FINISHED:
                // Set SS line high
                currentDevice->state = SPI_SS_IDLE;
                break;
        } // end switch

        /* TODO Receive byte */
        if(SPI_IsReceiveRegisterFull(currentDevice->peripheral))
        {
            uint8_t data = SPI_GetReceivedByte(currentDevice->peripheral);

            if(currentDevice->readWriteCount <= currentDevice->numBytesToRead)
            {
                currentDevice->readBuffer[currentDevice->readWriteCount] = data;
            }
            currentDevice->readWriteCount++;
        }

        /* TODO Check if there are more bytes to transfer */
        if(currentDevice->readWriteCount < currentDevice->numBytesToSend)
        {

        }
        else if(currentDevice->readWriteCount < currentDevice->numBytesToRead)
        {

        }
        else
        {
            //currentDevice->state.transferFinished = 1;
        }

        currentDevice = currentDevice->next;
    }
}

// *****************************************************************************

void SPI_Manager_Enable(void)
{

}

// *****************************************************************************

void SPI_Manager_Disable(void)
{

}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Static Functions ****************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Add SPI peripheral to the array 
 * 
 * @param newPeripheral 
 */
static void SPI_Manager_AddPeripheral(SPI *newPeripheral)
{
    if(newPeripheral == NULL || newPeripheral->interface == NULL)
        return;

    bool deviceInList = false;

    for(uint8_t i = 0; i < DEFAULT_NUM_PERIPHERALS; i++)
    {
        if(peripheralArray[i].peripheral == newPeripheral)
        {
            deviceInList = true;
            break; // device already in list
        }
        else if(peripheralArray[i].peripheral == NULL)
        {
            // Add to list
            deviceInList = true;
            peripheralArray[i].peripheral = newPeripheral;
            numPeripherals++;
            break;
        }
    }

    if(!deviceInList)
        numPeripherals++;
}

/***************************************************************************//**
 * @brief Check if a certain SPI peripheral is busy
 * 
 * Go through the list of devices and see if anyone is already using my 
 * peripheral. I was originally going to have a list of peripherals, but then I
 * would have to loop that list and update it anyway. For a small number of 
 * devices, this won't take long.
 * 
 * @param slave 
 * 
 * @return true 
 */
static bool SPI_Manager_CheckPeripheralBusy(SPISlave *slave)
{
    bool isBusy = false;
    SPISlave *index = ptrToLastDevice->next;

    while(index != ptrToLastDevice)
    {
        if(slave->peripheral == index->peripheral)
        {
            if(index->state != SPI_SS_IDLE)
                isBusy = true;
            break;
        }
        index = index->next;
    }
    return isBusy;
}

static void SPI_Manager_DevicePush(SPISlave *self)
{
    /* Add the new entry to the beginning of the list. The last entry's "next" 
    pointer will always point to the beginning of the list */
    self->next = ptrToLastDevice->next;
    
    /* Update the beginning entry to point to the new beginning */
    ptrToLastDevice->next = self;
}

/*
 End of File
 */