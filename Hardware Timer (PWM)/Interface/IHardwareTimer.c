/***************************************************************************//**
 * @brief Hardware Timer Interface
 * 
 * @author Matthew Spinks
 * 
 * @date 9/17/22  Original creation
 * 
 * @file IHardwareTimer.c
 * 
 * @details
 *      // TODO
 * 
 ******************************************************************************/

#include "IHardwareTimer.h"
#include <stddef.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************


// ***** Static Function Prototypes ********************************************


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void HWTimer_Create(HWTimer *self, HWTimerInterface *interface)
{
    self->interface = interface;
}

// *****************************************************************************

void HWTimer_CreateInitType(HWTimerInitType *params, void *instanceOfSubclass)
{
    params->instance = instanceOfSubclass;
}

// *****************************************************************************

void HWTimer_SetInitTypeParams(HWTimerInitType *params, HWTimerPrescaleSelect prescaleSelect,
    uint16_t prescaleCounter, bool useOverflowInterrupt, bool useCompareMatchInterrupts)
{
    params->prescaleSelect = prescaleSelect;
    params->prescaleCounterValue = prescaleCounter;
    params->useOverflowInterrupt = useOverflowInterrupt;
    params->useCompareMatchInterrupts = useCompareMatchInterrupts;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void HWTimer_ComputePeriodUs(HWTimer *self, HWTimerInitType *params, 
    uint32_t desiredPeriodUs, uint32_t clkInHz, uint16_t *retDiffInTicks)
{
    if(self->interface->HWTimer_ComputePeriodUs != NULL && params->instance != NULL)
    {
        (self->interface->HWTimer_ComputePeriodUs)(params->instance, desiredPeriodUs,
            clkInHz, retDiffInTicks);
    }
}

// *****************************************************************************

void HWTimer_Init(HWTimer *self, HWTimerInitType *params)
{
    if(self->interface->HWTimer_Init != NULL && params->instance != NULL)
    {
        (self->interface->HWTimer_Init)(params->instance);
    }
}

// *****************************************************************************

HWTimerSize HWTimer_GetSize(HWTimer *self)
{
    if(self->interface->HWTimer_GetSize != NULL)
    {
        return (self->interface->HWTimer_GetSize)();
    }
    else
    {
        return 0;
    }
}

// *****************************************************************************

void HWTimer_Start(HWTimer *self)
{
    if(self->interface->HWTimer_Start != NULL)
    {
        (self->interface->HWTimer_Start)();
    }
}

// *****************************************************************************

void HWTimer_Stop(HWTimer *self)
{
    if(self->interface->HWTimer_Stop != NULL)
    {
        (self->interface->HWTimer_Stop)();
    }
}

// *****************************************************************************

void HWTimer_Reset(HWTimer *self)
{
    if(self->interface->HWTimer_Reset != NULL)
    {
        (self->interface->HWTimer_Reset)();
    }
}

// *****************************************************************************

bool HWTimer_IsRunning(HWTimer *self)
{
    if(self->interface->HWTimer_IsRunning != NULL)
    {
        return (self->interface->HWTimer_IsRunning)();
    }
    else
    {
        return false;
    }
}

// *****************************************************************************

void HWTimer_SetCount(HWTimer *self, uint16_t count)
{
    if(self->interface->HWTimer_SetCount != NULL)
    {
        (self->interface->HWTimer_SetCount)(count);
    }
}

// *****************************************************************************

uint16_t HWTimer_GetCount(HWTimer *self)
{
    if(self->interface->HWTimer_GetCount != NULL)
    {
        return (self->interface->HWTimer_GetCount)();
    }
    else
    {
        return 0;
    }
}

// *****************************************************************************

void HWTimer_AddToCount(HWTimer *self, uint16_t addToCount)
{
    if(self->interface->HWTimer_AddToCount != NULL)
    {
        (self->interface->HWTimer_AddToCount)(addToCount);
    }
}

// *****************************************************************************

uint8_t HWTimer_GetNumCompareChannels(HWTimer *self)
{
    if(self->interface->HWTimer_GetNumCompareChannels != NULL)
    {
        return (self->interface->HWTimer_GetNumCompareChannels)();
    }
    else
    {
        return 0;
    }
}

// *****************************************************************************

void HWTimer_SetCompare16Bit(HWTimer *self, uint8_t compChan, uint16_t compValue)
{
    if(self->interface->HWTimer_SetCompare16Bit != NULL)
    {
        (self->interface->HWTimer_SetCompare16Bit)(compChan, compValue);
    }
}

// *****************************************************************************

uint16_t HWTimer_GetCompare16Bit(HWTimer *self, uint8_t compChan)
{
    if(self->interface->HWTimer_GetCompare16Bit != NULL)
    {
        return (self->interface->HWTimer_GetCompare16Bit)(compChan);
    }
    else
    {
        return 0;
    }
}

// *****************************************************************************

void HWTimer_SetComparePercent(HWTimer *self, uint8_t compChan, uint8_t percent)
{
    if(self->interface->HWTimer_SetComparePercent != NULL)
    {
        (self->interface->HWTimer_SetComparePercent)(compChan, percent);
    }
}

// *****************************************************************************

uint8_t HWTimer_GetComparePercent(HWTimer *self, uint8_t compChan)
{
    if(self->interface->HWTimer_GetComparePercent != NULL)
    {
        return (self->interface->HWTimer_GetComparePercent)(compChan);
    }
    else
    {
        return 0;
    }
}

// *****************************************************************************

void HWTimer_EnableCompare(HWTimer *self, uint8_t compChan)
{
    if(self->interface->HWTimer_EnableCompare != NULL)
    {
        (self->interface->HWTimer_EnableCompare)(compChan);
    }
}

// *****************************************************************************

void HWTimer_DisableCompare(HWTimer *self, uint8_t compChan)
{
    if(self->interface->HWTimer_DisableCompare != NULL)
    {
        (self->interface->HWTimer_DisableCompare)(compChan);
    }
}

// *****************************************************************************

bool HWTimer_GetOverflow(HWTimer *self)
{
    if(self->interface->HWTimer_GetOverflow != NULL)
    {
        return (self->interface->HWTimer_GetOverflow)();
    }
    else
    {
        return false;
    }
}

// *****************************************************************************

bool HWTimer_GetCompareMatch(HWTimer *self, uint8_t compChan)
{
    if(self->interface->HWTimer_GetCompareMatch != NULL)
    {
        return (self->interface->HWTimer_GetCompareMatch)(compChan);
    }
    else
    {
        return false;
    }
}

// *****************************************************************************

void HWTimer_ClearOverflowFlag(HWTimer *self)
{
    if(self->interface->HWTimer_ClearOverflowFlag != NULL)
    {
        (self->interface->HWTimer_ClearOverflowFlag)();
    }
}

// *****************************************************************************

void HWTimer_ClearCompareMatchFlag(HWTimer *self, uint8_t compChan)
{
    if(self->interface->HWTimer_ClearCompareMatchFlag != NULL)
    {
        (self->interface->HWTimer_ClearCompareMatchFlag)(compChan);
    }
}

// *****************************************************************************

void HWTimer_OverflowEvent(HWTimer *self)
{
    if(self->interface->HWTimer_OverflowEvent != NULL)
    {
        (self->interface->HWTimer_OverflowEvent)();
    }
}

// *****************************************************************************

void HWTimer_CompareMatchEvent(HWTimer *self)
{
    if(self->interface->HWTimer_CompareMatchEvent != NULL)
    {
        (self->interface->HWTimer_CompareMatchEvent)();
    }
}

// *****************************************************************************

void HWTimer_SetOverflowCallback(HWTimer *self, HWTimerOverflowCallbackFunc Function)
{
    if(self->interface->HWTimer_SetOverflowCallback != NULL)
    {
        (self->interface->HWTimer_SetOverflowCallback)(Function);
    }
}

// *****************************************************************************

void HWTimer_SetCompareMatchCallback(HWTimer *self, HWTimerCompareMatchCallbackFunc Function)
{
    if(self->interface->HWTimer_SetCompareMatchCallback != NULL)
    {
        (self->interface->HWTimer_SetCompareMatchCallback)(Function);
    }
}

/*
 End of File
 */