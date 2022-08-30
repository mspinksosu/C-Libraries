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

typedef struct SPIPeripheralTag
{
    SPI *peripheral;
    bool peripheralBusy;
} SPIPeripheral;

static SPISlaveEntry *ptrToLastDevice = NULL;  // circular linked list
static SPISlaveEntry *currentDevice = NULL;  // index for linked list
static SPIPeripheral peripheralArray[DEFAULT_NUM_PERIPHERALS];
static numPeripherals = 0;

// ***** Static Function Prototypes ********************************************

/* Put static function prototypes here */
static void SPI_Manager_AddPeripheral(SPI *newPeripheral);
static void SPI_Manager_DevicePush(SPISlaveEntry *self, SPISlave *newChannel);
// static void ADC_Manager_InsertChannelAfter(ADCChannelEntry *entryToInsert, ADCChannelEntry *prev, ADCChannel *newChannel);

// *****************************************************************************

void ADC_Manager_Init(uint16_t sampleTimeMs, uint16_t tickRateMs)
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

void SPI_Manager_AddSlaveDevice(SPISlaveEntry *self, SPISlave *newDevice)
{
    if(ptrToLastDevice == NULL)
    {
        /* Begin with empty list */
        self->device = newDevice;

        /* Update the pointer to point to the last entry, which is itself */
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
        SPI_Manager_DevicePush(self, newDevice);
        currentDevice = ptrToLastDevice->next; // reset the index
    }

    /* Add the SPI peripheral to the peripheral list */
    SPI_Manager_AddPeripheral(newDevice->peripheral);
}

// *****************************************************************************

void SPI_Manager_BeginTransfer(SPISlave *self, uint16_t numBytesToSend, uint16_t numBytesToRead)
{

}

// *****************************************************************************

void SPI_Manager_Tick(void)
{
    /* Go round-robin through the list of devices, first checking if the
    peripheral is already in use. */
    if(currentDevice != NULL)
    {


        currentDevice = currentDevice->next;
    }
}

// *****************************************************************************

void ADC_Manager_Enable(void)
{

}

// *****************************************************************************

void ADC_Manager_Disable(void)
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
 * I decided to have an array with a default size that should cover the 
 * majority of use cases. This is so that the user doesn't need to worry about
 * having to create a list of SPI peripherals as well as SPI devices. If there 
 * are more SPI peripherals than I have room for, then I will go through the 
 * list of devices, checking if their peripheral is busy before starting a 
 * transfer. 
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

static void SPI_Manager_DevicePush(SPISlaveEntry *self, SPISlave *newDevice)
{
    /* Store the new data */
    self->device = newDevice;
    /* Add the new entry to the beginning of the list. The last entry's "next" 
    pointer will always point to the beginning of the list */
    self->next = ptrToLastDevice->next;
    /* Update the beginning entry to point to the new beginning */
    ptrToLastDevice->next = self;
}

// static void ADC_Manager_InsertChannelAfter(ADCChannelEntry *entryToInsert, ADCChannelEntry *prev, ADCChannel *newChannel)
// {
//     if(prev == NULL || entryToInsert == NULL)
//     {
//         return;
//     }
//     /* Store the new data */
//     entryToInsert->channel = newChannel;
//     /* Insert the new entry */
//     entryToInsert->next = prev->next;
//     /* Change the previous entry to point to our new entry */
//     prev->next = entryToInsert->next;
// }

/*
 End of File
 */