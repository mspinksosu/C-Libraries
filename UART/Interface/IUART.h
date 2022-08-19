/***************************************************************************//**
 * @brief Basic UART Interface Header
 * 
 * @author Matthew Spinks
 * 
 * @date 12/1/14   Original creation
 * @date 2/4/22    Changed to interface. Added Doxygen
 * @date 3/3/22    Redesigned to use function table. Also added new parameters
 * @date 6/13/22   Changed compute baud rate function and flow control
 * @date 7/31/22   Added handler for pending transmit interrupt
 * 
 * @file IUART.h
 * 
 * @details
 *      An interface for a UART library to be used with different processors.
 * There are two types of objects. One that holds the properties needed for the
 * UART to operate and the other other that holds the parameters needed to
 * initialize the UART object called UARTInitType.
 * 
 * The UART object contains a pointer to the UARTInterface object. This
 * UARTInterface or function table tells the interface what functions to call.
 * Each of the UART implementations will have its own function table. When the
 * function table is created, its members (which are function pointers) are
 * initialized to the functions in that implementation. By doing this, each 
 * UART peripheral can have its own function table, but the function calls
 * don't change. Therefore, we can easily swap UART periperhals around. This 
 * also allows us to name our UARTs. For example "bluetoothUART" could be 
 * UART1 on one processor, but UART3 on a different processor. The code that 
 * uses "bluetoothUART" never has to change though.
 * 
 * Declare your UARTInterface object as extern in your UART implementation's
 * header file. This is so whatever file does the initialization can set the
 * function table. Then, in your implementation's .c file declare and 
 * initialize your UARTInterface object. Set each of its members to the 
 * functions in your implementation.
 * 
 * Now you are ready to create a UART object. First, declare a UART object, 
 * then call the UART_Create function and pass it a reference to that 
 * UARTInterface function table you made in the previous step.
 * 
 * Next, declare a UARTInitType object, then call either UART_SetInitTypeParams
 * or UART_SetInitTypeToDefaultParams functions to configure the settings for 
 * your UART. The calculation for the UART baud rate will vary depending on 
 * your processor. Instead of having the init function always compute the 
 * baud rate, I decided to have a separate function called "ComputeBRGValue". 
 * This function will return the value to be loaded into the baud rate 
 * generator's registers. This gives more flexibility with how the math is 
 * handled (or not handled). How you choose to implement the ComputeBRGValue 
 * function is up to you, but it should still be implemented even if it isn't 
 * used. So, get your BRG value either by calling the compute function or 
 * hardcoding a precomputed value, and then give it to the UART_SetBRGValue 
 * function. After this, you are ready to call UART_Init.
 * 
 * Example Code:
 *      UART myUART, anotherUART;
 *      UART_Create(&myUART, &UART1_FunctionTable);
 *      UART_SetInitTypeToDefaultParams(&myUART);
 *      uint32_t baud = UART_ComputeBRGValue(&myUART, 115200, 16000000UL);
 *      UART_SetBRGValue(&myUART, baud);
 *      UART_Init(&myUART);
 *      UART_Create(&anotherUART, &UART2FunctionTable);
 *      baud = UART_ComputeBRGValue(&anotherUART, 19200, 16000000UL);
 *      UART_SetBRGValue(&anotherUART, baud);
 *      UART_Init(&anotherUART);
 * 
 * ****************************************************************************/

#ifndef IUART_H
#define IUART_H

// ***** Includes **************************************************************

#include <stdint.h>
#include <stdbool.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

typedef enum UARTStopBitsTag
{
    UART_ONE_P = 0,
    UART_HALF_P,
    UART_ONE_PLUS_HALF_P,
    UART_TWO_P
} UARTStopBits;

typedef enum UARTParityTag
{
    UART_NO_PARITY = 0,
    UART_EVEN_PARITY,
    UART_ODD_PARITY
} UARTParity;

typedef enum UARTFlowControlTag
{
    UART_FLOW_NONE = 0,
    UART_FLOW_HARDWARE,
    UART_FLOW_CALLBACKS, // User will implement functions for controlling pins
    UART_FLOW_SOFTWARE
} UARTFlowControl;

