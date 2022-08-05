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
 *      TODO
 * 
 ******************************************************************************/

#ifndef ADC_MANAGER_H
#define	ADC_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

#include "IADC.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

typedef struct ADCChannelEntryTag
{
    ADCChannel *channel;
    ADCChannelEntry *next;
} ADCChannelEntry;

/* Any file that includes this header can access the values of these channels
by calling ADC_Get8Bit or ADC_Get16Bit. */

// ----- Declare ADC channels here. Declare as extern --------------------------

extern ADCChannel analogInput1, analogInput2;


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Function Prototypes *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief 
 * 
 * @param sampleTimeMs  
 * 
 * @param tickRateMs  
 */
void ADC_Manager_Init(uint16_t sampleTimeMs, uint16_t tickRateMs);

/***************************************************************************//**
 * @brief 
 * 
 * @param self  
 * 
 * @param newChannel  
 */
void ADC_Manager_AddChannel(ADCChannelEntry *self, ADCChannel *newChannel);

/***************************************************************************//**
 * @brief 
 * 
 */
void ADC_Manager_Tick(void);

/***************************************************************************//**
 * @brief 
 * 
 */
void ADC_Manager_Enable(void);

/***************************************************************************//**
 * @brief 
 * 
 */
void ADC_Manager_Disable(void);

#endif	/* ADC_MANAGER_H */