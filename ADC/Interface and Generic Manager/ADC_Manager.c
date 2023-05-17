/***************************************************************************//**
 * @brief ADC Manager (Non-Processor Specific)
 * 
 * @file ADC_Manager.c
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 2/13/22   Original creation
 * @date 2/19/22   Modified initialization routine
 * @date 7/23/22   Modified for interface updates
 * 
 * @details
 *      A simple implementation of the ADC_Manager library that handles taking
 * sample for you. It manages a linked list, then goes around the list one by 
 * one calling the ADC_TakeSample function.
 * 
 * There will be one ADCChannel and one ADCChannelEntry for each channel used. 
 * Only the ADCChannel needs to be made extern. Any file that needs access to
 * the ADC channel's value can use the ADCChannel by including the ADC_Manager
 * header file.
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

#include "ADC_Manager.h"
#include <stddef.h> // needed for NULL

// ***** Defines ***************************************************************

#define ADC_MANAGE_SAMPLE_MS    5 // time to wait for a sample to finish
#define ADC_MANAGE_TICK_MS      1

// ***** Global Variables ******************************************************

static ADCChannelEntry *ptrToLast = NULL;       // circular linked list
static ADCChannelEntry *currentEntry = NULL;  // index for linked list
static bool adcManagerEnabled = true;

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Initialization ADC Channels *****************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/* Declare ADC channels and entries starting here. The entry variable is only 
for use by the manager. Each ADCChannel object should have a matching extern 
declaration in a header file. Use a memorable name. You will call ADC_Get8bit 
or ADC_Get16Bit with this ADCChannel object to get its value. */

ADCChannel analogInput1, analogInput2;
ADCChannelEntry entry1, entry2;


// ***** Static Function Prototypes ********************************************

/* Put static function prototypes here */
static void ADC_Manager_ChannelPush(ADCChannelEntry *self, ADCChannel *newChannel);
static void ADC_Manager_InsertChannelAfter(ADCChannelEntry *entryToInsert, ADCChannelEntry *prev, ADCChannel *newChannel);

// *****************************************************************************

void ADC_Manager_Init(uint16_t sampleTimeMs, uint16_t tickRateMs)
{
    analogInput1.channelNumber = 4;
    analogInput2.channelNumber = 5;

// ----- Add your channels to the manager --------------------------------------

    ADC_Manager_AddChannel(&entry1, &analogInput1);
    ADC_Manager_AddChannel(&entry2, &analogInput2);

// -----------------------------------------------------------------------------

    /* Initialize the ADC peripheral */
    ADC_UseNonBlockingMode(sampleTimeMs, tickRateMs);
    ADC_InitPeripheral();
    ADC_Manager_Enable();
}

// *****************************************************************************

void ADC_Manager_AddChannel(ADCChannelEntry *self, ADCChannel *newChannel)
{
    if(ptrToLast == NULL)
    {
        /* Begin with empty list */
        self->channel = newChannel;

        /* Update the pointer to point to the last entry, which is itself */
        ptrToLast = self;

        /* Since the list only contains one entry, the "next" pointer will
        also point to itself */
        self->next = ptrToLast;

        /* Set index to the beginning */
        currentEntry = ptrToLast->next;
    }
    else
    {
        ADC_Manager_ChannelPush(self, newChannel);
        currentEntry = ptrToLast->next; // reset the index
    }

    /* Initialize the new channel */
    ADC_InitChannel(newChannel, newChannel->channelNumber);
}

// *****************************************************************************

void ADC_Manager_Tick(void)
{
    ADC_Tick();

    /* Go round-robin through the list */
    if(adcManagerEnabled && !ADC_IsBusy() && currentEntry != NULL)
    {
        ADC_TakeSample(currentEntry->channel);
        currentEntry = currentEntry->next;
    }
}

// *****************************************************************************

void ADC_Manager_Enable(void)
{
    if(!ADC_IsEnabled())
        ADC_Enable();
    
    adcManagerEnabled = true;
}

// *****************************************************************************

void ADC_Manager_Disable(void)
{
    adcManagerEnabled = false;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Static Functions ****************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief 
 * 
 * @param self 
 * 
 * @param newChannel 
 */
static void ADC_Manager_ChannelPush(ADCChannelEntry *self, ADCChannel *newChannel)
{
    /* Store the new data */
    self->channel = newChannel;
    /* Add the new entry to the beginning of the list. The last entry's "next" 
    pointer will always point to the beginning of the list */
    self->next = ptrToLast->next;
    /* Update the beginning entry to point to the new beginning */
    ptrToLast->next = self;
}

/***************************************************************************//**
 * @brief 
 * 
 * @param entryToInsert 
 * 
 * @param prev 
 * 
 * @param newChannel 
 */
static void ADC_Manager_InsertChannelAfter(ADCChannelEntry *entryToInsert, ADCChannelEntry *prev, ADCChannel *newChannel)
{
    if(prev == NULL || entryToInsert == NULL)
    {
        return;
    }
    /* Store the new data */
    entryToInsert->channel = newChannel;
    /* Insert the new entry */
    entryToInsert->next = prev->next;
    /* Change the previous entry to point to our new entry */
    prev->next = entryToInsert;
}

/*
 End of File
 */