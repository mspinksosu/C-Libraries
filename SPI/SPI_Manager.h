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
    SPI_SS_BEGIN,
    SPI_SS_SEND_BYTE,
    SPI_SS_RECEIVE_BYTE,
    SPI_SS_FINISHED
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
};

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Function Prototypes *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


void SPI_Manager_Init(void);

/* TODO should I add size parameters or just let the user handle it? */
void SPI_Manager_CreateSlave(SPISlave *self, SPI *peripheral, uint8_t *writeBuffer, uint8_t *readBuffer);

void SPI_Manager_AddSlave(SPISlave *self);

void SPI_Manager_BeginTransfer(SPISlave *self, uint16_t numBytesToSend, uint16_t numBytesToRead);

bool SPI_Manager_IsTransferFinished(SPISlave *self);

void SPI_Manager_Process(void);

void SPI_Manager_Enable(void);

void SPI_Manager_Disable(void);

/* TODO Figure out if I should have an address argument. Or maybe use a 
SPISlave object. Or possibly put the function pointer in the slave object. */
void SPI_Manager_SetSSPinFunc(SPISlave *self, void (*Function)(bool setPinHigh, void *slaveContext));

#endif  /* SPI_MANAGER_H */