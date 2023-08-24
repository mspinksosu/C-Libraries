/***************************************************************************//**
 * @brief Hardware Timer 2 Implementation (STM32F1)
 * 
 * @file HWTimer2_STM32F1.c
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 9/18/22  Original creation
 *
 * @details
 *      // TODO
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

#include "HWTimer2_STM32F1.h"

/* Include processor specific header files here */
#include "stm32f10x_map.h"

// ***** Defines ***************************************************************

#define HW_TIM_SIZE_BITS            16
#define HW_TIM_BITS_MAX             (1 << HW_TIM_SIZE_BITS)
#define HW_TIM_NUM_COMP_CHANNELS    4
#define TIMx                        TIM2

// ***** Global Variables ******************************************************

/*  The sub class must implement the functions provided in the interface. In 
    this case we are declaring an interface struct and initializing its members 
    (which are function pointers) the our local functions. Typecasting is 
    necessary. When a new sub class object is created, we will set its interface
    member equal to this table. */
HWTimerInterface HWTimer2_FunctionTable = {
    .HWTimer_GetPrescaleOptions = HWTimer2_STM32_GetPrescaleOptions,
    .HWTimer_ComputePeriod = (void (*)(void *, uint32_t, uint32_t, uint16_t *))HWTimer2_STM32_ComputePeriod,
    .HWTimer_Init = (void (*)(void *))HWTimer2_STM32_Init,
    .HWTimer_GetSize = HWTimer2_STM32_GetSize,
    .HWTimer_Start = HWTimer2_STM32_Start,
    .HWTimer_Stop = HWTimer2_STM32_Stop,
    .HWTimer_Reset = HWTimer2_STM32_Reset,
    .HWTimer_IsRunning = HWTimer2_STM32_IsRunning,
    .HWTimer_SetCount = HWTimer2_STM32_SetCount,
    .HWTimer_GetCount = HWTimer2_STM32_GetCount,
    .HWTimer_AddToCount = HWTimer2_STM32_AddToCount,
    .HWTimer_GetNumCompareChannels = HWTimer2_STM32_GetNumCompareChannels,
    .HWTimer_SetCompare16Bit = HWTimer2_STM32_SetCompare16Bit,
    .HWTimer_GetCompare16Bit = HWTimer2_STM32_GetCompare16Bit,
    .HWTimer_SetComparePercent = HWTimer2_STM32_SetComparePercent,
    .HWTimer_GetComparePercent = HWTimer2_STM32_GetComparePercent,
    .HWTimer_EnableComparePWM = HWTimer2_STM32_EnableComparePWM,
    .HWTimer_DisableComparePWM = HWTimer2_STM32_DisableComparePWM,
    .HWTimer_GetOverflow = HWTimer2_STM32_GetOverflow,
    .HWTimer_ClearOverflowFlag = HWTimer2_STM32_ClearOverflowFlag,
    .HWTimer_GetCompareMatch = HWTimer2_STM32_GetCompareMatch,
    .HWTimer_ClearCompareMatchFlag = HWTimer2_STM32_ClearCompareMatchFlag,
    .HWTimer_OverflowEvent = HWTimer2_STM32_OverflowEvent,
    .HWTimer_CompareMatchEvent = HWTimer2_STM32_CompareMatchEvent,
    .HWTimer_SetOverflowCallback = HWTimer2_STM32_SetOverflowCallback,
    .HWTimer_SetCompareMatchCallback = HWTimer2_STM32_SetCompareMatchCallback,
};

// static bool useOverflowInterrupt = false, useCompareMatchInterrupts = false; // TODO add interrupts
static uint16_t timerPeriod = HW_TIM_BITS_MAX - 1;

// local function pointers
static void (*OverflowCallback)(void);
static void (*CompareMatchCallback)(uint8_t compChan);

// ***** Static Function Prototypes ********************************************

/* Put static function prototypes here */
static uint32_t compChanToAddress(uint8_t chan);

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

HWTimerPrescaleOptions HWTimer2_STM32_GetPrescaleOptions(void)
{
    HWTimerPrescaleOptions retVal = {.options.usesCounter = 1,
                                     .counterNumBits = 16 };
    return retVal;
}

// *****************************************************************************

