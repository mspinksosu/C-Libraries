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

static SPISlave *ptrToLastDevice = NULL;  // circular linked list
static SPISlave *currentDevice = NULL;  // index for linked list
static SPIEntry peripheralArray[DEFAULT_NUM_PERIPHERALS];
static numPeripherals = 0;

// ***** Static Function Prototypes ********************************************

/* Put static function prototypes here */
static void SPI_Manager_AddPeripheral(SPI *newPeripheral);
static void SPI_Manager_DevicePush(SPISlave *self);
static bool SPI_Manager_IsPeripheralBusy(SPISlave *slave);

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
    self->transferFinished = false;
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

void SPI_Manager_Process(void)
{
    /* Go round-robin through the list of devices. Right now, I'm only going to 
    deal with SPI master mode. */
    // TODO add check for master mode, and eventually add slave mode
    if(currentDevice != NULL && currentDevice->peripheral != NULL)
    {
        switch(currentDevice->state)
        {
            case SPI_SS_RQ_START:
                if(!SPI_Manager_IsPeripheralBusy(currentDevice))
                {
                    /* Begin transfer. Set slave select line low */
                    if(currentDevice->SetSSPin != NULL)
                        currentDevice->SetSSPin(false, currentDevice);
                    currentDevice->state = SPI_SS_SEND_BYTE;
                }
                break;
            case SPI_SS_SEND_BYTE:
                if(currentDevice->readWriteCount < currentDevice->numBytesToSend)
                {
                    SPI_TransmitByte(currentDevice->peripheral, 
                        currentDevice->writeBuffer[currentDevice->readWriteCount]);
                }
                else
                {
                    /* Send empty data out for a slave read */
                    SPI_TransmitByte(currentDevice->peripheral, 0);
                }
                currentDevice->state = SPI_SS_SEND_BYTE;
                break;
            case SPI_SS_RECEIVE_BYTE:
                /* In master mode, there is always a read after a write. */
                if(SPI_IsTransmitRegisterEmpty(currentDevice->peripheral))
                {
                    uint8_t data = SPI_GetReceivedByte(currentDevice->peripheral);

                    if(currentDevice->readWriteCount < currentDevice->numBytesToRead)
                    {
                        currentDevice->readBuffer[currentDevice->readWriteCount] = data;
                    }
                    currentDevice->readWriteCount++;
                    
                    if(currentDevice->readWriteCount < currentDevice->numBytesToSend ||
                        currentDevice->readWriteCount < currentDevice->numBytesToRead)
                    {
                        /* There are more bytes to send */
                        currentDevice->state = SPI_SS_SEND_BYTE;
                    }
                    else
                    {
                        /* Set slave select line high */
                        if(currentDevice->SetSSPin != NULL)
                            currentDevice->SetSSPin(true, currentDevice);
                        currentDevice->transferFinished = true;
                        currentDevice->state = SPI_SS_IDLE;
                    }
                }
                break;
        } // end switch
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

/* TODO remove this later. I decided to just use the slave list instead of a 
seperate peripheral list. This makes it easier for the user. I do have to 
traverse the list, but I made it so that I only have to do so whenever a 
request to begin data transfer happens. */
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
 * peripheral.
 * 
 * @param slave  pointer to the slave who's peripheral we are checking
 * 
 * @return true  true if anyone else is already using that peripheral
 */
static bool SPI_Manager_IsPeripheralBusy(SPISlave *slave)
{
    bool isBusy = false;
    SPISlave *index = slave->next;
    /* Go around the loop once, starting with our the slave index + 1 */
    while(index != slave)
    {
        if(index->peripheral == slave->peripheral)
        {
            if(index->state == SPI_SS_SEND_BYTE || index->state == SPI_SS_RECEIVE_BYTE)
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