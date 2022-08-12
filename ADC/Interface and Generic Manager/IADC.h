/***************************************************************************//**
 * @brief ADC Interface Header File
 * 
 * @author Matthew Spinks
 * 
 * @date 2/11/22  Original creation
 * @date 2/24/22  Added function callbacks for enable and disable
 * @date 7/23/22  Made blocking and non-blocking options independent
 * 
 * @file IADC.h
 * 
 * @details
 *      An interface for an ADC library that can be used with different
 * processors. This is a simple type of interface with no inheritance or 
 * function tables involved. In order to use this interface, you must have an 
 * implementation (.c file) which has all of the function listed implemented 
 * for your specific processor.
 * 
 * An ADC channel is comprised of the adc channel number and a variable which
 * will hold its current value. First, the ADC init peripheral function is 
 * called which sets up any processor specific registers for your ADC. After 
 * that, you typically set up either non-blocking or blocking mode. Blocking
 * mode means that a while loop is inserted in the ADC_TakeSample function and 
 * the code waits until the sample is finished. Non-blocking code means that 
 * the function returns after starting the sample process. More on that in a
 * moment.
 * 
 * To setup a channel to use with your ADC, first create an ADCChannel object
 * then call the ADC_InitChannel function. This will setup the channel for you
 * to use with the ADC. Most of the time this is as simple as setting the
 * channel number equal to the struct member. How you choose to encode this 
 * value is up to you. That is why we are using this interface. After the
 * initialization, any code that deals with the ADC channel can refer to it by
 * name. This will give you the ability to quickly and easily swap physical 
 * hardware to different channels, or use different processors without changing
 * the code in multiple places. 
 * 
 * This library should not deal with setting the GPIO to analog type. That
 * should be done before calling the the ADC channel init function. If we do
 * that here, it would create a dependency on some GPIO library.
 * 
 * Non-blocking mode is the default mode. If you choose, you can start a sample 
 * by calling ADC_TakeSample and poll the ADC_IsBusy function to know when the 
 * sample is ready. This is how a majority of processors operate. In order to
 * avoid doing this, you can setup the non-blocking mode timer by calling the
 * ADC_UseNonBlockingMode function. You give it the sample time in milliseconds
 * (how long to wait) and the expected update rate in milliseconds (how often
 * you plan to call the tick function). Call the tick function periodically,
 * and then when the sample is ready you will get a callback. (I realize most
 * samples are probably going to finish within a millisecond, but a callback is
 * really convienent and it avoids while loops. Plus, do you *really* need to 
 * have a new sample every 100 microseconds?)
 * 
 * To set up a callback, create a function which follows the format listed in 
 * ADC_SetSampleFinishedCallbackFunc. It must have a pointer to an ADCChannel
 * as its argument. This is so that you know which ADCChannel initiated the
 * callback. Then inside your function, you can use the pointer given to you 
 * and call the either the ADC_Get8Bit or ADC_Get16Bit functions.
 * 
 * One last note. I use a left-justified result for all of my ADC results. This 
 * ensures that 10-bit, 12-bit, 14-bit or whatever ADC values always go from 
 * 0 to 65535. Plus with a left-justified value, in order to get an 8-bit 
 * result, you simply drop the lower byte and return only the upper byte.
 * You can always shift your 16-bit result to the right, but if you port your 
 * code to another processor, you may have a different shift value, which is 
 * kind of like another dependency isn't it? I think it's better to stick with 
 * the full 16-bit range so there are no surprises.
 * 
 ******************************************************************************/

#ifndef IADC_H
#define	IADC_H

#include <stdint.h>
#include <stdbool.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

typedef struct ADCChannelTag
{
    uint8_t channelNumber;
    uint16_t adcValue;
} ADCChannel;

/** 
 * channelNumber    The ADC channel number. Can vary between MCU's
 * 
 * value            The ADC value for that channel
 */

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Function Prototypes *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Initialize necessary registers for this MCU
 * 
 */
