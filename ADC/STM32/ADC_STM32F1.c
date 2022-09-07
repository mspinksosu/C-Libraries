/***************************************************************************//**
 * @brief ADC Library Implementation (STM32F1)
 * 
 * @author Matthew Spinks
 * 
 * @date 7/24/22  Original creation (ported from G0 code)
 * 
 * @file ADC_STM32F1.c
 * 
 * @details
 *      TODO Finish porting over settings
 * 
 ******************************************************************************/

#include "IADC.h"

/* Include processor specific header files here */


// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

static bool useNonBlocking = false, restoreSettingsAfterFinish = false;
static ADCChannel *currentChannel;
static uint16_t periodInTicks, adcCounter;
static uint32_t configReg1, chanSelReg;

static union {
        struct {
            unsigned start      :1;
            unsigned active     :1;
            unsigned expired    :1;
            unsigned            :0; // fill to nearest byte
            };
        } adcFlags;

// local function pointers
static void (*ADC_SampleFinishedCallbackFunc)(ADCChannel *context);
static void (*ADC_PeripheralEnabledCallbackFunc)(void);
static void (*ADC_PeripheralDisabledCallbackFunc)(void);

// ***** Static Function Prototypes ********************************************

/* Put static functions here */


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

// *****************************************************************************

void ADC_InitPeripheral(void)
{
        LL_ADC_InitTypeDef ADC_InitStruct = {0};
        LL_ADC_REG_InitTypeDef ADC_REG_InitStruct = {0};

        /* Set up ADC for 12-bit, single conversion mode, software trigger */

        /* Peripheral clock enable */
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC);

        ADC_InitStruct.DataAlignment = LL_ADC_DATA_ALIGN_LEFT;
        ADC_InitStruct.Clock = LL_ADC_CLOCK_SYNC_PCLK_DIV4;
        ADC_InitStruct.Resolution = LL_ADC_RESOLUTION_12B;
        ADC_InitStruct.LowPowerMode = LL_ADC_LP_MODE_NONE;
        LL_ADC_Init(ADC1, &ADC_InitStruct);

        /* Initial set up is for single transfer with software trigger */
        LL_ADC_ClearFlag_CCRDY(ADC1);
        ADC_REG_InitStruct.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;
        ADC_REG_InitStruct.SequencerLength = LL_ADC_REG_SEQ_SCAN_DISABLE;
        ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
        ADC_REG_InitStruct.ContinuousMode = LL_ADC_REG_CONV_SINGLE;
        ADC_REG_InitStruct.DMATransfer = LL_ADC_REG_DMA_TRANSFER_NONE;
        ADC_REG_InitStruct.Overrun = LL_ADC_REG_OVR_DATA_OVERWRITTEN;
        LL_ADC_REG_SetSequencerConfigurable(ADC1, LL_ADC_REG_SEQ_FIXED); // CHSELR_MOD = 0
        LL_ADC_REG_Init(ADC1, &ADC_REG_InitStruct);

        LL_ADC_SetOverSamplingScope(ADC1, LL_ADC_OVS_DISABLE);
        LL_ADC_SetTriggerFrequencyMode(ADC1, LL_ADC_CLOCK_FREQ_MODE_HIGH);
        LL_ADC_SetSamplingTimeCommonChannels(ADC1, LL_ADC_SAMPLINGTIME_COMMON_1, LL_ADC_SAMPLINGTIME_39CYCLES_5);
        LL_ADC_SetSamplingTimeCommonChannels(ADC1, LL_ADC_SAMPLINGTIME_COMMON_2, LL_ADC_SAMPLINGTIME_39CYCLES_5);
        LL_ADC_DisableIT_EOC(ADC1);
        LL_ADC_DisableIT_EOS(ADC1);
        LL_ADC_EnableInternalRegulator(ADC1);

        /* Enable ADC */
        LL_ADC_ClearFlag_ADRDY(ADC1); // clear flag by writing a 1
        LL_ADC_Enable(ADC1); // enable bit ADC_CR.ADEN
        while(LL_ADC_IsActiveFlag_ADRDY(ADC1) == 0){}
        
        /* Wait 20 us for the regulator to stabilize */
        uint32_t wait_loop_index;
        wait_loop_index = ((20UL * (SystemCoreClock / (100000 * 2))) / 10);
        while(wait_loop_index != 0)
        {
            wait_loop_index--;
        }
}

// *****************************************************************************

void ADC_UseNonBlockingMode(uint16_t sampleTimeMs, uint16_t tickRateMs)
{
    if(tickRateMs != 0)
        periodInTicks = sampleTimeMs / tickRateMs;
    else
        periodInTicks = 1;

    useNonBlocking = true;
}

// *****************************************************************************

void ADC_UseBlockingMode(void)
{
    useNonBlocking = false;
}

// *****************************************************************************

void ADC_InitChannel(ADCChannel *self, uint8_t channelNumber)
{
    self->channelNumber = channelNumber;
}

// *****************************************************************************