typedef struct UARTInterfaceTag
{
    /*  These are the functions that will be called. You will create your own
    interface object for your class that will have these function signatures.
    Set each of your functions equal to one of these pointers */
    uint32_t (*UART_ComputeBRGValue)(uint32_t, uint32_t);
    void (*UART_Init)(UARTInitType *params);
    void (*UART_ReceivedDataEvent)(void);
    uint8_t (*UART_GetReceivedByte)(void);
    bool (*UART_IsReceiveRegisterFull)(void);
    void (*UART_ReceiveEnable)(void);
    void (*UART_ReceiveDisable)(void);
    void (*UART_TransmitFinishedEvent)(void);
    void (*UART_TransmitByte)(uint8_t);
    bool (*UART_IsTransmitRegisterEmpty)(void);
    void (*UART_TransmitEnable)(void);
    void (*UART_TransmitDisable)(void);
    void (*UART_PendingEventHandler)(void);
    void (*UART_SetTransmitFinishedCallback)(void (*Function)(void));
    void (*UART_SetReceivedDataCallback)(void (*Function)(uint8_t (*CallToGetData)(void)));
    void (*UART_SetIsCTSPinLowFunc)(bool (*Function)(void));
    void (*UART_SetRTSPinFunc)(void (*Function)(bool));
} UARTInterface;

typedef struct UARTTag
{
    UARTInterface *interface;
} UART;

typedef struct UARTInitTypeTag
{
    uint32_t BRGValue;
    UARTStopBits stopBits;
    UARTParity parity;
    UARTFlowControl flowControl;
    bool use9Bit;
    bool useTxInterrupt;
    bool useRxInterrupt;
} UARTInitType;

/** 
 * Description of struct members. You shouldn't really mess with any of these
 * variables directly. That is why I made functions for you to use.
 * 
 * UARTInterface  The table of functions that need to be implemented
 * 
 * BRGValue  The raw value that gets loaded into the baud rate generator 
 *           register. (not the baud rate!) Varies based on the processor.
 * 
 * stopBits  Most UART's use one stop bit, but they don't have to.
 * 
 * parity  The parity type, none, even, or odd.
 * 
 * use9Bit  Use 9 bit addressing. The actual method varies based on the 
 *          implementation.
 * 
 * flowControl  Hardware means the processor has some sort of dedicated flow
 *              control setting for that peripheral that takes control of the
 *              CTS and RTS pins. Callbacks allows the user make a simple 
 *              function to set the CTS and RTS pins. Software sends a special
 *              flow control message over the line.
 * 
 * useTxInterrupt  Enables or disables the transmit interrupt for you
 *                 automatically. Place the TransmitFinishedEvent function 
 *                 wherever that interrupt happens.
 * 
 * useRxInterrupt  Enables or disables the receive interrupt for you
 *                 automatically. Place the ReceivedDataEvent function wherever 
 *                 that interrupt happens. GetReceivedByte must still be called
 *                 in order to get the data and clear the flag.
 */

// ***** Function Prototypes ***************************************************

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Combine the object and function table 
 * 
 * The UARTInterface is a list of functions for your UART. When you create a
 * a UART implementation, you implement each of the functions listed in the 
 * interface and also assign them to one of the function pointers in the 
 * UARTInterface object.
 * 
 * @param self  pointer to the UART you are using
 * 
 * @param interface  pointer to the function table that your UART uses
 */
void UART_Create(UART *self, UARTInterface *interface);

/***************************************************************************//**
 * @brief Set the default values of the UARTInitType object
 * 
 * 8 data bits, 1 stop bit, no parity, no flow control, no interrupts
 * 
 * @param params  pointer to the UARTInitType that you are using
 */
void UART_SetInitTypeToDefaultParams(UARTInitType *params);

/***************************************************************************//**
 * @brief Change the initial parameters of your UARTInitType object
 * 
 * If UART_FLOW_CALLBACKS is chosen for flow control, you must implement
 * functions to set the RTS and CTS pins, and setup callbacks functions by
 * using the UART_SetRTSPinFunc and UART_SetIsCTSPinLowFunc functions. If you
 * using interrupts, you will want to setup callback functions using the 
 * SetReceivedDataCallback and SetTransmitFinishedCallback functions.
 * 
 * @param params  pointer to the UARTInitType that you are going to use
 * @param numStopBits UART_ONE_P,UART_HALF_P,UART_ONE_PLUS_HALF_P,UART_TWO_P
 * @param parityType UART_NO_PARITY, UART_EVEN_PARITY, UART_ODD_PARITY
 * @param enable9Bit true or false
 * @param flowControl UART_FLOW_NONE,UART_FLOW_HARDWARE,UART_FLOW_CALLBACKS,
 *                    UART_FLOW_SOFTWARE
 * @param useRxInterrupt true or false
 * @param useTxInterrupt true or false
 */
