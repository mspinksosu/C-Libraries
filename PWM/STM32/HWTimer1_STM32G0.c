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
 *      TODO
 * 
 ******************************************************************************/

#include "HWTimer1_STM32G0.h"

// ***** Defines ***************************************************************


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
static HWTimerOverflowCallbackFunc OverflowCallback;
static HWTimerCompareMatchCallbackFunc CompareMatchCallback;

// ***** Static Function Prototypes ********************************************

/* Put static function prototypes here */


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

}

// *****************************************************************************

void HWTimer1_STM32_ComputePeriodUs(HWTimerInitType_STM32 *params, uint32_t desiredPeriodUs, 
    uint32_t clkInHz, uint16_t *retDiffInTicks)
{

}

// *****************************************************************************

void HWTimer1_STM32_Init(HWTimerInitType_STM32 *params)
{

}

// *****************************************************************************

HWTimerSize HWTimer1_STM32_GetSize(void)
{

}

// *****************************************************************************

void HWTimer1_STM32_Start(void)
{

}

// *****************************************************************************

void HWTimer1_STM32_Stop(void)
{

}

// *****************************************************************************

void HWTimer1_STM32_Reset(void)
{

}

// *****************************************************************************

bool HWTimer1_STM32_IsRunning(void)
{

}

// *****************************************************************************

void HWTimer1_STM32_SetCount(uint16_t count)
{

}

// *****************************************************************************

uint16_t HWTimer1_STM32_GetCount(void)
{

}

// *****************************************************************************

void HWTimer1_STM32_AddToCount(uint16_t addToCount)
{

}

// *****************************************************************************

uint8_t HWTimer1_STM32_GetNumCompareChannels(void)
{

}

// *****************************************************************************

void HWTimer1_STM32_SetCompare16Bit(uint8_t compChan, uint16_t compValue)
{

}

// *****************************************************************************

uint16_t HWTimer1_STM32_GetCompare16Bit(uint8_t compChan)
{

}

// *****************************************************************************

void HWTimer1_STM32_SetComparePercent(uint8_t compChan, uint8_t percent)
{

}

// *****************************************************************************

uint16_t HWTimer1_STM32_GetComparePercent(uint8_t compChan)
{

}

// *****************************************************************************

void HWTimer1_STM32_EnableCompare(uint8_t compChan)
{

}

// *****************************************************************************

void HWTimer1_STM32_DisableCompare(uint8_t compChan)
{

}

// *****************************************************************************

bool HWTimer1_STM32_GetOverflow(void)
{

}

// *****************************************************************************

bool HWTimer1_STM32_GetCompareMatch(uint8_t compChan)
{

}

// *****************************************************************************

void HWTimer1_STM32_ClearOverflowFlag(void)
{

}

// *****************************************************************************

void HWTimer1_STM32_ClearCompareMatchFlag(uint8_t compChan)
{

}

// *****************************************************************************

void HWTimer1_STM32_OverflowEvent(void)
{

}

// *****************************************************************************

void HWTimer1_STM32_CompareMatchEvent(void)
{

}

// *****************************************************************************

void HWTimer1_STM32_SetOverflowCallback(HWTimerOverflowCallbackFunc Function)

{
    OverflowCallback = Function;
}

// *****************************************************************************

void HWTimer1_STM32_SetCompareMatchCallback(HWTimerCompareMatchCallbackFunc Function)

{
    CompareMatchCallback = Function;
}

/*
 End of File
 */