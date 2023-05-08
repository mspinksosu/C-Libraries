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
 *      // TODO details
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
    SPI_ROLE_MASTER = 0,
    SPI_ROLE_SLAVE,
} SPIRole;

typedef enum SPIModeTag
{
    SPI_MODE_0 = 0, // clock idle low, sample on first edge
    SPI_MODE_1,     // clock idle low, sample on second edge
    SPI_MODE_2,     // clock idle high, sample on first edge
    SPI_MODE_3      // clock idle high, sample on second edge
} SPIMode;

typedef enum SPISSControlTag
{
    SPI_SS_NONE = 0,
    SPI_SS_HARDWARE,
    SPI_SS_CALLBACKS, // User will implement functions for controlling the pin
} SPISSControl;

typedef struct SPIInitTypeTag
{
    SPIRole role;
    SPIMode mode;
    SPISSControl ssControl;
    bool useTxInterrupt;
    bool useRxInterrupt;
} SPIInitType;

typedef struct SPIInterfaceTag
{
    /*  These are the functions that will be called. You will create your own
    interface object for your class that will have these function signatures.
    Set each of your functions equal to one of these pointers */
    void (*SPI_Init)(SPIInitType *params);
    void (*SPI_Enable)(void);
    void (*SPI_Disable)(void);
    void (*SPI_ReceivedDataEvent)(void);
    uint8_t (*SPI_GetReceivedByte)(void);
    bool (*SPI_IsReceiveRegisterFull)(void);
    void (*SPI_TransmitFinishedEvent)(void);
    void (*SPI_TransmitByte)(uint8_t);
    bool (*SPI_IsTransmitRegisterEmpty)(void);
    SPIStatusBits (*SPI_GetStatus)(void);
    void (*SPI_PendingEventHandler)(void);
    void (*SPI_SetTransmitFinishedCallback)(void (*Function)(void));
    void (*SPI_SetReceivedDataCallback)(void (*Function)(uint8_t));
    void (*SPI_SetSSPinFunc)(void (*Function)(bool));
} SPIInterface;

typedef struct SPITag
{
    SPIInterface *interface;
} SPI;

typedef struct SPIStatusBitsTag
{
    union {
        struct {
            // TODO status bits
            unsigned :8;
        };
        uint8_t all;
    };

} SPIStatusBits;

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

/***************************************************************************//**
 * @brief Combine the object and function table 
 * 
 * The SPIInterface is a list of functions for your SPI. When you create a
 * a SPI implementation, you implement each of the functions listed in the 
 * interface and also assign them to one of the function pointers in the 
 * SPIInterface object.
 * 
 * @param self  pointer to the SPI you are using
 * 
 * @param interface  pointer to the function table that your SPI uses
 */
void SPI_Create(SPI *self, SPIInterface *interface);

/***************************************************************************//**
 * @brief Set the default values of the SPIInitType object
 * 
 * master, SPI mode 0, no slave select control, no interrupts
 * 
 * @param params  pointer to the SPIInitType that you are using
 */
void SPI_SetInitTypeToDefaultParams(SPIInitType *params);

/***************************************************************************//**
 * @brief Change the initial parameters of your SPIInitType object
 * 
 * SPI Mode 0 is the most common mode which is clock idle low, sample on first
 * edge. Slave select hardware mode is only really useful if you have only one 
 * slave device, or all slave devices daisy chained together, and your micro
 * has a dedicated SS pin.
 * 
 * @param params  pointer to the SPIInitType that you are going to use
 * @param role  SPI_ROLE_MASTER, SPI_ROLE_SLAVE
 * @param mode  SPI_MODE_0, SPI_MODE_1, SPI_MODE_2, SPI_MODE_3
 * @param ssControl  SPI_SS_NONE, SPI_SS_HARDWARE, SPI_SS_CALLBACKS
 * @param useRxInterrupt  true or false
 * @param useTxInterrupt  true or false
 */
void SPI_SetInitTypeParams(SPIInitType *params, SPIRole role, SPIMode mode, 
    SPISSControl ssControl, bool useRxInterrupt, bool useTxInterrupt);

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Initialize the SPI
 * 
 * Use the parameters provided to set the necessary registers for your MCU.
 * 
 * @param self  pointer to the SPI you are using
 * 
 * @param params  pointer to the SPIInitType that you are using
 */
void SPI_Init(SPI *self, SPIInitType *params);

/***************************************************************************//**
 * @brief Enable the SPI transmitter/receiver
 * 
 * @param self  pointer to the SPI you are using
 */
void SPI_Enable(SPI *self);

/***************************************************************************//**
 * @brief Disable the SPI transmitter/receiver
 * 
 * @param self  pointer to the SPI you are using
 */
void SPI_Disable(SPI *self);

/***************************************************************************//**
 * @brief A byte has been received. Set SS line, then call Rx callback.
 * 
 * This event is called whenever a byte is shifted into the received data
 * register. If you are using interrupts, this will be the function you call
 * wherever the receive interrupt is at. In many instances, when the master is
 * transmitting data it doesn't need to receive anything. So hold the byte in 
 * temporary buffer and clear the Rx interrupt flag. Then set the SS line high,
 * and call the receive data callback function pointer.
 * 
 * @param self  pointer to the SPI you are using
 */
void SPI_ReceivedDataEvent(SPI *self);

