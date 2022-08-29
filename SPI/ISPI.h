/***************************************************************************//**
 * @brief SPI Interface Header File
 * 
 * @author Matthew Spinks
 * 
 * @date 8/27/22  Original creation
 * 
 * @file ISPI.h
 * 
 * @details
 *      TODO
 * 
 ******************************************************************************/

#ifndef ISPI_H
#define ISPI_H

#include <stdint.h>
#include <stdbool.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

typedef enum SPIRoleTag
{
    SPI_MASTER = 0,
    SPI_SLAVE,
} SPIRole;

typedef enum SPIModeTag
{
    SPI_MODE_0 = 0, // clock idle low, sample on first edge
    SPI_MODE_1, // clock idle low, sample on second edge
    SPI_MODE_2, // clock idle high, sample on first edge
    SPI_MODE_3 // clock idle high, sample on second edge
} SPIMode;

typedef enum SPISSControlTag
{
    SPI_SS_NONE = 0,
    SPI_SS_HARDWARE,
    SPI_SS_CALLBACKS, // User will implement functions for controlling pins
} SPISSControl;

typedef struct SPIInitTypeTag
{
    SPIRole role;
    SPIMode mode;
    SPISSControl control;
    bool useTxInterrupt;
    bool useRxInterrupt;
} SPIInitType;

typedef struct SPIInterfaceTag
{
    /*  These are the functions that will be called. You will create your own
    interface object for your class that will have these function signatures.
    Set each of your functions equal to one of these pointers */

} SPIInterface;

typedef struct SPITag
{
    SPIInterface *interface;
} SPI;

typedef struct SPIStatusBitsTag
{
    union {
        struct {
            // TODO
            unsigned :8;
        };
        uint8_t all;
    };

} SPIStatusBits;

/** 
 * Description of struct
 * 
 * member1      description of variable member1
 * 
 */

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void SPI_Create(SPI *self, SPIInterface *interface);

void SPI_SetInitTypeToDefaultParams(SPIInitType *params);

void SPI_SetInitTypeParams(SPIInitType *params, SPIRole role, SPIMode mode, 
    SPISSControl ssControl, bool useRxInterrupt, bool useTxInterrupt);

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void SPI_Init(SPI *self, SPIInitType *params);

void SPI_ReceivedDataEvent(SPI *self);

uint8_t SPI_GetReceivedByte(SPI *self);

bool SPI_IsReceiveRegisterFull(SPI *self);

void SPI_ReceiveEnable(SPI *self);

void SPI_ReceiveDisable(SPI *self);

void SPI_TransmitFinishedEvent(SPI *self);

void SPI_TransmitByte(SPI *self, uint8_t dataToSend);

bool SPI_IsTransmitRegisterEmpty(SPI *self);

void SPI_TransmitEnable(SPI *self);

void SPI_TransmitDisable(SPI *self);

SPIStatusBits SPI_GetStatus(SPI *self);

void SPI_SetSSPin(bool setPinHigh, uint8_t slaveAddress); // for master mode

#endif  /* ISPI_H */