void ADC_TakeSample(ADCChannel *self)
{
    if(adcFlags.start == 0 && adcFlags.active == 0)
    {
        /* Load the channel to start the conversion */
        currentChannel = self;

        ADC1->CHSELR = (1 << currentChannel->channelNumber);

        /* Enable the ADC if necessary */
        if(LL_ADC_IsActiveFlag_ADRDY(ADC1) == 0)
        {
            LL_ADC_ClearFlag_ADRDY(ADC1); // clear flag by writing a 1
            LL_ADC_Enable(ADC1); // enable bit ADC_CR.ADEN
            while(LL_ADC_IsActiveFlag_ADRDY(ADC1) == 0){}
        }

        /* After modifying the channel select register, you must wait 
        for the CCRDY flag Ref Man: 15.3.8 */
        while (LL_ADC_IsActiveFlag_CCRDY(ADC1) == 0){}
        LL_ADC_ClearFlag_CCRDY(ADC1);

        LL_ADC_ClearFlag_EOS(ADC1);
        LL_ADC_REG_StartConversion(ADC1);
        adcFlags.start = 1;

        if(!useNonBlocking)
        {
            /* wait for ADC conversion to finish */
            while (LL_ADC_IsActiveFlag_EOC(ADC1) == 0){}
            LL_ADC_ClearFlag_EOC(ADC1);
            currentChannel->adcValue = LL_ADC_REG_ReadConversionData12(ADC1);
            adcFlags.start = 0;
            adcFlags.active = 0;
            // if(restoreSettingsAfterFinish)
            // {
            //     restoreSettingsAfterFinish = false;
            //     /* If you had some sort of DMA transfer or something set up,
            //     it had to be paused. Set up the enable callback function to 
            //     re-enable anything you had going before. */
            //     ADC_STM32G0_RestoreRegisterSettings();
            //     ADC_Enable();
            // }
        }
    }
}

// *****************************************************************************

bool ADC_IsBusy(void)
{
    if(adcFlags.start && adcFlags.active)
        return true;
    else
        return false;
}

// *****************************************************************************

ADCChannel* ADC_GetCurrentChannel(void)
{
    return currentChannel;
}

// *****************************************************************************

uint8_t ADC_GetCurrentChannelNumber(void)
{
    return currentChannel->channelNumber;
}

// *****************************************************************************

uint16_t ADC_Get16Bit(ADCChannel *self)
{
    return self->adcValue;
}

// *****************************************************************************

uint8_t ADC_Get8Bit(ADCChannel *self)
{
    return (uint8_t)((self->adcValue) >> 8);
}

// *****************************************************************************

void ADC_Enable(void)
{
    if(LL_ADC_IsActiveFlag_ADRDY(ADC1) == 0)
    {
        LL_ADC_ClearFlag_ADRDY(ADC1); // clear flag by writing a 1
        LL_ADC_Enable(ADC1); // enable bit ADC_CR.ADEN
        while(LL_ADC_IsActiveFlag_ADRDY(ADC1) == 0){}
    }

    if(ADC_PeripheralEnabledCallbackFunc)
    {
        ADC_PeripheralEnabledCallbackFunc();
    }
}

// *****************************************************************************

void ADC_Disable(void)
{
    LL_ADC_REG_StopConversion(ADC1);
    while(LL_ADC_REG_IsStopConversionOngoing(ADC1)){}
    LL_ADC_Disable(ADC1); // disable bit ADC_CR.ADDIS
    LL_ADC_ClearFlag_ADRDY(ADC1); // clear flag by writing a 1
    adcFlags.start = 0;
    adcFlags.active = 0;

    if(ADC_PeripheralDisabledCallbackFunc)
    {
        ADC_PeripheralDisabledCallbackFunc();
    }
}

// *****************************************************************************

void ADC_Tick(void)
{
    /* Start the conversion timer */
    if(adcFlags.start)
    {
        adcFlags.start = 0;
        adcFlags.active = 1;
        adcCounter = periodInTicks;
    }

    /* Check active conversion. Either until the conversion finishes or we 
    hit our timer value */
    if(adcFlags.active)
    {
        adcCounter--;
        if(adcCounter == 0 || LL_ADC_IsActiveFlag_EOS(ADC1) == 0)
        {
            /* Load the value into the current channel */
            currentChannel->adcValue = LL_ADC_REG_ReadConversionData12(ADC1);
            LL_ADC_ClearFlag_EOC(ADC1);
            adcFlags.active = 0;

            if(restoreSettingsAfterFinish)
            {
                restoreSettingsAfterFinish = false;
                /* If you had some sort of DMA transfer or something set up,
                it had to be paused. Set up the enable callback function to 
                re-enable anything you had going before. */
                ADC_STM32G0_RestoreRegisterSettings();
                ADC_Enable();
            }

            if(ADC_SampleFinishedCallbackFunc)
            {
                ADC_SampleFinishedCallbackFunc(currentChannel);
            }
        }
    }
}

// *****************************************************************************

bool ADC_IsEnabled(void)
{   
    if(LL_ADC_IsEnabled(ADC1) == 1) // ADC_CR.ADEN
        return true;
    else
        return false;
}

// *****************************************************************************

void ADC_SetSampleFinishedCallbackFunc(void (*CallbackFunc)(ADCChannel *context))
{
    ADC_SampleFinishedCallbackFunc = CallbackFunc;
}

// *****************************************************************************

void ADC_SetPeripheralEnabledCallbackFunc(void (*Function)(void))
{
    ADC_PeripheralEnabledCallbackFunc = Function;
}

// *****************************************************************************

void ADC_SetPeripheralDisabledCallbackFunc(void (*Function)(void))
{
    ADC_PeripheralDisabledCallbackFunc = Function;
}

/*
 End of File
 */