void UART_SetInitTypeParams(UARTInitType *params, UARTStopBits numStopBits, UARTParity parityType, 
    bool enable9Bit, UARTFlowControl flowControl, bool useRxInterrupt, bool useTxInterrupt);

/***************************************************************************//**
 * @brief Set the value for the baud rate generator
 * 
 * This is the value that gets loaded directly to the register, NOT the baud 
 * rate. This function will take your BRG value and apply it to the
 * UARTInitType object that you give it. The value will vary depending on
 * your processor.
 * 
 * @param params  pointer to the UARTInitType that you are using
 * 
 * @param BRGValue  the baud rate generator raw value (not the baud rate)
 */
void UART_SetInitBRGValue(UARTInitType *params, uint32_t BRGValue);

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Return a raw baud rate generator value
 * 
 * Computes the baud rate generator value that will loaded into the register
 * given the desired baud rate and the clock frequency in Hertz. You may then
 * take this value and call SetInitBRGValue.
 * 
 * @param self  pointer to the UART you are using
 * 
 * @param desiredBaudRate  the baud rate you want
 * 
 * @param clkInHz  the frequency of your UART peripherals clock in Hertz
 * 
 * @return uint32_t  the baud rate generator value
 */
uint32_t UART_ComputeBRGValue(UART *self, uint32_t desiredBaudRate, uint32_t clkInHz);

/***************************************************************************//**
 * @brief Initialize the UART
 * 
 * Use the parameters provided to set the necessary registers for your MCU.
 * 
 * @param self  pointer to the UART you are using
 * 
 * @param params  pointer to the UARTInitType that you are using
 */
void UART_Init(UART *self, UARTInitType *params);

/***************************************************************************//**
 * @brief A byte has been received. Set RTS pin, then call Rx callback.
 * 
 * This event is called whenever a byte is shifted into the received data
 * register. If you are using interrupts, this will be the function you call
 * wherever the receive interrupt is at. The interrupt flag should not be 
 * cleared at this point. This function will set the RTS flow control pin high,
 * then call the receive data callback function pointer.
 * 
 * @param self  pointer to the UART you are using
 */
void UART_ReceivedDataEvent(UART *self);

/***************************************************************************//**
 * @brief Get a byte, clear the Rx interrupt flag, and clear the RTS pin.
 * 
 * Typically, the Rx interrupt flag is cleared automatically when the data 
 * register is read. Set the RTS pin low to signal that we are ready to receive
 * more data.
 * 
 * @param self  pointer to the UART you are using
 * 
 * @return uint8_t  the data from the Rx register
 */
uint8_t UART_GetReceivedByte(UART *self);

/***************************************************************************//**
 * @brief Check if received data is available
 * 
 * This function will return true whenever a byte is shifted into the received
 * data register. If you are not using interrupts, you can poll this function. 
 * Then when data is ready, call the GetReceivedByte function.
 * 
 * @param self  pointer to the UART you are using
 * 
 * @return true if there is data in the receive register
 */
bool UART_IsReceiveRegisterFull(UART *self);

/***************************************************************************//**
 * @brief Enable the UART receiver
 * 
 * If you are using flow control, you will want to set the RTS pin low to 
 * signal that you are ready to receive data.
 * 
 * @param self  pointer to the UART you are using
 */
void UART_ReceiveEnable(UART *self);

/***************************************************************************//**
 * @brief Disable the UART receiver
 * 
 * If you are using flow control, you will want to set the RTS pin high to 
 * signal that you are not ready to receive data.
 * 
 * @param self  pointer to the UART you are using
 */
void UART_ReceiveDisable(UART *self);

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
 * @param self  pointer to the UART you are using
 */
void UART_TransmitFinishedEvent(UART *self);

/***************************************************************************//**
 * @brief Check CTS pin, place data in the Tx register, send
 * 
 * If the flow control is being used and the CTS pin is low, place the data in
 * transmit register. If interrupts are being used, turn on the Tx interrupt.
 * 
 * @param self  pointer to the UART you are using
 * 
 * @param dataToSend  the data you want to send out
 */
void UART_TransmitByte(UART *self, uint8_t dataToSend);

