/***************************************************************************//**
 * @brief ADC Manager (Non-Processor Specific)
 * 
 * @author Matthew Spinks
 * 
 * @date 2/15/22  Original creation
 * @date 2/19/22  Modified to use array for initialization
 * 
 * @file ADC_Manager.c
 * 
 * @details
 *      TODO
 * 
 ******************************************************************************/

#include "ADC_Manager.h"

// ***** Defines ***************************************************************

#define ADC_MANAGE_SAMPLE_MS    5
#define ADC_MANAGE_TICK_MS      1

// ***** Global Variables ******************************************************

static ADC_Channel_Entry *ptrToLast = NULL;
static ADC_Channel_Entry *currentChannel = NULL;
static bool adcManagerEnabled;

typedef struct ADC_Channel_FullTag
{
    ADC_Channel **ptrToChannel;
    ADC_Channel adcChannel;
    ADC_Channel_Entry entry;
} ADC_Channel_Full;

// ***** Function Prototypes ***************************************************

/* Put static function prototypes here */
static void ADC_Manager_ChannelPush(ADC_Channel_Entry *self, ADC_Channel *newChannel);
static void ADC_Manager_InsertChannelAfter(ADC_Channel_Entry *entryToInsert, ADC_Channel_Entry *prev, ADC_Channel *newChannel);

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Initialization ADC Channels *****************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/* Declare pointers for adc channels starting here. Each variable should have a 
matching extern declaration in a header file. Use a memorable name. This is the 
pointer you will use for library function calls. Also, don't forget to set the 
corresponding GPIO to analog. */
//------------------------------------------------------------------------------

// ADC_Channel pot = {.channelNumber = 4};
// ADC_ChannelEntry _pot;
//
ADC_Channel *analogPin1;

//------------------------------------------------------------------------------

/* Initialize your channels here. Set each channel's pointer member to the 
external pointer that you wish to use for it */
static ADC_Channel_Full ChannelArray[] = {
{   .ptrToChannel = &analogPin1,
    .adcChannel.channelNumber = 4} /*,
    
{   .ptrToChannel = &analogPin2,
    .adcChannel.channelNumber = 5},*/ };
                           
// -----------------------------------------------------------------------------

// TODO experiment
static uint8_t numChannels = (sizeof(ChannelArray)/sizeof(ChannelArray[0]));

/***************************************************************************//**
 * @brief 
 * 
 * @param sampleTimeMs  
 * 
 * @param tickRateMs  
 */
void ADC_Manager_Init(uint16_t sampleTimeMs, uint16_t tickRateMs)
{
// ----- Add your channels to the list -----------------------------------------
      
    // ADC_Manager_AddChannel(&potEntry, &potADC);

    for(uint8_t i = 0; i < numChannels; i++)
    {
        if(ChannelArray[i].ptrToChannel != NULL)
        {
            *(ChannelArray[i].ptrToChannel) = &(ChannelArray[i].adcChannel);
            ADC_Manager_AddChannel(&(ChannelArray[i].entry), &(ChannelArray[i].adcChannel));
        }
    }

// -----------------------------------------------------------------------------
    
    /* Initialize the ADC peripheral */
    ADC_InitPeripheralNonBlocking(sampleTimeMs, tickRateMs);

    ADC_Manager_Enable();
}

/***************************************************************************//**
 * @brief 
 * 
 * @param self  
 * 
 * @param newChannel  
 */
void ADC_Manager_AddChannel(ADC_Channel_Entry *self, ADC_Channel *newChannel)
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
        currentChannel = ptrToLast->next;
    }
    else
    {
        ADC_Manager_ChannelPush(self, newChannel);
        currentChannel = ptrToLast->next;
    }

    /* Initialize the new channel */
    ADC_InitChannel(newChannel, newChannel->channelNumber);
}

/***************************************************************************//**
 * @brief 
 * 
 */
void ADC_Manager_Tick(void)
{
    ADC_Tick();

    /* Go round-robin through the list */
    if(!ADC_IsBusy() && currentChannel != NULL)
    {
        ADC_TakeSample(currentChannel);
        currentChannel = currentChannel->next;
    }
}

/***************************************************************************//**
 * @brief 
 * 
 */
void ADC_Manager_Enable(void)
{
    adcManagerEnabled = true;
}

/***************************************************************************//**
 * @brief 
 * 
 */
void ADC_Manager_Disable(void)
{
    adcManagerEnabled = false;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Static Functions ****************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

static void ADC_Manager_ChannelPush(ADC_Channel_Entry *self, ADC_Channel *newChannel)
{
    /* Store the new data */
    self->channel = newChannel;

    /* Add the new entry to the beginning of the list. The last entry's "next" 
    pointer will always point to the beginning of the list */
    self->next = ptrToLast->next;

    /* Update the beginning entry to point to the new beginning */
    ptrToLast->next = self;
}

static void ADC_Manager_InsertChannelAfter(ADC_Channel_Entry *entryToInsert, ADC_Channel_Entry *prev, ADC_Channel *newChannel)
{
    if(prev == NULL || entryToInsert == NULL)
    {
        return;
    }
    // Store the new data
    entryToInsert->channel = newChannel;
    // Insert the new entry
    entryToInsert->next = prev->next;
    // Change the previous entry to point to our new entry
    prev->next = entryToInsert->next;
}

/*
 End of File
 */