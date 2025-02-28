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
 *      // TODO details
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

#ifndef SPI_MANAGER_H
#define SPI_MANAGER_H

#include "ISPI.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

typedef enum SPISlaveStateTag
{
    SPI_STATE_IDLE = 0,
    SPI_STATE_RQ_START,
    SPI_STATE_SEND_BYTE,
    SPI_STATE_RECEIVE_BYTE
} SPISlaveState;

typedef struct SPISlaveTag SPISlave;

// @todo add callback function pointers like in old I2C library?
struct SPISlaveTag
{
    SPISlave *next;
    void (*SetSSPin)(bool setPinHigh, void *slaveContext);
    SPIManager *manager;
    uint8_t *writeBuffer;
    uint8_t *readBuffer;
    uint16_t numBytesToSend;
    uint16_t numBytesToRead;
    uint16_t readWriteCount; // @todo might go back to my old method of making a private struct
    SPISlaveState state;
    bool transferFinished;
};

typedef struct SPIManagerTag
{
    SPI *peripheral;
    SPISlave *endOfList; // circular linked list
    SPISlave *device;
    bool busy;
    // @todo is the busy flag needed? The SPI manager is just using each slave device's SPISlaveState
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

/* TODO finish Doxygen */

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