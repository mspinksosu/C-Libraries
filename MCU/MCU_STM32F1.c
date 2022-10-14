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

#include "IMCU.h"
#include "stm32f10x_map.h"
#include "stm32f10x_nvic.h"
#include "stm32f10x_systick.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

static uint32_t nvicIntReg0, nvicIntReg1, sysTickCtrlReg, sysTickCount;

// ***** Static Function Prototypes ********************************************

/* Put static function prototypes here */


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void MCU_STM32_EnterLPMAutowake(uint16_t timeInSeconds)
{
    /* Store NVIC interrupts, then disable all. But keep the RTC alarm
    interrupt enabled */
    nvicIntReg0 = NVIC->ICER[0];
    nvicIntReg1 = NVIC->ICER[1];
    NVIC->ICER[0] = 0xFFFFFFFF;
    NVIC->ICER[1] = 0xFFFFFFFF;
    /* IRQChannel / 32 = 1 << (IRQChannel mod 32) */
    NVIC->ISER[(RTCAlarm_IRQChannel >> 5)] = 1 << (RTCAlarm_IRQChannel & 0x1F);
    RTC->CRL &= ~RTC_CRL_ALRF; // clear alarm flag
    /* Wait for RSF sync before reading RTC registers - RM page 488 */
    while(RTC->CRL & RTC_CRL_RSF == 0){}
    RTC->CRL |= RTC_CRL_CNF; // enter config mode
    RTC->ALRH = RTC->CNTH;
    RTC->ALRL = RTC->CNTL + timeInSeconds; // set alarm
    RTC->CRL &= ~RTC_CRL_CNF; // exit config mode
    while(RTC->CRL & RTC_CRL_RTOFF == 0){} // wait for last RTC operation to finish
    RTC->CRH |= RTC_CRH_ALRIE; // enable alarm interrupt
    NVIC->ICPR[0] = 0xFFFFFFFF;
    NVIC->ICPR[1] = 0xFFFFFFFF;
    SysTick_ITConfig(DISABLE);
    __WFI(); // go to sleep/stop
    NVIC->ISER[0] = nvicIntReg0;
    NVIC->ISER[1] = nvicIntReg1;
    SysTick_ITConfig(ENABLE);
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void MCU_DelayUs(uint16_t microseconds)
{
    while(microseconds)
    {
        asm("NOP");
        asm("NOP");
        asm("NOP");
        microseconds--;
    }
}

// *****************************************************************************

void MCU_DelayMs(uint16_t milliseconds)
{
    /* The SysTick timer should be set up to provide a 1 ms tick. The clock
    source of the SysTick timer is usually the processor clock / 8. The counter
    counts down to zero. Core Programming Manual 4.5.1 */
    sysTickCtrlReg = SysTick->CTRL;
    SysTick->CTRL |= SysTick_CTRL_ENABLE;

    sysTickCount = SysTick->VAL + 1;
    while(sysTickCount > SysTick->VAL);
    SysTick->CTRL = sysTickCtrlReg;
}

// *****************************************************************************

void MCU_EnterLowPowerMode(MCUPowerMode powerMode)
{
    /* When exiting stop mode (SLEEPDEEP = 1) the internal oscillator is used.
    Therefore, the external oscillator (HSE) must be disabled and switched to
    internal (HSI) before going into stop mode. - RM page 75 */
    
    /* Interrupts must be disabled at the NVIC, or else the WFI command will
    not work. Also, the system tick interrupt must be disbaled as well. I also
    went ahead and cleared pending interrupts on the NVIC, although it hasn't 
    made any difference in my tests - MS */

    switch(powerMode)
    {
        case MCU_LPM_LEVEL_1:
            /* Store NVIC interrupts, then disable all. */
            nvicIntReg0 = NVIC->ICER[0];
            nvicIntReg1 = NVIC->ICER[1];
            NVIC->ICER[0] = 0xFFFFFFFF;
            NVIC->ICER[1] = 0xFFFFFFFF;
            NVIC->ICPR[0] = 0xFFFFFFFF; // clear pending interrupts just in case
            NVIC->ICPR[1] = 0xFFFFFFFF;
            SysTick_ITConfig(DISABLE);
            __WFI(); // go to sleep/stop
            NVIC->ISER[0] = nvicIntReg0; // wake and restore interrupts
            NVIC->ISER[1] = nvicIntReg1;
            SysTick_ITConfig(ENABLE);
            break;
        case MCU_LPM_LEVEL_2:
            nvicIntReg0 = NVIC->ICER[0];
            nvicIntReg1 = NVIC->ICER[1];
            NVIC->ICER[0] = 0xFFFFFFFF;
            NVIC->ICER[1] = 0xFFFFFFFF;
            NVIC->ICPR[0] = 0xFFFFFFFF;
            NVIC->ICPR[1] = 0xFFFFFFFF;
            /* TODO need to implement clock switching first. Uncomment the rest
            of these lines when clock switching is working */
            //PWR->CR &= ~PWR_CR_PDDS; // choose stop mode when CPU enters deep sleep
            //PWR->CR |= PWR_CR_LPDS; // regulator in low-power when CPU in stop mode
            //SCB->SCR |= SCB_SCR_SLEEPDEEP; // enable stop mode in cortex control register
            SysTick_ITConfig(DISABLE);
            __WFI(); // go to sleep/stop
            //SCB->SCR &= ~SCB_SCR_SLEEPDEEP;
            NVIC->ISER[0] = nvicIntReg0;
            NVIC->ISER[1] = nvicIntReg1;
            SysTick_ITConfig(ENABLE);
            break;
    }
}

/*
 End of File
 */