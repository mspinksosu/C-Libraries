/***************************************************************************//**
 * @brief Hardware Timer Interface Header File
 * 
 * @file IHardwareTimer.h
 * 
 * @author Matthew Spinks
 * 
 * @date 9/17/22  Original creation
 * 
 * @details
 *      An interface to make use of a hardware timer peripheral features.
 * // TODO details
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

#ifndef IHW_TIMER_H
#define IHW_TIMER_H

#include <stdint.h>
#include <stdbool.h>

// ***** Defines ***************************************************************

#define HW_TIM_8_BIT_MAX    256
#define HW_TIM_16_BIT_MAX   65536

// ***** Global Variables ******************************************************

typedef enum HWTimerTypeTag // TODO might move this to a timer manager or something
{
    HWTIM_FREE_RUNNING = 0,
    HWTIM_MONO_RESETTABLE,
    HWTIM_MONO_NONRESETTABLE
} HWTimerType;

typedef enum HWTimerSizeTag // TODO probably not needed
{
    HWTIM_8_BIT = 0,
    HWTIM_16_BIT,
    //HWTIM_32_BIT
} HWTimerSize;

typedef enum HWTimerPrescaleSelectTag
{
    HWTIM_PRESCALE_USES_COUNTER = 0,
    HWTIM_PRESCALE_2,
    HWTIM_PRESCALE_4,
    HWTIM_PRESCALE_8,
    HWTIM_PRESCALE_16,
    HWTIM_PRESCALE_32,
    HWTIM_PRESCALE_64,
    HWTIM_PRESCALE_128,
    HWTIM_PRESCALE_256,
    HWTIM_PRESCALE_512,
    HWTIM_PRESCALE_1024,
} HWTimerPrescaleSelect;

typedef struct HWTimerPrescaleOptionsTag
{
    struct {
        unsigned usesCounter    :1;
        unsigned prescale2      :1;
        unsigned prescale4      :1;
        unsigned prescale8      :1;
        unsigned prescale16     :1;
        unsigned prescale32     :1;
        unsigned prescale64     :1;
        unsigned prescale128    :1;
        unsigned prescale256    :1;
        unsigned prescale512    :1;
        unsigned prescale1024   :1;
        unsigned                :5;
    } options;
    uint8_t counterNumBits;
} HWTimerPrescaleOptions;

typedef struct HWTimerInitTypeTag
{
    void *instance;
    //HWTimerOverflowCallbackFunc overflowCallback;
    //HWTimerCompareMatchCallbackFunc compareMatchCallback;
    uint32_t periodInUs; // TODO not needed unless we want to set values in us
    //HWTimerType type;
    uint16_t period;
    uint16_t count; // TODO probably not needed
    uint16_t prescaleCounterValue;
    HWTimerPrescaleSelect prescaleSelect;
    bool useOverflowInterrupt;
    bool useCompareMatchInterrupts;
} HWTimerInitType;

typedef struct HWTimerInterfaceTag
{
    /* These are the functions that will be called. You will create your own
    interface object for your class that will have these function signatures.
    Set each of your functions equal to one of these pointers. The void pointer
    will be set to the sub class object. Typecasting will be needed. */
    HWTimerPrescaleOptions (*HWTimer_GetPrescaleOptions)(void);
    void (*HWTimer_ComputePeriod)(void *params, uint32_t, uint32_t, uint16_t *retValue);
    void (*HWTimer_Init)(void *params);
    HWTimerSize (*HWTimer_GetSize)(void);
    void (*HWTimer_Start)(void);
    void (*HWTimer_Stop)(void);
    void (*HWTimer_Reset)(void);
    bool (*HWTimer_IsRunning)(void);
    void (*HWTimer_SetCount)(uint16_t count);
    uint16_t (*HWTimer_GetCount)(void);
    void (*HWTimer_AddToCount)(uint16_t addToCount);
    uint8_t (*HWTimer_GetNumCompareChannels)(void);
    void (*HWTimer_SetCompare16Bit)(uint8_t compChan, uint16_t compValue);
    uint16_t (*HWTimer_GetCompare16Bit)(uint8_t compChan);
    void (*HWTimer_SetComparePercent)(uint8_t compChan, uint8_t percent);
    uint8_t (*HWTimer_GetComparePercent)(uint8_t compChan);
    void (*HWTimer_EnableComparePWM)(uint8_t compChan, bool useInterrupt);
    void (*HWTimer_DisableComparePWM)(uint8_t compChan);
    bool (*HWTimer_GetOverflow)(void);
    void (*HWTimer_ClearOverflowFlag)(void);
    bool (*HWTimer_GetCompareMatch)(uint8_t compChan);
    void (*HWTimer_ClearCompareMatchFlag)(uint8_t compChan);
    void (*HWTimer_OverflowEvent)(void);
    void (*HWTimer_CompareMatchEvent)(void);
    void (*HWTimer_SetOverflowCallback)(void (*Function)(void));
    void (*HWTimer_SetCompareMatchCallback)(void (*Function)(uint8_t compChan));
} HWTimerInterface;