void HWTimer2_STM32_ComputePeriod(HWTimerInitType_STM32 *retParams, 
    uint32_t desiredFreqHz,  uint32_t clkInHz, uint16_t *retDiffInTicks)
{
    uint32_t period = 0, prescale = 1, fxpClkInHz, integerPart = 0;
    /* I want the period of the timer to be as large as possible to give the 
    most resolution. Equations: 
    prescale = clkInHz / HW_TIM_BITS_MAX / desiredFreqHz 
    period = clkInHz / prescale / desiredFreqHz */

    /* Shift the clock input left to convert to a 27.5 fixed point number. 
    A 27.5 fixed point number is just large enough to support the maximum 
    system clock. 2^27 = 134 MHz. Max clock = 72 MHz. */
    fxpClkInHz = clkInHz << 5;

    if(clkInHz / desiredFreqHz > (HW_TIM_BITS_MAX - 1))
    {
        prescale = fxpClkInHz / HW_TIM_BITS_MAX;
        /* Add 0.5 to round the prescale value value up. 0.5 in 27.5 fixed 
        point is 2^4 or 1 << 4. */
        prescale += (1 << 4);
        /* Shift to gain back some precision before dividing again. This will 
        make it 23.9 fxp. */
        prescale <<= 4;
        prescale = prescale / desiredFreqHz;
        /* Add 0.5 to round up again */
        prescale += (1 << 8);
        /* Now shift back to get the prescale integer */
        integerPart = prescale >> 9;
        /* Perform a ceiling function. If our prescale value in fixed point is 
        greater than the integer part, then there is a decimal digit. Round up 
        to the next integer. */
        if(prescale > (integerPart << 9))
            integerPart++;
        prescale = integerPart;
    }
    else
    {
        prescale = 1;
    }

    /* Now calculate the period needed (27.5 fxp) */
    period = fxpClkInHz / prescale;
    /* Add 0.5 to round the period value value up. 0.5 in 27.5 fixed point is 
    2^4 or 1 << 4. */
    period += (1 << 4);
    period = period / desiredFreqHz;
    /* Add 0.5 to round up again */
    period += (1 << 4);
    /* Now shift back to get the result */
    period >>= 5;

    /* @note I use this method instead if the prescale uses fixed powers of two 
    rather than a prescale counter. It can also work if there is a prescale 
    table. The idea is the same. It uses the smallest prescale value that gets 
    the period as close the max as possible. */
    // prescale = 0;
    // do {
    //     if(prescale == 0)
    //         prescale = 1;
    //     else
    //         prescale <<= 1;

    //     period = fxpClkInHz / prescale;
    //     period += (1 << 4);
    //     period = period / desiredFreqHz;
    //     period += (1 << 4);
    //     period >>= 5;
    // } while(period > HW_TIM_BITS_MAX - 1 && prescale < HW_TIM_BITS_MAX - 1);

    /* Subtract one to get the actual values to be loaded into the register */
    if(prescale > 0)
        prescale--;
    if(period > 0)
        period--;
    retParams->super->prescaleCounterValue = (uint16_t)prescale;
    retParams->super->period = (uint16_t)period;
    retParams->super->prescaleSelect = HWTIM_PRESCALE_USES_COUNTER;
    *retDiffInTicks = (HW_TIM_BITS_MAX - 1) - period;
}

// *****************************************************************************

void HWTimer2_STM32_Init(HWTimerInitType_STM32 *params)
{
    /* Reset counter. Count up continously, edge aligned mode. Disable. */
    TIMx->CR1 = 0;
    TIMx->ARR = 0xFFFF; // reset auto-reload value
    TIMx->CCER = 0; // disable all compare channels
    TIMx->CCR1 = 0;
    TIMx->CCR2 = 0;
    TIMx->CCR3 = 0;
    TIMx->CCR4 = 0;
    /* Set the global variable and the reload value */
    if(params->super->period < 0xFFFF)
        timerPeriod = params->super->period;
    else
        timerPeriod = 0xFFFF;
    TIMx->ARR = timerPeriod;
    TIMx->PSC = params->super->prescaleCounterValue;
    /* Don't start the timer yet */
}

// *****************************************************************************

