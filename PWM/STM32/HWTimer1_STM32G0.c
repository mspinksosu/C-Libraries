/***************************************************************************//**
 * @brief Foo Library Implementation (MCU1)
 * 
 * @author Matthew Spinks
 * 
 * @date 12/2/14  Original creation
 * @date 2/4/22   Modified
 * 
 * @file Foo_MCU1.c
 * 
 * @details
 *      // TODO
 * 
 ******************************************************************************/

#include "HWTimer1_STM32G0.h"

/* Include processor specific header files here */
#include "stm32g071xx.h"

// ***** Defines ***************************************************************

#define HW_TIM1_NUM_COMP_CHANNELS   6
#define TIMx    TIM1

// ***** Global Variables ******************************************************

/*  The sub class must implement the functions provided in the interface. In 
    this case we are declaring an interface struct and initializing its members 
    (which are function pointers) the our local functions. Typecasting is 
    necessary. When a new sub class object is created, we will set its interface
    member equal to this table. */
HWTimerInterface HWTimer1_FunctionTable = {
    .HWTimer_GetPrescaleOptions = HWTimer1_STM32_GetPrescaleOptions,
    .HWTimer_ComputePeriodUs = (void (*)(void *, uint32_t, uint32_t, uint16_t *))HWTimer1_STM32_ComputePeriodUs,
    .HWTimer_Init = (void (*)(void *))HWTimer1_STM32_Init,
    .HWTimer_GetSize = HWTimer1_STM32_GetSize,
    .HWTimer_Start = HWTimer1_STM32_Start,
    .HWTimer_Stop = HWTimer1_STM32_Stop,
    .HWTimer_Reset = HWTimer1_STM32_Reset,
    .HWTimer_IsRunning = HWTimer1_STM32_IsRunning,
    .HWTimer_SetCount = HWTimer1_STM32_SetCount,
    .HWTimer_GetCount = HWTimer1_STM32_GetCount,
    .HWTimer_AddToCount = HWTimer1_STM32_AddToCount,
    .HWTimer_GetNumCompareChannels = HWTimer1_STM32_GetNumCompareChannels,
    .HWTimer_SetCompare16Bit = HWTimer1_STM32_SetCompare16Bit,
    .HWTimer_GetCompare16Bit = HWTimer1_STM32_GetCompare16Bit,
    .HWTimer_SetComparePercent = HWTimer1_STM32_SetComparePercent,
    .HWTimer_GetComparePercent = HWTimer1_STM32_GetComparePercent,
    .HWTimer_EnableCompare = HWTimer1_STM32_EnableCompare,
    .HWTimer_DisableCompare = HWTimer1_STM32_DisableCompare,
    .HWTimer_GetOverflow = HWTimer1_STM32_GetOverflow,
    .HWTimer_GetCompareMatch = HWTimer1_STM32_GetCompareMatch,
    .HWTimer_ClearOverflowFlag = HWTimer1_STM32_ClearOverflowFlag,
    .HWTimer_ClearCompareMatchFlag = HWTimer1_STM32_ClearCompareMatchFlag,
    .HWTimer_OverflowEvent = HWTimer1_STM32_OverflowEvent,
    .HWTimer_CompareMatchEvent = HWTimer1_STM32_CompareMatchEvent,
    .HWTimer_SetOverflowCallback = HWTimer1_STM32_SetOverflowCallback,
    .HWTimer_SetCompareMatchCallback = HWTimer1_STM32_SetCompareMatchCallback,
};

static bool useOverflowInterrupt = false, useCompareMatchInterrupts = false;

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

HWTimerPrescaleOptions HWTimer1_STM32_GetPrescaleOptions(void)
{
    HWTimerPrescaleOptions retVal = {.options.usesCounter = 1,
                                     .counterNumBits = 16 };
    return retVal;
}

// *****************************************************************************