typedef struct HWTimerTag
{
    HWTimerInterface *interface;
} HWTimer;

/**
 * Description of struct members. You shouldn't really mess with any of these
 * variables directly. That is why I made functions for you to use.
 * 
 * // TODO description
 * 
 */

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Set the function table for your HWTimer object
 * 
 * @param self  pointer to the HWTimer you are using
 * 
 * @param interface  pointer to the function table that your HWTimer uses
 */
void HWTimer_Create(HWTimer *self, HWTimerInterface *interface);

/***************************************************************************//**
 * @brief Combine the base class and sub class HWTimerInitType
 * 
 * My preferred method is to call this function from a sub class constructor. 
 * This makes the create function more type safe. To create a sub class 
 * constructor, make your own Create function that takes your sub class and 
 * base class as arguments. Set your variables then call this function from it.
 * 
 * @param params  pointer to the HWTimerInitType you are using
 * 
 * @param instanceOfSubclass  the child object that implements HWTimerInitType
 */
void HWTimer_CreateInitType(HWTimerInitType *params, void *instanceOfSubclass);

/***************************************************************************//**
 * @brief Set the default values of the HWTimerInitType object
 * 
 * Prescale settings to 0, timer period = maximum (0xFFFF), no interrupts
 * 
 * @param params  pointer to the HWTimerInitType you are using
 */
void HWTimer_SetInitTypeToDefaultParams(HWTimerInitType *params);

/***************************************************************************//**
 * @brief Set the initial values for the HWTimerInitType
 * 
 * Alternatively, you can set the values of the HWTimerInitType members 
 * directly.
 * 
 * @param params  pointer to the HWTimerInitType you are using
 * @param prescaleSelect  the preselect options you want to use
 * @param prescaleCounter  value used if counter is selected as prescale option
 * @param period  the period for the timer
 * @param useOverflowInterrupt  if true, enable overflow interrupt if available
 * @param useCompareMatchInterrupts  if true, enable CM interrupts if available
 */
void HWTimer_SetInitTypeParams(HWTimerInitType *params, HWTimerPrescaleSelect prescaleSelect,
    uint16_t prescaleCounter, uint16_t period, bool useOverflowInterrupt, bool useCompareMatchInterrupts);

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Get the prescale options for the Hardware Timer
 * 
 * A bit will be set for each prescale option that is availble for the timer.
 * If the timer uses a counter for a prescaler instead, bit zero should be set 
 * as well as the number of bits for the counter.
 * 
 * @param self  pointer to the HWTimer you are using
 * 
 * @return HWTimerPrescaleOptions  either fixed prescale values or a counter
 */
HWTimerPrescaleOptions HWTimer_GetPrescaleOptions(HWTimer *self);

/***************************************************************************//**
 * @brief Select the settings needed for the desired period (in us)
 * 
 * The function should go through the prescale settings starting from the 
 * lowest, and find the setting that generates a frequency greater than or 
 * equal to the desired frequency. Then return the settings used and the 
 * difference in ticks subtracted from the maximum value of the timer. The idea
 * is that the user can use the settings returned to get close enough to the 
 * value they want. Most timers will automatically restart once they hit the 
 * period value that is loaded. If for some reason the timer doesn't have that 
 * feature, the user can take the difference in ticks and load that value into 
 * the counter every time the counter overflows. 
 * 
 * @param self  pointer to the HWTimer you are using
 * @param retParams  pointer to the HWTimerInitType that you are using
 * @param desiredFreqHz  the frequency that you want in Hertz
 * @param clkInHz  the frequency of your timer peripheral's clock in Hertz
 * @param retDiffInTicks  difference in ticks subtracted from the max count
 */
void HWTimer_ComputePeriod(HWTimer *self, HWTimerInitType *retParams, 
    uint32_t desiredFreqHz, uint32_t clkInHz, uint16_t *retDiffInTicks);

/***************************************************************************//**
 * @brief Initialize the Hardware Timer
 * 
 * Use the parameters provided to set the necessary registers for your MCU.
 * The timer should count from 0 up to its maximum value, then rollover.
 * 
 * @param self  pointer to the HWTimerInitType you are using
 * 
 * @param params  pointer to the HWTimerInitType you are using
 */
void HWTimer_Init(HWTimer *self, HWTimerInitType *params);

/***************************************************************************//**
 * @brief Get the size of the Hardware Timer
 * 
 * @param self  pointer to the HWTimer you are using
 * 
 * @return HWTimerSize  HWTIM_8_BIT or HWTIM_16_BIT
 */