/***************************************************************************//**
 * @brief Check if the transmit register is empty and CTS is low
 * 
 * This function will return true whenever the contents of the Tx register have
 * been shifted out. If you are not using interrupts, you can poll this
 * function to know when the previous transmission is finished. Then when the 
 * register is empty, call TransmitByte. If you are using flow control, it is a 
 * good idea to check the CTS pin as well.
 * 
 * @param self  pointer to the UART you are using
 * 
 * @return true if the transmit register is empty
 */
bool UART_IsTransmitRegisterEmpty(UART *self);

/***************************************************************************//**
 * @brief Enable the UART transmitter
 * 
 * @param self  pointer to the UART you are using
 */
void UART_TransmitEnable(UART *self);

/***************************************************************************//**
 * @brief Disable the UART transmitter
 * 
 * @param self  pointer to the UART you are using
 */
void UART_TransmitDisable(UART *self);

/***************************************************************************//**
 * @brief Checks for pending events (transmit finished)
 * 
 * Because the transmit finished callback is called within the interrupt, if
 * the user wants to transmit another byte, the transmit finished interrupt
 * will almost certainly fire before the current callback is done. This can
 * lead to multiple recursive function calls. You will need to have a flag of 
 * some kind set whenever the TransmitFinishedEvent function is entered. If 
 * another Tx interrupt is called then set a pending interrupt flag. Then, call
 * this function in a loop continuously and it will check for a pending 
 * interrupt for you. This will let the stack unwind. Note that this is really 
 * only an issue if you are using interrupts to transmit.
 * 
 * @param self  pointer to the UART you are using
 */
void UART_PendingEventHandler(UART *self);

/***************************************************************************//**
 * @brief Set a function to be called whenever the transmit event happens
 * 
 * This function pointer is called from within the TransmitFinishedEvent
 * function, which is your "transmit register not empty" interrupt. Your 
 * function should follow the format listed below.
 * 
 * @param self  pointer to the UART you are using
 * 
 * @param Function  format: void SomeFunction(void)
 */
void UART_SetTransmitFinishedCallback(UART *self, void (*Function)(void));

/***************************************************************************//**
 * @brief Set a function to be called whenever a received data event happens
 * 
 * This callback function uses another function pointer as its argument. Copy 
 * the format listed below to create your function to be used with this 
 * callback. When your function is called, you will have a pointer to another
 * function passed to you. This function pointer "CallToGetData" has a uint8_t
 * return type. When you call it, you will be getting the actual data from the
 * UART. The reason I give you a function to call instead of the data itself, 
 * is because we don't want to mess up any flow control or interrupt flags in 
 * the UART before the user gets the actual data. Typically, the receive 
 * interrupt flag is cleared when the data is read out. So it's a simple method
 * of ensure that the data stays in the UART until the user calls for it. To
 * issue the callback in your implementation, you will typically give it a
 * reference to the GetReceivedByte function.
 * 
 * void MyFunction(uint8_t (*CallToGetData)void) {
 *      uint8_t data = CallToGetData();
 * }
 * 
 * // In the implementation:
 * void UART1_ReceivedDataEvent(void) {
 *      if(ReceivedDataCallbackPtr != NULL)
 *          ReceivedDataCallbackPtr(UART1_GetReceivedByte)
 * }
 * 
 * @param self  pointer to the UART you are using
 * 
 * @param Function  format: void SomeFunction(uint8_t (*CallToGetData)(void))
 */
void UART_SetReceivedDataCallback(UART *self, void (*Function)(uint8_t (*CallToGetData)(void)));

/***************************************************************************//**
 * @brief Set a function to check the CTS pin for the UART library
 * 
 * If the CTS pin is asserted (low) it means we are okay to send data to the 
 * other device. Your function should follow the format listed below. Your 
 * function should return true if the CTS pin is low
 * 
 * @param self  pointer to the UART you are using
 * 
 * @param Function  format: bool SomeFunction(void) // return true if CTS low
 */
void UART_SetIsCTSPinLowFunc(UART *self, bool (*Function)(void));

/***************************************************************************//**
 * @brief Set a function to allow the UART library to control the RTS pin
 * 
 * The RTS pin is asserted (low) whenever we are ready to receive data and
 * deasserted (high) whenever we are not ready to receive data. Your function 
 * should follow the format listed below. If the argument passed in is true, 
 * set the RTS pin high.
 * 
 * @param self  pointer to the UART you are using
 * 
 * @param Function  format: void SomeFunction(bool) // set pin high if true
 */
void UART_SetRTSPinFunc(UART *self, void (*Function)(bool setPinHigh));

#endif  /* IUART_H */
