/***************************************************************************//**
 * @brief ADC Library Implementation (STM32G0 with DMA)
 * 
 * @author Matthew Spinks
 * 
 * @date 2/19/22  Original creation
 * @date 7/23/22  Interface updates
 * 
 * @file ADC_STM32G0_DMA.c
 * 
 * @details
 *      An ADC library for STM32G0 that implements the functions listed in the 
 * ADC interface (IADC.h) and has functions related to the DMA controller.
 *
 * This implementation is intended to work with the ADC_Manager_STM32G0_DMA.c 
 * file which is a version of ADC_Manager that uses the G0's DMA controller.
 * Technically, you don't need the DMA controller to use this library. In fact, 
 * it doesn't even include the DMA header files. That is because the DMA 
 * settings related to the ADC are contained in the G0's ADC header files. When 
 * you call the take sample function, it will pause the DMA controller, save
 * its settings, take the sample, then restore its settings.
 * 
 ******************************************************************************/

#include "IADC.h"

/* Include processor specific header files here */
#include "stm32g071xx.h"
#include "stm32g0xx_ll_adc.h"

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
static void ADC_STM32G0_SaveRegisterSettings(void);
static void ADC_STM32G0_RestoreRegisterSettings(void);

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

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
    /* TODO do I want to stop if there is a conversion going on? */
    if(adcFlags.start == 0 && adcFlags.active == 0)
    {
        /* If you have DMA or something else set up, you need to set the ADC 
        disable callback function to pause the DMA sequence appropriately,
        because I'm about to stop it. I will save the DMA settings for you
        though */
        if(LL_ADC_REG_GetDMATransfer != LL_ADC_REG_DMA_TRANSFER_NONE)
        {
            ADC_STM32G0_SaveRegisterSettings();
            ADC_Disable(); // includes callback function
            LL_ADC_REG_SetDMATransfer(ADC1, LL_ADC_REG_DMA_TRANSFER_NONE);
            LL_ADC_REG_SetTriggerSource(ADC1,LL_ADC_REG_TRIG_SOFTWARE);
            restoreSettingsAfterFinish = true;
        }

        /* Load the channel to start the conversion */
        currentChannel = self;

        /* To convert a single channel with the STM32, you must program
        a sequence with a length of 1. Ref Man: 15.3.10 */
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
            if(restoreSettingsAfterFinish)
            {
                restoreSettingsAfterFinish = false;
                /* If you had some sort of DMA transfer or something set up,
                it had to be paused. Set up the enable callback function to 
                re-enable anything you had going before. */
                ADC_STM32G0_RestoreRegisterSettings();
                ADC_Enable();
            }
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

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Local Functions *****************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Store the settings for the ADC and channels
 * 
 */
static void ADC_STM32G0_SaveRegisterSettings(void)
{
    configReg1 = ADC1->CFGR1;
    chanSelReg = ADC1->CHSELR;
}

/***************************************************************************//**
 * @brief Restore the settings for the ADC and channels
 * 
 */
static void ADC_STM32G0_RestoreRegisterSettings(void)
{
    ADC1->CFGR1 = configReg1;
    ADC1->CHSELR = chanSelReg;
}

/*
 End of File
 */