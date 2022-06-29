/***************************************************************************//**
 * @brief ADC Interface Header File
 * 
 * @author Matthew Spinks
 * 
 * @date 2/11/22  Original creation
 * @date 2/24/22  Added function callbacks for enable and disable
 * 
 * @file IADC.h
 * 
 * @details
 *      TODO
 * 
 ******************************************************************************/

#ifndef IADC_H
#define	IADC_H

#include <stdint.h>
#include <stdbool.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

typedef struct ADC_ChannelTag
{
    uint8_t channelNumber;
    uint16_t adcValue;
} ADC_Channel;

/** 
 * value            The ADC value for that channel
 * 
 * channelNumber    The ADC channel number
 */

// ***** Function Prototypes ***************************************************

/***************************************************************************//**
 * @brief Initialize necessary registers for this MCU
 * 
 */
void ADC_InitPeripheral(void);

/***************************************************************************//**
 * @brief Initialize the ADC with a timer to allow for non-blocking use
 * 
 * Calls the ADC init function. Requires you to call the ADC tick function 
 * periodically in order to upate the timer. The timer will check ongoing
 * conversions and perform a callback if needed
 * 
 * @param sampleTimeMs  the amount of time to wait for a sample to finish
 * 
 * @param tickRateMs  how often you plan to call the tick function
 */
void ADC_InitPeripheralNonBlocking(uint16_t sampleTimeMs, uint16_t tickRateMs);

/***************************************************************************//**
 * @brief Initialize an ADC channel object
 * 
 * @param self  pointer to the ADC channel object you are using
 * 
 * @param channelNumber  the channel number for this MCU
 */
void ADC_InitChannel(ADC_Channel *self, uint8_t channelNumber);

/***************************************************************************//**
 * @brief Start an ADC conversion
 * 
 * Load the channel given by the ADC_Channel object and start the conversion
 * 
 * @param self  pointer to the ADC channel object you are using
 */
void ADC_TakeSample(ADC_Channel *self);

/***************************************************************************//**
 * @brief Check if the ADC is busy
 * 
 * @return  true if busy
 */
bool ADC_IsBusy(void);

/***************************************************************************//**
 * @brief Get the channel the ADC is currently processing
 * 
 * @return ADC_Channel*  pointer to the current ADC channel
 */
ADC_Channel *ADC_GetCurrentChannel(void);

/***************************************************************************//**
 * @brief Get the channel number the ADC is currently processing
 * 
 * @return uint8_t  ADC channel number
 */
uint8_t ADC_GetCurrentChannelNumber(void);

/***************************************************************************//**
 * @brief Get the 16 bit value (left-justified) for a channel
 * 
 * @param self  pointer to the ADC channel object you are using
 * 
 * @return uint16_t  left-justified result
 */
uint16_t ADC_Get16Bit(ADC_Channel *self);

/***************************************************************************//**
 * @brief Get the 8 bit value for a channel
 * 
 * @param self  pointer to the ADC channel object you are using
 * 
 * @return uint8_t  8-bit result
 */
uint8_t ADC_Get8Bit(ADC_Channel *self);

/***************************************************************************//**
 * @brief Enable the ADC peripheral
 * 
 */
void ADC_Enable(void);

/***************************************************************************//**
 * @brief Disable the ADC peripheral
 * 
 */
void ADC_Disable(void);

/***************************************************************************//**
 * @brief Update the ADC conversion timers
 * 
 * You must call this function periodically. The update rate is determined by 
 * using the non-blocking ADC initialization function. The timer will check 
 * ongoing conversions and perform a callback if needed
 */
void ADC_Tick(void);

/***************************************************************************//**
 * @brief Check if the ADC is enabled
 * 
 * @return true  if the ADC is enabled
 */
bool ADC_IsEnabled(void);

/***************************************************************************//**
 * @brief A function pointer that is called when a conversion is finished
 * 
 * The context is so that multiple callbacks can be serviced by the same
 * function if desired.
 * 
 * @param CallbackFunc format: void SomeFunction(ADC_Channel *context)
 */
void ADC_SetSampleFinishedCallbackFunc(void (*CallbackFunc)(ADC_Channel *context));

/***************************************************************************//**
 * @brief A function pointer that is called after the ADC is enabled
 * 
 * Useful for turning on or resetting DMA for example
 * 
 * @param Function  format: void SomeFunction(void)
 */
void ADC_SetEnableFinishedCallbackFunc(void (*Function)(void));

/***************************************************************************//**
 * @brief A function pointer that is called after the ADC is disabled
 * 
 * Useful for turning off or resetting DMA for example
 * 
 * @param Function  format: void SomeFunction(void)
 */
void ADC_SetDisableFinishedCallbackFunc(void (*Function)(void));

#endif	/* IADC_H */