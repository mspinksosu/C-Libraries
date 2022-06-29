/***************************************************************************//**
 * @brief ADC Manager Header
 * 
 * @author Matthew Spinks
 * 
 * @date 2/13/22  Original creation
 * @date 2/24/22  Added functions for enable and disable
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

typedef struct ADC_Channel_EntryTag
{
    ADC_Channel *channel;
    ADC_Channel_Entry *next;
} ADC_Channel_Entry;

/** 
 * Description of struct TODO
 * 
 * member1      description of variable member1
 * 
 */

// ***** Function Prototypes ***************************************************

void ADC_Manager_Init(  uint16_t sampleTimeMs, uint16_t tickRateMs);

void ADC_Manager_AddChannel(ADC_Channel_Entry *self, ADC_Channel *newChannel);

void ADC_Manager_Tick(void);

void ADC_Manager_Enable(void);

void ADC_Manager_Disable(void);

#endif	/* ADC_MANAGER_H */