void HWTimer1_STM32_ComputePeriodUs(HWTimerInitType_STM32 *params, uint32_t desiredPeriodUs, 
    uint32_t clkInHz, uint16_t *retDiffInTicks)
{
    uint32_t prescaleCounter = clkInHz / desiredPeriodUs;
    if(prescaleCounter > 0x0000FFFF)
        prescaleCounter = 0x0000FFFF;
    params->super->prescaleCounterValue = (uint16_t)prescaleCounter;
    params->super->prescaleSelect = HWTIM_PRESCALE_USES_COUNTER;
}

// *****************************************************************************

void HWTimer1_STM32_Init(HWTimerInitType_STM32 *params)
{
    /* Reset counter. Count up continously, edge aligned mode. Disable. */
    TIMx->CR1 = 0;
    TIMx->ARR = 0xFFFF; // reset auto-reload value
    TIMx->CCER = 0; // disable all compare channels
    TIMx->CCR1 = 0;
    TIMx->CCR2 = 0;
    TIMx->CCR3 = 0;
    TIMx->CCR4 = 0;
    TIMx->CCR5 = 0;
    TIMx->CCR6 = 0;
    TIMx->PSC = params->super->prescaleCounterValue;
}

// *****************************************************************************

HWTimerSize HWTimer1_STM32_GetSize(void)
{
    return HWTIM_16_BIT;
}

// *****************************************************************************

void HWTimer1_STM32_Start(void)
{
    TIMx->CR1 |= TIM_CR1_CEN;
}

// *****************************************************************************

void HWTimer1_STM32_Stop(void)
{
    TIMx->CR1 &= ~TIM_CR1_CEN;
}

// *****************************************************************************

void HWTimer1_STM32_Reset(void)
{
    TIMx->CNT = 0;
}

// *****************************************************************************

bool HWTimer1_STM32_IsRunning(void)
{
    if(TIMx->CR1 & TIM_CR1_CEN)
        return true;
    else
        return false;
}

// *****************************************************************************

void HWTimer1_STM32_SetCount(uint16_t count)
{
    TIMx->CNT = count;
}

// *****************************************************************************

uint16_t HWTimer1_STM32_GetCount(void)
{
    return TIMx->CNT;
}

// *****************************************************************************

void HWTimer1_STM32_AddToCount(uint16_t addToCount)
{
    TIMx->CNT += addToCount;
}

// *****************************************************************************

uint8_t HWTimer1_STM32_GetNumCompareChannels(void)
{
    return HW_TIM1_NUM_COMP_CHANNELS;
}

// *****************************************************************************

void HWTimer1_STM32_SetCompare16Bit(uint8_t compChan, uint16_t compValue)
{
    if(compChan >= HW_TIM1_NUM_COMP_CHANNELS)
        return;
    
    uint32_t *CCRx = compChanToAddress(compChan);
    *CCRx = compValue;
}

// *****************************************************************************

uint16_t HWTimer1_STM32_GetCompare16Bit(uint8_t compChan)
{
    if(compChan >= HW_TIM1_NUM_COMP_CHANNELS)
        return;
    
    uint32_t *CCRx = compChanToAddress(compChan);
    return *CCRx;
}

// *****************************************************************************

void HWTimer1_STM32_SetComparePercent(uint8_t compChan, uint8_t percent)
{
    if(compChan >= HW_TIM1_NUM_COMP_CHANNELS)
        return;

    uint32_t *CCRx = compChanToAddress(compChan);
    uint32_t compValue = percent * 65535 / 100;
    *CCRx = compValue;
}

// *****************************************************************************

uint8_t HWTimer1_STM32_GetComparePercent(uint8_t compChan)
{
    if(compChan >= HW_TIM1_NUM_COMP_CHANNELS)
        return 0;

    uint32_t *CCRx = compChanToAddress(compChan);
    return *CCRx * 100 / 65535;
}

// *****************************************************************************

