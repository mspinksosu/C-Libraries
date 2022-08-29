/***************************************************************************//**
 * @brief ADC Manager Header (Non-Processor Specific)
 * 
 * @author Matthew Spinks
 * 
 * @date 2/13/22  Original creation
 * @date 2/24/22  Added functions for enable and disable
 * @date 7/23/22  Modified for interface updates
 * 
 * @file ADC_Manager.h
 * 
 * @details
 *      A library that handles taking ADC samples for you. It also handles 
 * access to the ADC Channels for other files as well. This library uses the
 * ADC Channel object and calls functions listed in IADC.h
 * 
 * This library manages a linked list of ADC Channel objects. The entries in
 * the list are ADCChannelEntry which is an ADCChannel and a pointer to the 
 * next channel. The ADC channels and their entries are declared and 
 * initialized in the ADC_Manger.c file, then added to the list. 
 * 
 * To use this library, first you must call the init function and give it the 
 * sample time in milliseconds and the update rate in milliseconds (how often 
 * you plan to call the ADC_Manager_Tick function). Inside the initialization 
 * function, the ADC channels and ADC channel entries are added to the list. 
 * To add a channel to the list, you must call the ADC_Manager_AddChannel 
 * function and give it the ADCChannel you intialized and its ADCChannelEntry 
 * you declared.
 * 
 * The ADC_Manger sets up non-blocking mode for the ADC then calls the ADC
 * initialization for you. The sample time you give the ADC_Manager is how 
 * long to wait for an ADC channel to finish getting a sample. The ADC_Manager
 * tick function must be called periodically. This function will, in turn, call
 * the ADC_Tick function. The ADC_Manager then goes around the list, taking a 
 * sample for each channel. The result is stored in the ADCChannel object.
 * 
 * Each external ADCChannel object declared here will need a matching 
 * declaration in the ADC_Manager.c file, or in your own processor specific 
 * ADC_Manager.c file if you choose to make one. Only the ADCChannel objects 
 * need to be made extern, not the ADCChannelEntry objects. Any file that needs 
 * access to the ADC channels values can include this header file and then
 * call either ADC_Get16Bit or ADC_Get8Bit to get the value. 
 * 
 ******************************************************************************/

#ifndef ADC_MANAGER_H
#define ADC_MANAGER_H

#include "IADC.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

typedef struct ADCChannelEntryTag
{
    ADCChannel *channel;
    ADCChannelEntry *next;
} ADCChannelEntry;

/* Any file that includes this header can access the values of these channels
declared below by calling ADC_Get8Bit or ADC_Get16Bit. */

// ----- Declare ADC channels here. Declare as extern --------------------------

extern ADCChannel analogInput1, analogInput2;


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Function Prototypes *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Initialize all ADC channels and entries then initialize the ADC
 * 
 * This function sets up non-blocking mode for the ADC and calls the ADC's
 * initialization function.
 * 
 * @param sampleTimeMs  the amount of time to wait for a sample to finish
 * 
 * @param tickRateMs  how often you call the ADC_Manager_Tick function
 */
void ADC_Manager_Init(uint16_t sampleTimeMs, uint16_t tickRateMs);

/***************************************************************************//**
 * @brief Add a channel entry and link it to an ADCChannel object
 * 
 * This function will call the ADC_InitChannel function after setting up the 
 * ADCChannelEntry. Therefore, you must set the channelNumber member of the 
 * ADCChannel object before calling this function.
 * 
 * @param self  pointer to an ADCChannelEntry object you declared
 * 
 * @param newChannel  pointer to an ADCChannel object
 */
void ADC_Manager_AddChannel(ADCChannelEntry *self, ADCChannel *newChannel);

/***************************************************************************//**
 * @brief Update the ADC_Managers timers and then call ADC_Tick
 * 
 * Checks if the current channel is busy taking a sample. If it is, continue. 
 * If the sample is finished, load the next channel in the list and call the
 * ADC_TakeSample function.
 */
void ADC_Manager_Tick(void);

/***************************************************************************//**
 * @brief Enable the ADC Manager
 * 
 * Unpause the execution of the ADC Manager's loop if it is paused. Check if
 * the ADC is disabled and enable it if needed.
 */
void ADC_Manager_Enable(void);

/***************************************************************************//**
 * @brief Disable the ADC Manager
 * 
 * Pauses the execution of the ADC Manager loop. This should not disable the 
 * ADC peripheral.
 */
void ADC_Manager_Disable(void);

#endif  /* ADC_MANAGER_H */