HWTimerSize HWTimer_GetSize(HWTimer *self);

/***************************************************************************//**
 * @brief Start the Hardware Timer
 * 
 * @param self  pointer to the HWTimer you are using
 */
void HWTimer_Start(HWTimer *self);

/***************************************************************************//**
 * @brief Stop the Hardware Timer
 * 
 * Does not reset the counter.
 * 
 * @param self  pointer to the HWTimer you are using
 */
void HWTimer_Stop(HWTimer *self);

/***************************************************************************//**
 * @brief Reset the counter to zero
 * 
 * Does not stop the timer.
 * 
 * @param self  pointer to the HWTimer you are using
 */
void HWTimer_Reset(HWTimer *self);

/***************************************************************************//**
 * @brief Check if the Hardware Timer is running
 * 
 * @param self  pointer to the HWTimer you are using
 * 
 * @return true if the timer is running
 */
bool HWTimer_IsRunning(HWTimer *self);

/***************************************************************************//**
 * @brief Set the counter of the Hardware Timer directly
 * 
 * @param self  pointer to the HWTimer you are using
 * 
 * @param count  the value to set the counter register to
 */
void HWTimer_SetCount(HWTimer *self, uint16_t count);

/***************************************************************************//**
 * @brief Get the current count of the Hardware Timer
 * 
 * @param self  pointer to the HWTimer you are using
 * 
 * @return uint16_t  the current count of the timer
 */
uint16_t HWTimer_GetCount(HWTimer *self);

/***************************************************************************//**
 * @brief Add a value to the current count directly
 * 
 * If you want to generate an overflow event every so many ticks, adding your 
 * value to the current value of the counter is a little bit more accurate 
 * than just loading your value at every overflow event. This function is 
 * provided to make it slightly faster, but don't expect it to be extremely
 * accurate.
 * 
 * @param self  pointer to the HWTimer you are using
 * 
 * @param addToCount  the value to add to the count
 */
void HWTimer_AddToCount(HWTimer *self, uint16_t addToCount);

/***************************************************************************//**
 * @brief Get the number of compare channels available
 * 
 * The compare match channels should be numbered in ascending order starting
 * with zero. Sometimes microcontrollers will use letters 'A' 'B' etc.
 * 
 * @param self  pointer to the HWTimer you are using
 * 
 * @return uint8_t  how many compare channels there are available
 */
uint8_t HWTimer_GetNumCompareChannels(HWTimer *self);

/***************************************************************************//**
 * @brief Set the compare value for a given channel (0-65535 full scale)
 * 
 * This does not set the value directly. Rather, it uses a 16-bit full scale
 * value and then computes the actual value based on the period of the timer.
 * When the timer count hits the value set by the compare channel, you will get 
 * a compare match event. When using the compare output to generate a PWM 
 * waveform, you should not be altering the period of the timer, or else your 
 * compare match event may not get triggered.
 * 
 * @param self  pointer to the HWTimer you are using
 * 
 * @param compChan  the number of the compare channel (beginning with 0)
 * 
 * @param compValue  the value of the compare channel (0-65535)
 */
void HWTimer_SetCompare16Bit(HWTimer *self, uint8_t compChan, uint16_t compValue);

/***************************************************************************//**
 * @brief Get the compare value for a given channel (0-65535 full scale)
 * 
 * The value being returned will be scaled to a 16-bit number, regardless of 
 * the size of the timer.
 * 
 * @param self  pointer to the HWTimer you are using
 * 
 * @param compChan  the number of the compare channel (beginning with 0)
 * 
 * @return uint16_t  the value of the compare channel (0-65535)
 */
uint16_t HWTimer_GetCompare16Bit(HWTimer *self, uint8_t compChan);

/***************************************************************************//**
 * @brief Set the compare value for a given channel (0-100 full scale)
 * 
 * Sets the compare value to a percentage. This is probably most useful for 
 * generating PWM waveforms. When the timer count hits the value set by the 
 * compare channel, you will get a compare match event. When using the compare 
 * output to generate a PWM waveform, you should not be altering the period of 
 * the timer, or else your compare match event may not get triggered. If you 
 * need more resolution, use the SetCompare16Bit function instead.
 * 
 * @param self  pointer to the HWTimer you are using
 * 
 * @param compChan  the number of the compare channel (beginning with 0)
 * 
 * @param percent  the value of the compare channel (0-100)
 */
void HWTimer_SetComparePercent(HWTimer *self, uint8_t compChan, uint8_t percent);

/***************************************************************************//**
 * @brief Get the compare value for a given channel (0-100 full scale)
 * 
 * @param self  pointer to the HWTimer you are using
 * 
 * @param compChan  the number of the compare channel (beginning with 0)
 * 
 * @return uint8_t  the value of the compare channel (0-100)
 */
