/***************************************************************************//**
 * @brief Basic I2C Interface Header
 * 
 * @file IC21.h 
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 10/2/16   Original Creation
 * @date 2/21/22   Added Doxygen
 * @date 2/27/25   Refactored
 * 
 * @details
 *      @todo details. Reorganizing code based on my newer SPI manager code.
 *      master is only supported right now
 * 
 * @section license License
 * SPDX-FileCopyrightText: © 2016 Matthew Spinks
 * SPDX-License-Identifier: Zlib
 * 
 * This software is released under the Zlib license. You are free alter and
 * redistribute it, but you must not misrepresent the origin of the software.
 * This notice may not be removed. <http://www.zlib.net/zlib_license.html>
 * 
*******************************************************************************/

#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include <stdbool.h>

// ***** Defines ***************************************************************

enum
{
    I2C_NACK = 0,
    I2C_ACK = 1
};

// ***** Global Variables ******************************************************

typedef enum I2CRoleTag
{
    I2C_ROLE_UNKNOWN = 0,
    I2C_ROLE_MASTER,
    I2C_ROLE_SLAVE,
} I2CRole;

// @todo make error codes enum?

// @todo add status bits if needed for the peripheral level
typedef struct I2CStatusBitsTag
{
    union {
        struct {
            unsigned busy             :1;
            unsigned txEmpty          :1; // tx register empty
            unsigned rxNotEmpty       :1; // rx register not empty
            unsigned transmitFinished :1;
            unsigned fault            :1; // mode fault or frame error
            unsigned overflow         :1;
            unsigned                  :2;
        };
        uint8_t all;
    };
} I2CStatusBits;

// @todo keep old get status bits function?

typedef struct I2CInitTypeTag
{
    I2CRole role;
    uint32_t BRGValue;
    bool useTxInterrupt;
    bool useRxInterrupt;
} I2CInitType;

typedef struct I2CInterfaceTag
{
    /*  These are the functions that will be called. You will create your own
    interface object for your class that will have these function signatures.
    Set each of your functions equal to one of these pointers */
    uint32_t (*I2C_ComputeBRGValue)(uint32_t, uint32_t);
    void (*I2C_Init)(I2CInitType *params);
    void (*I2C_Enable)(void);
    void (*I2C_Disable)(void);
    void (*I2C_ReceivedDataEvent)(void);
    uint8_t (*I2C_GetReceivedByte)(void);
    bool (*I2C_IsReceiveRegisterFull)(void);
    bool (*I2C_IsReceiveUsingInterrupts)(void);
    void (*I2C_ReceiveEnable)(void);
    void (*I2C_ReceiveDisable)(void);
    void (*I2C_TransmitRegisterEmptyEvent)(void);
    void (*I2C_TransmitByte)(uint8_t);
    bool (*I2C_IsTransmitRegisterEmpty)(void);
    bool (*I2C_IsTransmitFinished)(void);
    bool (*I2C_IsTransmitUsingInterrupts)(void); // @todo transmit using interrupts

    // @todo pending event handler, and function setters
    void (*I2C_PendingEventHandler)(void);
    void (*I2C_SetTransmitRegisterEmptyCallback)(void (*Function)(void));
    void (*I2C_SetReceivedDataCallback)(void (*Function)(uint8_t (*CallToGetData)(void)));

    bool (*I2C_IsBusy)(void);
    // @todo add get state function also?
    void (*I2C_Start)(void);
    void (*I2C_Stop)(void);
    void (*I2C_Restart)(void);
    void (*I2C_SendAck)(bool ackOrNack);
    bool (*I2C_GetStartStatus)(void);
    bool (*I2C_GetStopStatus)(void);
    bool (*I2C_GetRestartStatus)(void);
    bool (*I2C_GetAckStatus)(void);

} I2CInterface;

typedef struct I2CTag
{
    I2CInterface *interface;
} I2C;

/** 
 * Description of struct members: // TODO description
 * 
 * member1      description of variable member1
 * 
 */

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void I2C_Create(I2C *self, I2CInterface *interface);

void I2C_SetInitTypeToDefaultParams(I2CInitType *params);

// @todo add more params if needed
void I2C_SetInitTypeParams(I2CInitType *params, bool useRxInterrupt, bool useTxInterrupt);

// @todo does I2C need a init BRG value function? If not, remove ComputeBRGValue
void I2C_SetInitBRGValue(I2CInitType *params, uint32_t BRGValue);

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

uint32_t I2C_ComputeBRGValue(I2C *self, uint32_t desiredBaudRate, uint32_t clkInHz);

void I2C_Init(I2C *self, I2CInitType *params);

void I2C_Enable(I2C *self);

void I2C_Disable(I2C *self);

void I2C_ReceivedDataEvent(I2C *self);

uint8_t I2C_GetReceivedByte(I2C *self);

bool I2C_IsReceiveRegisterFull(I2C *self);

bool I2C_IsReceiveUsingInterrupts(I2C *self);

void I2C_ReceiveEnable(I2C *self);

void I2C_ReceiveDisable(I2C *self);

void I2C_TransmitRegisterEmptyEvent(I2C *self);

void I2C_TransmitByte(I2C *self, uint8_t dataToSend);

bool I2C_IsTransmitRegisterEmpty(I2C *self);

bool I2C_IsTransmitFinished(I2C *self);

bool I2C_IsTransmitUsingInterrupts(I2C *self);

// @todo pending event handler, and function setters
void I2C_PendingEventHandler(I2C *self);
void I2C_SetTransmitRegisterEmptyCallback(I2C *self, void (*Function)(void));
void I2C_SetReceivedDataCallback(I2C *self, void (*Function)(uint8_t (*CallToGetData)(void)));

// Other functions. (from old PIC32 I2C code)

bool I2C_IsBusy(I2C *self);

void I2C_Start(I2C *self);

void I2C_Stop(I2C *self);

void I2C_Restart(I2C *self);

void I2C_SendAck(I2C *self, bool ackOrNack);

bool I2C_GetStartStatus(I2C *self);

bool I2C_GetStopStatus(I2C *self);

bool I2C_GetRestartStatus(I2C *self);

bool I2C_GetAckStatus(I2C *self);

// @todo add IsBusIdle? or just use IsBusy?
// @todo add get state, make generic states
// @todo add get bus errors
// @todo handle bus collision

#endif /* I2C_H */