void HWTimer1_STM32_EnableCompare(uint8_t compChan, bool useInterrupt)
{
    if(compChan >= HW_TIM1_NUM_COMP_CHANNELS)
        return;
    
    TIMx->CCER |= (1 << (compChan * 4));

    /* Only CC channels 0 - 3 (1 to 4) have interrupts */
    if(compChan < 4)
    {
        if(useInterrupt)
            TIMx->DIER |= (1 << (compChan + 1)); // [5:1]
        else
            TIMx->DIER &= ~(1 << (compChan + 1)); // [5:1]
    }
}

// *****************************************************************************

void HWTimer1_STM32_DisableCompare(uint8_t compChan)
{
    if(compChan >= HW_TIM1_NUM_COMP_CHANNELS)
        return;
    
    TIMx->CCER &= ~(1 << (compChan * 4));
}

// *****************************************************************************

bool HWTimer1_STM32_GetOverflow(void)
{
    return (TIMx->SR & TIM_SR_UIF) ? 1 : 0;
}

// *****************************************************************************

bool HWTimer1_STM32_GetCompareMatch(uint8_t compChan)
{
    uint8_t bitPos = 0;
    if(compChan < 4)
    {
        return (TIMx->SR & (1 << (compChan + 1))) ? 1 : 0;
    }
    else if(compChan < 6)
    {
        return (TIMx->SR & (1 << (compChan + 12))) ? 1 : 0;
    }
    else
    {
        return false;
    }
}

// *****************************************************************************

void HWTimer1_STM32_ClearOverflowFlag(void)
{
    TIMx->SR &= ~TIM_SR_UIF;
}

// *****************************************************************************

void HWTimer1_STM32_ClearCompareMatchFlag(uint8_t compChan)
{
    if(compChan < 4)
    {
        TIMx->SR &= ~(1 << (compChan + 1));
    }
    else if(compChan < 6)
    {
        TIMx->SR &= ~(1 << (compChan + 12));
    }
}

// *****************************************************************************

void HWTimer1_STM32_OverflowEvent(void)
{
    TIMx->SR &= ~TIM_SR_UIF;

    if(OverflowCallback)
        OverflowCallback();
}

// *****************************************************************************

void HWTimer1_STM32_CompareMatchEvent(void)
{
    if(TIMx->SR & TIM_SR_CC1IF)
    {
        TIMx->SR &= TIM_SR_CC1IF;
        if(CompareMatchCallback)
            CompareMatchCallback(0);
    }
    if(TIMx->SR & TIM_SR_CC2IF)
    {
        TIMx->SR &= TIM_SR_CC2IF;
        if(CompareMatchCallback)
            CompareMatchCallback(1);
    }
    if(TIMx->SR & TIM_SR_CC3IF)
    {
        TIMx->SR &= TIM_SR_CC3IF;
        if(CompareMatchCallback)
            CompareMatchCallback(2);
    }
    if(TIMx->SR & TIM_SR_CC4IF)
    {
        TIMx->SR &= TIM_SR_CC4IF;
        if(CompareMatchCallback)
            CompareMatchCallback(3);
    }
    if(TIMx->SR & TIM_SR_CC5IF)
    {
        TIMx->SR &= TIM_SR_CC5IF;
        if(CompareMatchCallback)
            CompareMatchCallback(4);
    }
    if(TIMx->SR & TIM_SR_CC6IF)
    {
        TIMx->SR &= TIM_SR_CC6IF;
        if(CompareMatchCallback)
            CompareMatchCallback(5);
    }
}

// *****************************************************************************

void HWTimer1_STM32_SetOverflowCallback(void (*Function)(void))

{
    OverflowCallback = Function;
}

// *****************************************************************************

void HWTimer1_STM32_SetCompareMatchCallback(void (*Function)(uint8_t compChan))

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
    uint32_t address = &(TIMx->CCR1); // address offset 0x34

    // channel 0 - 3 equals CCR1 to CCR4
    if(channel < 4)
    {
        // address offset 0x34 through 0x40
        address += channel * 4;
    }
    else if(channel < 6)
    {
        // address offset 0x58 and 0x5C
        address = address + 20 + channel * 4;
    }
}

/*
 End of File
 */