uint8_t HWTimer_GetComparePercent(HWTimer *self, uint8_t compChan);

/***************************************************************************//**
 * @brief Enable a compare match channel
 * 
 * The compare channel should be in PWM mode. The output should be low when the 
 * count is below the compare value and high when it is above it. Enabling PWM 
 * mode does not alter the pin settings.
 * 
 * The compare match channels should be numbered in ascending order starting
 * with zero. Sometimes microcontrollers will use letters 'A' 'B' etc.
 * If you enable the compare match interrupt, you must utlize the 
 * CompareMatchEvent function.
 * 
 * @param self  pointer to the HWTimer you are using
 * 
 * @param compChan  the number of the compare channel (beginning with 0)
 */
void HWTimer_EnableComparePWM(HWTimer *self, uint8_t compChan); // Add mode function in future

// TODO add use interrupt function

/***************************************************************************//**
 * @brief Disable a compare match channel
 * 
 * @param self  pointer to the HWTimer you are using
 * 
 * @param compChan  the number of the compare channel (beginning with 0)
 */
void HWTimer_DisableComparePWM(HWTimer *self, uint8_t compChan);

/***************************************************************************//**
 * @brief Check if the overflow flag is set
 * 
 * The flag should not cleared when calling this function.
 * 
 * @param self  pointer to the HWTimer you are using
 * 
 * @return true if the timer did overflow
 */
bool HWTimer_GetOverflow(HWTimer *self);

/***************************************************************************//**
 * @brief Clear the overflow flag
 * 
 * If you are using an interrupt, this flag will also be cleared when the 
 * interrupt is called.
 * 
 * @param self  pointer to the HWTimer you are using
 */
void HWTimer_ClearOverflowFlag(HWTimer *self);

/***************************************************************************//**
 * @brief Check if the hardware compare match flag is set
 * 
 * The flag should not cleared when calling this function.
 * 
 * @param self  pointer to the HWTimer you are using
 * 
 * @param compChan  the number of the compare channel (beginning with 0)
 * 
 * @return true if the compare match flag is set
 */
bool HWTimer_GetCompareMatch(HWTimer *self, uint8_t compChan);

/***************************************************************************//**
 * @brief Clear the compare match flag
 * 
 * If you are using an interrupt, this flag will also be cleared when the 
 * interrupt is called.
 * 
 * @param self  pointer to the HWTimer you are using
 * 
 * @param compChan  the number of the compare channel (beginning with 0)
 */
void HWTimer_ClearCompareMatchFlag(HWTimer *self, uint8_t compChan);

/***************************************************************************//**
 * @brief The timer has overflowed. Clear the flag, then call the callback
 * 
 * This event is called whenever the timer overflows and the overflow interrupt
 * is enabled. Place this function wherever your interrupt is at. It should 
 * clear the interrupt flag and call the OverflowCallback function.
 * 
 * @param self  pointer to the HWTimer you are using
 */
void HWTimer_OverflowEvent(HWTimer *self);

/***************************************************************************//**
 * @brief An event has happened. Check flags, clear flags, call any callbacks
 * 
 * This event is called anytime there is a compare match event. Place this 
 * function call in each of your compare match interrupts for this timer. 
 * This function will go through the list of events and check if each one is 
 * enabled and has a flag set. If so, clear the flag, and call the appropriate 
 * callback function. The compare match channels should be numbered in 
 * ascending order starting with zero. This function will call the 
 * CompareMatchCallback function.
 * 
 * @param self  pointer to the HWTimer you are using
 */
void HWTimer_CompareMatchEvent(HWTimer *self);

/***************************************************************************//**
 * @brief Set a function to be called whenever the timer overflows
 * 
 * This function is called from within the OverflowEvent function. Your 
 * function should follow the format listed below.
 * 
 * @param self  pointer to the HWTimer you are using
 * 
 * @param Function  format: void SomeFunction(void)
 */
void HWTimer_SetOverflowCallback(HWTimer *self, void (*Function)(void));

/***************************************************************************//**
 * @brief Set a function to be called whenever a compare match happens
 * 
 * This function is called from within the CompareMatchEvent function. Your 
 * function should follow the format listed below.
 * 
 * @param self  pointer to the HWTimer you are using
 * 
 * @param Function  format: void Function(uint8_t compChan)
 */
void HWTimer_SetCompareMatchCallback(HWTimer *self, void (*Function)(uint8_t compChan));

// TODO This would require us to store our period in us. But it would be very convenient
void HWTimer_SetCompareInUs(HWTimer *self, uint8_t compChan, uint32_t desiredTimeInUs);

// TODO
uint32_t HWTimer_GetPeriodInUs(HWTimer *self);

#endif  /* IHW_TIMER_H */