HWTimerSize HWTimer2_STM32_GetSize(void)
{
    return HWTIM_16_BIT;
}

// *****************************************************************************

void HWTimer2_STM32_Start(void)
{
    TIMx->CR1 |= TIM_CR1_CEN;
}

// *****************************************************************************

void HWTimer2_STM32_Stop(void)
{
    TIMx->CR1 &= ~TIM_CR1_CEN;
}

// *****************************************************************************

void HWTimer2_STM32_Reset(void)
{
    TIMx->CNT = 0;
}

// *****************************************************************************

bool HWTimer2_STM32_IsRunning(void)
{
    if(TIMx->CR1 & TIM_CR1_CEN)
        return true;
    else
        return false;
}

// *****************************************************************************

void HWTimer2_STM32_SetCount(uint16_t count)
{
    TIMx->CNT = count;
}

// *****************************************************************************

uint16_t HWTimer2_STM32_GetCount(void)
{
    return TIMx->CNT;
}

// *****************************************************************************

void HWTimer2_STM32_AddToCount(uint16_t addToCount)
{
    TIMx->CNT += addToCount;
}

// *****************************************************************************

uint8_t HWTimer2_STM32_GetNumCompareChannels(void)
{
    return HW_TIM_NUM_COMP_CHANNELS;
}

// *****************************************************************************

void HWTimer2_STM32_SetCompare16Bit(uint8_t compChan, uint16_t compValue)
{
    if(compChan >= HW_TIM_NUM_COMP_CHANNELS)
        return;
    
    uint32_t *CCRx = (uint32_t*)compChanToAddress(compChan);
    uint32_t scaledCompValue = compValue * timerPeriod / (HW_TIM_16_BIT_MAX - 1);
    *CCRx = (uint16_t)scaledCompValue;
}

// *****************************************************************************

uint16_t HWTimer2_STM32_GetCompare16Bit(uint8_t compChan)
{
    if(compChan >= HW_TIM_NUM_COMP_CHANNELS)
        return 0;
    
    uint32_t *CCRx = (uint32_t*)compChanToAddress(compChan);
    uint32_t compValue = *CCRx;
    uint32_t retVal = compValue * (HW_TIM_16_BIT_MAX - 1) / timerPeriod;
    return retVal;
}

// *****************************************************************************

void HWTimer2_STM32_SetComparePercent(uint8_t compChan, uint8_t percent)
{
    if(compChan >= HW_TIM_NUM_COMP_CHANNELS)
        return;

    if(percent > 100)
        percent = 100;

    uint32_t *CCRx = (uint32_t*)compChanToAddress(compChan);
    /* Shift the timer period left to convert to a 24.8 fixed point number */
    uint32_t fxpTimerPeriod = timerPeriod << 8;
    uint32_t scaledCompValue = percent * fxpTimerPeriod / 100;
    /* Add 0.5 to round the fixed point result up */
    scaledCompValue += (1 << 7);
    /* Shift back right to get the final result */
    scaledCompValue >>= 8;
    *CCRx = (uint16_t)scaledCompValue;
}

// *****************************************************************************

uint8_t HWTimer2_STM32_GetComparePercent(uint8_t compChan)
{
    if(compChan >= HW_TIM_NUM_COMP_CHANNELS)
        return 0;

    uint32_t *CCRx = (uint32_t*)compChanToAddress(compChan);
    /* Shift the dividend left to conver to 24.8 fixed point number */
    uint32_t fxpCompValue = *CCRx << 8;
    uint32_t retVal = fxpCompValue * 100 / timerPeriod;
    /* Add 0.5 to round the fixed point result up */
    retVal += (1 << 7);
    /* Shift back right to get the final result */
    retVal >>= 8;
    return retVal;
}

// *****************************************************************************