/***************************************************************************//**
 * @brief Get a byte, clear the interrupt flag.
 * 
 * Typically, the Rx interrupt flag is cleared automatically when the data 
 * register is read.
 * 
 * @param self  pointer to the SPI you are using
 * 
 * @return uint8_t  the data from the Rx register
 */
uint8_t SPI_GetReceivedByte(SPI *self);

/***************************************************************************//**
 * @brief Check if received data is available
 * 
 * This function will return true whenever a byte is shifted into the received
 * data register. If you are not using interrupts, you can poll this function,
 * then call GetReceivedByte when data is ready. If a byte was transmitted 
 * previously and the receive register becomes full, then data transmission is
 * complete. Set the SS line high.
 * 
 * @param self  pointer to the SPI you are using
 * 
 * @return true 
 */
bool SPI_IsReceiveRegisterFull(SPI *self);

/***************************************************************************//**
 * @brief A byte has been transmitted. Disable Tx interrupt, call Tx callback
 * 
 * This event is called whenever a byte is finished transmitting. This would be
 * equivalent to a "Tx register empty" event. If you are using interrupts, this
 * will be the function you call wherever your transmit register empty 
 * interrupt is at. Because we don't know when the full payload is finished, we
 * must disable the Tx interrupt after every byte. Then call the transmit
 * finished callback function pointer. In addition, you should check to make
 * sure there isn't already a call to this function pending. If there is, you
 * need to set a flag and return. The easiest way to do this is to use a static 
 * variable that is set whenever this function is entered. This flag will be 
 * checked by the PendingEventHandler function further below.
 * 
 * @param self  pointer to the SPI you are using
 */
void SPI_TransmitFinishedEvent(SPI *self);

/***************************************************************************//**
 * @brief Set the SS pin low, place data in the Tx register, send
 * 
 * If there is a dedicated SS line or a function, call it to set the SS line 
 * low, then place the data in the transmit register. If interrupts are being 
 * used, turn on the Tx interrupt.
 * 
 * @param self  pointer to the SPI you are using
 * 
 * @param dataToSend  the data you want to send out
 */
void SPI_TransmitByte(SPI *self, uint8_t dataToSend);

/***************************************************************************//**
 * @brief Check if the transmit register is empty
 * 
 * This function will return true whenever the contents of the Tx register have
 * been loaded into the shift register. If you are not using interrupts, you 
 * can poll this function to know when it's okay to load another byte. This 
 * function will not tell you exactly when a transmission is completed though,
 * so be careful. For that, you should should check IsReceiveRegisterFull.
 * 
 * @param self  pointer to the SPI you are using
 * 
 * @return true if the transmit register is empty
 */
bool SPI_IsTransmitRegisterEmpty(SPI *self);

/***************************************************************************//**
 * @brief Get the status bits of the SPI
 * 
 * @param self  pointer to the SPI you are using
 * 
 * @return SPIStatusBits  status bits
 */
SPIStatusBits SPI_GetStatus(SPI *self);

/***************************************************************************//**
 * @brief Checks for pending events (transmit finished)
 * 
 * Because the transmit finished callback is called within the interrupt, if
 * the user wants to transmit another byte, the transmit finished interrupt
 * will almost certainly fire before the current callback is done. This can
 * lead to multiple recursive function calls. You will need to have a flag of 
 * some kind set whenever the TransmitFinishedEvent function is entered. If 
 * another Tx interrupt is called then set a pending interrupt flag. This 
 * function will sit in a loop somewhere and check for a pending interrupt then 
 * call TransmitFinishedEvent for you. This will let the stack unwind. Note 
 * that this is really only an issue if you are using interrupts to transmit.
 * 
 * @param self  pointer to the SPI you are using
 */
void SPI_PendingEventHandler(SPI *self);

/***************************************************************************//**
 * @brief Set a function to be called whenever the transmit event happens
 * 
 * This function pointer is called from within the TransmitFinishedEvent
 * function, which is your "transmit register not empty" interrupt. Your 
 * function should follow the format listed below.
 * 
 * @param self  pointer to the SPI you are using
 * 
 * @param Function  format: void SomeFunction(void)
 */
void SPI_SetTransmitFinishedCallback(SPI *self, void (*Function)(void));

/***************************************************************************//**
 * @brief Set a function to be called whenever a received data event happens
 * 
 * Create a function using the format listed below. When your function is 
 * called, you will receive the data from the Rx register. Every time you 
 * transmit using SPI you are also receiving. In many cases, you may not need
 * the data from the receive register. It is up to you to handle this.
 * 
 * @param self  pointer to the SPI you are using
 * 
 * @param Function  format: void SomeFunction(uint8_t data)
 */
void SPI_SetReceivedDataCallback(SPI *self, void (*Function)(uint8_t data));

/***************************************************************************//**
 * @brief Set a function to allow this SPI library to control the SS pin.
 * 
 * There is one slave select pin callback option for each SPI peripheral. The 
 * SS pin is low as long as we are transmitting data. Your function should 
 * follow the format listed below. If the argument passed in is true, set the 
 * SS pin high.
 * 
 * This is best for using one slave or a bunch of slave daisy chained to one 
 * peripheral. This configuration will constitute the majority of users. If you
 * have multiple slaves tied to a peripheral with different SS lines, you will 
 * need to manage your SS pins yourself.
 * 
 * @param self  pointer to the SPI you are using
 * 
 * @param Function  format: void SomeFunction(bool) // set pin high if true
 */
void SPI_SetSSPinFunc(SPI *self, void (*Function)(bool setPinHigh));

#endif  /* ISPI_H */