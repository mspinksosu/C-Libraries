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

typedef enum SPISlaveStateTag
{
    SPI_SS_IDLE = 0,
    SPI_SS_RQ_START,
    SPI_SS_SEND_BYTE,
    SPI_SS_RECEIVE_BYTE
} SPISlaveState;

typedef struct SPISlaveTag SPISlave;

struct SPISlaveTag
{
    SPISlave *next;
    void (*SetSSPin)(bool setPinHigh, void *slaveContext);
    SPI *peripheral;
    uint8_t *writeBuffer;
    uint8_t *readBuffer;
    uint16_t numBytesToSend;
    uint16_t numBytesToRead;
    uint16_t readWriteCount;
    SPISlaveState state;
    bool transferFinished;
};

typedef struct SPIEntryTag SPIEntry;

struct SPIEntryTag
{
    SPI *peripheral;
    SPIEntry *next;
    bool peripheralBusy;
};

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Function Prototypes *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


void SPI_Manager_Init(void);

/* TODO should I add size parameters or just let the user handle it? */
void SPI_Manager_CreateSlave(SPISlave *self, SPI *peripheral, uint8_t *writeBuffer, uint8_t *readBuffer);

/* TODO I should probably just get rid of the create function and change it to add */
void SPI_Manager_AddSlave(SPISlave *self);

bool SPI_Manager_IsDeviceBusy(SPISlave *self);

void SPI_Manager_BeginTransfer(SPISlave *self, uint16_t numBytesToSend, uint16_t numBytesToRead);

bool SPI_Manager_IsTransferFinished(SPISlave *self);

//void SPI_Manager_GetData(SPISlave *self, uint8_t *retNumBytesSent, uint8_t *retNumBytesRead);

void SPI_Manager_Process(void);

void SPI_Manager_Enable(void);

void SPI_Manager_Disable(void);

void SPI_Manager_SetSSPinFunc(SPISlave *self, void (*Function)(bool setPinHigh, void *slaveContext));

#endif  /* SPI_MANAGER_H */