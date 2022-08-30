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
    uint8_t *writeBuffer;
    uint8_t *readBuffer;
    uint16_t numBytesToSend;
    uint16_t numBytesToRead;
    uint16_t readWriteCount;
    uint8_t slaveAddress; // TODO probably not needed
    bool busy; // TODO probably not needed
} SPISlave;

typedef struct SPISlaveEntryTag SPISlaveEntry;

struct SPISlaveEntryTag
{
    SPISlave *device;
    SPISlaveEntry *next;
};

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Function Prototypes *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


void SPI_Manager_Init(void);

/* TODO should I add size parameters or just let the user handle it? */
void SPI_Manager_CreateSlave(SPISlave *self, SPI *peripheral, uint8_t *writeBuffer, uint8_t *readBuffer);

void SPI_Manager_AddSlaveDevice(SPISlaveEntry *self, SPISlave *newDevice);

void SPI_Manager_BeginTransfer(SPISlave *self, uint16_t numBytesToSend, uint16_t numBytesToRead);

void SPI_Manager_Tick(void);

void SPI_Manager_Enable(void);

void SPI_Manager_Disable(void);

/* TODO Figure out if I should have an address argument. Or maybe use a 
SPISlave object. Or possibly put the function pointer in the slave object. */
void SPI_Manager_SetSSPinFunc(SPISlave *self, void (*Function)(bool setPinHigh, void *slaveContext));

#endif  /* SPI_MANAGER_H */