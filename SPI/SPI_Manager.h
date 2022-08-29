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

#ifndef SPI_MANAGER_H
#define SPI_MANAGER_H

#include "ISPI.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

typedef struct SPISlaveTag
{
    void (*SetSSPin)(bool setPinHigh, void *slaveContext);
    SPI *peripheral;
    uint8_t slaveAddress;
    uint8_t *writeBuffer;
    uint8_t *readBuffer;
    uint8_t numBytesToSend;
    uint8_t numBytesToRead;
    uint8_t writeCount;
    uint8_t readCount;
} SPISlave;

typedef struct SPISlaveEntryTag SPISlaveEntry;

struct SPISlaveEntryTag
{
    SPISlave *slaveDevice;
    SPISlaveEntry *next;
};

typedef struct SPIEntryTag SPIEntry;

struct SPIEntryTag
{
    SPI *peripheral;
    bool peripheralBusy;
    SPIEntry *next;
};

// static SPIEntry *peripheralList = NULL;  // linked list
// static SPISlaveEntry *ptrToLastDevice = NULL;  // circular linked list
// static SPISlaveEntry *currentDevice = NULL;  // index for linked list

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Function Prototypes *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


void SPI_Manager_Init(void);

//void ADC_Manager_AddChannel(ADCChannelEntry *self, ADCChannel *newChannel);

void SPI_Manager_Tick(void);

void SPI_Manager_Enable(void);

void SPI_Manager_Disable(void);

/* TODO Figure out if I should have an address argument. Or maybe use a 
SPISlave object. Or possibly put the function pointer in the slave object. */
void SPI_Manager_SetSSPinFunc(SPISlave *self, void (*Function)(bool setPinHigh, void *slaveContext));

#endif  /* SPI_MANAGER_H */