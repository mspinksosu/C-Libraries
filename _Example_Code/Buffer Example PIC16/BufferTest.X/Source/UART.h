/*******************************************************************************
 * Title: Basic UART Header
 * 
 * Author: Matthew Spinks
 * 
 * File: UART.h
 * 
 * Description:
 * 
 * ****************************************************************************/

#ifndef IUART_H
#define	IUART_H

#include <stdint.h>

/* ----- Initialize UART -------------------------------------------------------
 * 
 * Parameters:
 *      None
 * 
 * Returns:
 *      None
 */
void UARTInit(void);

/* ----- Enable UART Receive ---------------------------------------------------
 * 
 * Parameters:
 *      None
 * 
 * Returns:
 *      None
 */
void UARTReceiveEnable(void);

/* ----- Disable UART Receive --------------------------------------------------
 * 
 * Parameters:
 *      None
 * 
 * Returns:
 *      None
 */
void UARTReceiveDisable(void);

/* ----- UART Receive Interrupt ------------------------------------------------
 * 
 * To be called upon when a character is received from the UART. Either via 
 * an interrupt, or by other means. From here you must either do something
 * with the data, pass it along to another event handler.
 * 
 * Parameters:
 *      None
 * 
 * Returns:
 *      None
 */
void UARTReceiveInterrupt(void);

/* ----- UART Receive ----------------------------------------------------------
 * 
 * Parameters:
 *      None
 * 
 * Returns:
 *      The data from the UART register
 */
uint8_t UARTGetReceivedChar(void);

/* ----- Enable UART Transmit --------------------------------------------------
 * 
 * Parameters:
 *      None
 * 
 * Returns:
 *      None
 */
void UARTTransmitEnable(void);

/* ----- Disable UART Transmit -------------------------------------------------
 * 
 * Parameters:
 *      None
 * 
 * Returns:
 *      None
 */
void UARTTransmitDisable(void);

/* ----- UART Transmit ---------------------------------------------------------
 * 
 * First, it prepares a byte to be transmitted by UART. Either by an interrupt, 
 * or other means. Then it should enable the transmitter
 * 
 * Parameters:
 *      The data to transmit over the UART. The register names will be
 *      implemented based on the processor.
 * 
 * Returns:
 *      None.
 */
void UARTTransmitChar(uint8_t);

/* ----- UART Transmit Finished ------------------------------------------------
 * 
 * To be called upon whenever UART transmission has finished. If you are using 
 * interrupts, this would be typically be your TX interrupt function.
 * 
 * Parameters:
 *      None
 * 
 * Returns:
 *      None.
 */
void UARTTransmitFinished(void);

/* ----- UART Transmit Finished ------------------------------------------------
 * 
 * 
 * 
 * Parameters:
 *      None
 * 
 * Returns:
 *      None.
 */

void SetUARTTransmitFinishedCallback(void (*Function)(void));
void SetUARTReceiveInterruptCallback(void (*Function)(void));


#endif	/* IUART_H */