void ADC_InitPeripheral(void);

/***************************************************************************//**
 * @brief Initialize a timer to allow for non-blocking use (default)
 * 
 * Non-blocking will be set by default. Using non-blocking mode will require 
 * you to call the ADC_Tick function periodically in order to upate the timer. 
 * The timer will check ongoing conversions and perform a callback if needed.
 * Otherwise, you can always just poll the ADC_IsBusy function.
 * 
 * @param sampleTimeMs  the amount of time to wait for a sample to finish
 * 
 * @param tickRateMs  how often you plan to call the tick function
 */
void ADC_UseNonBlockingMode(uint16_t sampleTimeMs, uint16_t tickRateMs);

/***************************************************************************//**
 * @brief Disable non-blocking mode
 * 
 * This will insert a while loop which will wait until the sample is done
 */
void ADC_UseBlockingMode(void);

/***************************************************************************//**
 * @brief Initialize an ADC channel object
 * 
 * Setting the GPIO to analog type should not be done here. It should be done
 * before calling this function.
 * 
 * @param self  pointer to the ADC channel object you are using
 * 
 * @param channelNumber  the channel number for this MCU
 */
void ADC_InitChannel(ADCChannel *self, uint8_t channelNumber);

/***************************************************************************//**
 * @brief Start an ADC conversion
 * 
 * Load the channel given by the ADCChannel object and start the conversion
 * 
 * @param self  pointer to the ADC channel object you are using
 */
void ADC_TakeSample(ADCChannel *self);

/***************************************************************************//**
 * @brief Check if the ADC is busy taking a sample
 * 
 * @return  true if busy
 */
bool ADC_IsBusy(void);

/***************************************************************************//**
 * @brief Get the channel object the ADC is currently processing
 * 
 * @return ADCChannel*  pointer to the current ADC channel
 */
ADCChannel* ADC_GetCurrentChannel(void);

/***************************************************************************//**
 * @brief Get the channel number the ADC is currently processing
 * 
 * This is the actual channel in the hardware. It should be given by the
 * channelNumber member.
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
uint16_t ADC_Get16Bit(ADCChannel *self);

/***************************************************************************//**
 * @brief Get the 8 bit value for a channel
 * 
 * @param self  pointer to the ADC channel object you are using
 * 
 * @return uint8_t  8-bit result
 */
uint8_t ADC_Get8Bit(ADCChannel *self);

/***************************************************************************//**
 * @brief Enable the ADC peripheral
 * 
 * After the peripheral is enabled, the ADC_PeripheralEnabledCallbackFunc is 
 * called.
 */
void ADC_Enable(void);

/***************************************************************************//**
 * @brief Disable the ADC peripheral
 * 
 * After the peripheral is disabled, the ADC_PeripheralDisabledCallbackFunc is 
 * called.
 */
void ADC_Disable(void);

/***************************************************************************//**
 * @brief Update the ADC conversion timers
 * 
 * You must call this function periodically. The update rate is set when you
 * call the UseNonBlockingMode function. The timer will check ongoing 
 * conversions and perform a callback if needed
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
 * The context is so that you can tell which ADC channel initiated the callback
 * 
 * @param CallbackFunc format: void SomeFunction(ADCChannel *context)
 */
void ADC_SetSampleFinishedCallbackFunc(void (*CallbackFunc)(ADCChannel *context));

/***************************************************************************//**
 * @brief A function pointer that is called after the ADC is enabled
 * 
 * Useful for turning on or resetting DMA for example
 * 
 * @param Function  format: void SomeFunction(void)
 */
void ADC_SetPeripheralEnabledCallbackFunc(void (*Function)(void));

/***************************************************************************//**
 * @brief A function pointer that is called after the ADC is disabled
 * 
 * Useful for turning off or resetting DMA for example
 * 
 * @param Function  format: void SomeFunction(void)
 */
void ADC_SetPeripheralDisabledCallbackFunc(void (*Function)(void));

#endif /* IADC_H */