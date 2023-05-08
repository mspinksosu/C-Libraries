/***************************************************************************//**
 * @brief SPI 1 Implementation Header (Non-Processor Specific)
 * 
 * @author Matthew Spinks
 * 
 * @date 9/9/22    Original creation
 * 
 * @file SPI1.h
 * 
 * @details
 *      // TODO details
 * 
 ******************************************************************************/

#ifndef SPI1_H
#define SPI1_H

#include "ISPI.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/* Declare and define this variable in your implementation's .c file */
extern SPIInterface SPI1_FunctionTable;


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void SPI1_Init(SPIInitType *params);

void SPI1_Enable(void);

void SPI1_Disable(void);

void SPI1_ReceivedDataEvent(void);

uint8_t SPI1_GetReceivedByte(void);

bool SPI1_IsReceiveRegisterFull(void);

void SPI1_TransmitRegisterEmptyEvent(void);

void SPI1_TransmitByte(uint8_t dataToSend);

bool SPI1_IsTransmitRegisterEmpty(void);

bool SPI1_IsTransmitFinished(void);

SPIStatusBits SPI1_GetStatus(void);

void SPI1_PendingEventHandler(void);

void SPI1_SetTransmitFinishedCallback(void (*Function)(void));

void SPI1_SetReceivedDataCallback(void (*Function)(uint8_t data));

void SPI1_SetSSPinFunc(void (*Function)(bool setPinHigh));

#endif  /* SPI1_H */