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
 *      // TODO details
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
    SPIManager *manager;
    uint8_t *writeBuffer;
    uint8_t *readBuffer;
    uint16_t numBytesToSend;
    uint16_t numBytesToRead;
    uint16_t readWriteCount;
    SPISlaveState state;
    bool transferFinished;
};

typedef struct SPIManagerTag
{
    SPI *peripheral;
    SPISlave *endOfList; // circular linked list
    SPISlave *device;
    bool busy;
    // TODO is the busy flag needed? The SPI manager is just using each slave device's SPISlaveState
} SPIManager;

/**
 * Description of struct members:
 * // TODO description
 * 
 */

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Function Prototypes *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void SPI_Manager_Create(SPIManager *self, SPI *peripheral);

void SPI_Manager_AddSlave(SPIManager *self, SPISlave *slave, uint8_t *writeBuffer, uint8_t *readBuffer);

bool SPI_Manager_IsDeviceBusy(SPISlave *self);

void SPI_Manager_BeginTransfer(SPISlave *self, uint16_t numBytesToSend, uint16_t numBytesToRead);

bool SPI_Manager_IsTransferFinished(SPISlave *self);

void SPI_Manager_Process(SPIManager *self);

void SPI_Manager_Enable(SPIManager *self);

void SPI_Manager_Disable(SPIManager *self);

void SPI_Manager_SetSSPinFunc(SPISlave *self, void (*Function)(bool setPinHigh, void *slaveContext));

#endif  /* SPI_MANAGER_H */