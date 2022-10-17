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

static uint32_t nvicIntReg0, nvicIntReg1, sysTickCtrlReg, sysTickLoadReg, sysTickCount;

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

void MCU_DelayUs(uint16_t microseconds, uint32_t clkInHz)
{
    /* The SysTick timer can be configured to use a prescale of 1 or 8. The 
    counter is 24-bits. For a 1 ms tick the reload value at the maximum clock
    speed of 72 MHz would be 1 ms / (1 / 72 MHz) = 72000. As soon as the 
    counter is enabled, it loads the RELOAD value and begins counting down. 
    The extra subtraction is to shave off some time that it takes to do the
    instructions inside the delay loop. */
    uint32_t period1us = clkInHz / 1000000UL - 15;
    sysTickCtrlReg = SysTick->CTRL;
    /* Store the reload value and change prescale to 1 */
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE;
    sysTickLoadReg = SysTick->LOAD;
    if(!(SysTick->CTRL & SysTick_CTRL_CLKSOURCE))
        SysTick->LOAD <<= 3;
    sysTickCount = SysTick->LOAD - period1us - 5;
    /* Writing anything to the  value register will clear the count and the 
    COUNTFLAG bit. When the enable bit is set, the count will be equal to the 
    reload value */
    SysTick->VAL = 0;
    SysTick->CTRL |= (SysTick_CTRL_CLKSOURCE | SysTick_CTRL_ENABLE);
    while(microseconds > 0)
    {
        while(SysTick->VAL > sysTickCount);
        if(period1us > sysTickCount)
            SysTick->VAL = 0;
        sysTickCount = SysTick->VAL - period1us;
        microseconds--;
    }
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE;
    SysTick->LOAD >>= 3;
    SysTick->LOAD = sysTickLoadReg;
    SysTick->CTRL = sysTickCtrlReg;
}

// *****************************************************************************

void MCU_DelayMs(uint16_t milliseconds, uint32_t clkInHz)
{
    /* The SysTick timer should be set up to provide a 1 ms tick. The counter
    counts down to zero and automatically reloads the value in SysTick->LOAD.
    When the counter hits zero the COUNTFLAG bit is set.
    Core Programming Manual 4.5.1 */
    sysTickCtrlReg = SysTick->CTRL;
    SysTick->CTRL |= SysTick_CTRL_ENABLE;
    sysTickCount = SysTick->VAL;
    SysTick->CTRL &= ~SysTick_CTRL_COUNTFLAG;
    while(!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG));
    while(milliseconds > 1)
    {
        SysTick->CTRL &= ~SysTick_CTRL_COUNTFLAG;
        while(!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG));
        milliseconds--;
    }
    while(SysTick->VAL > sysTickCount);
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