void HWTimer2_STM32_EnableComparePWM(uint8_t compChan)
{
    if(compChan < HW_TIM_NUM_COMP_CHANNELS)
    {
        uint32_t mode = TIM_CCMR1_OC1M_2; // PWM mode 1 (positive polarity)
        mode |= TIM_CCMR1_OC1M_1;
        mode &= ~TIM_CCMR1_OC1M_0;
        /* Select either the upper or lower OCxM bits (0 or 1) */
        uint32_t ocmSelect = compChan & 0x01;

        switch(compChan)
        {
            case 0:
            case 1:
                TIMx->CCMR1 &= ~(TIM_CCMR1_OC1M << (ocmSelect * 8));
                TIMx->CCMR1 |= (mode << (ocmSelect * 8));
                TIMx->CCER |= (1 << (compChan * 4)); // enable
                break;
            case 2:
            case 3:
                TIMx->CCMR2 &= ~(TIM_CCMR1_OC1M << (ocmSelect * 8));
                TIMx->CCMR2 |= (mode << (ocmSelect * 8));
                TIMx->CCER |= (1 << (compChan * 4)); // enable
                break;
        }
    }
}

// *****************************************************************************

void HWTimer2_STM32_DisableComparePWM(uint8_t compChan)
{
    if(compChan < HW_TIM_NUM_COMP_CHANNELS)
    {
        /* Select either the upper or lower OCxM bits (0 or 1) */
        uint32_t ocmSelect = compChan & 0x01;

        switch(compChan)
        {
            case 0:
            case 1:
                TIMx->CCMR1 &= ~(TIM_CCMR1_OC1M << (ocmSelect * 8));
                TIMx->CCER &= ~(1 << (compChan * 4));
                break;
            case 2:
            case 3:
                TIMx->CCMR2 &= ~(TIM_CCMR1_OC1M << (ocmSelect * 8));
                TIMx->CCER &= ~(1 << (compChan * 4));
                break;
        }
    }
}

// *****************************************************************************

bool HWTimer2_STM32_GetOverflow(void)
{
    return (TIMx->SR & TIM_SR_UIF) ? 1 : 0;
}

// *****************************************************************************

void HWTimer2_STM32_ClearOverflowFlag(void)
{
    TIMx->SR &= ~TIM_SR_UIF;
}

// *****************************************************************************

bool HWTimer2_STM32_GetCompareMatch(uint8_t compChan)
{
    if(compChan < 4)
    {
        return (TIMx->SR & (1 << (compChan + 1))) ? 1 : 0; // bits [4:1]
    }
    else
    {
        return false;
    }
}

// *****************************************************************************

void HWTimer2_STM32_ClearCompareMatchFlag(uint8_t compChan)
{
    if(compChan < 4)
    {
        TIMx->SR &= ~(1 << (compChan + 1)); // bits [4:1]
    }
}

// *****************************************************************************

void HWTimer2_STM32_OverflowEvent(void)
{
    TIMx->SR &= ~TIM_SR_UIF;

    if(OverflowCallback)
        OverflowCallback();
}

// *****************************************************************************

void HWTimer2_STM32_CompareMatchEvent(void)
{
    if(TIMx->SR & TIM_SR_CC1IF)
    {
        TIMx->SR &= ~TIM_SR_CC1IF;
        if(CompareMatchCallback)
            CompareMatchCallback(0);
    }
    if(TIMx->SR & TIM_SR_CC2IF)
    {
        TIMx->SR &= ~TIM_SR_CC2IF;
        if(CompareMatchCallback)
            CompareMatchCallback(1);
    }
    if(TIMx->SR & TIM_SR_CC3IF)
    {
        TIMx->SR &= ~TIM_SR_CC3IF;
        if(CompareMatchCallback)
            CompareMatchCallback(2);
    }
    if(TIMx->SR & TIM_SR_CC4IF)
    {
        TIMx->SR &= ~TIM_SR_CC4IF;
        if(CompareMatchCallback)
            CompareMatchCallback(3);
    }
}

// *****************************************************************************

void HWTimer2_STM32_SetOverflowCallback(void (*Function)(void))

{
    OverflowCallback = Function;
}

// *****************************************************************************

void HWTimer2_STM32_SetCompareMatchCallback(void (*Function)(uint8_t compChan))

{
    CompareMatchCallback = Function;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Local Functions *****************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

static uint32_t compChanToAddress(uint8_t channel)
{
    uint32_t address = ((uint32_t)&(TIMx->CCR1)); // address offset 0x34

    // channel 0 - 3 equals CCR1 to CCR4
    if(channel < 4)
    {
        // address offset 0x34 through 0x40
        address += channel * 4;
    }
    return address;
}

/*
 End of File
 */