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
#include "stm32f10x_rcc.h"
#include "stm32f10x_systick.h"

// ***** Defines ***************************************************************

#define HSI_HZ          8000000UL
#define HSI_TO_PLL_HZ   4000000UL
#define HSE_STARTUP     16000  // 2 ms at 8 MHz

// ***** Global Variables ******************************************************

uint8_t pllDivArray[2] = {2, 1}, pllMulArray[6] = {4,5,6,7,8,9};
uint32_t pllDivLookup[2] = {RCC_PLLSource_HSE_Div2, RCC_PLLSource_HSE_Div1};
uint32_t pllMulLookup[6] = {RCC_PLLMul_4, RCC_PLLMul_5, RCC_PLLMul_6, 
                            RCC_PLLMul_7, RCC_PLLMul_8, RCC_PLLMul_9};

static uint32_t systemClockInHz, sysTickCtrlReg, sysTickLoadReg, sysTickCount, 
                nvicIntReg0, nvicIntReg1, period1us = 8;

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

uint32_t MCU_InitSystemClock(uint32_t desiredClkInHz, uint32_t xtalInHz)
{
    if(desiredClkInHz > 72000000UL)
        desiredClkInHz = 72000000UL;

    systemClockInHz = desiredClkInHz;
    uint32_t selectedClkSource = RCC_SYSCLKSource_HSI; // HSI default
    uint32_t pllInputClk = HSI_TO_PLL_HZ;              // PLL input default 

    /* If there is an external cystal, it gets priority */
    if(xtalInHz > 0)
    {
        /* Enable external crystal (HSE) */
        RCC->CR |= RCC_CR_HSEON;

        /* Wait until HSE is ready */
        for(uint16_t count = HSE_STARTUP; count > 0; count--)
        {
            if(RCC->CR & RCC_CR_HSERDY)
                break;
        }

        if(RCC->CR & RCC_CR_HSERDY)
        {
            /* Update variables */
            selectedClkSource = RCC_SYSCLKSource_HSE;
            pllInputClk = xtalInHz;
            systemClockInHz = xtalInHz;

            /* Perform clock switch to HSE */
            uint32_t cfgrReg = RCC->CFGR & ~RCC_CFGR_SW; // clear SW bits
            cfgrReg |= RCC_SYSCLKSource_HSE;
            RCC->CFGR = cfgrReg;
        }
    }
    
    if(desiredClkInHz > pllInputClk)
    {
        /* Prepare the PLL. Max SYSCLK is 72 MHz when using external crystal. 
        Max SYSCLK is 36 MHz when using the internal oscillator (HSI). */
        int32_t difference, prev = 0x7FFFFFFF;
        uint32_t divSelect = pllDivLookup[1], mulSelect = pllMulLookup[0];
        bool match = false;
        int8_t d, m;

        /* Go through the table of multipliers and dividers and find the 
        combination that gets us closest to the desired clock frequency */
        if(selectedClkSource == RCC_SYSCLKSource_HSI)
        {
            if(desiredClkInHz > 36000000UL)
                desiredClkInHz = 36000000UL;

            /* For HSI, the input to the PLL can only be 4 MHz. */
            for(m = sizeof(pllMulArray) - 1; m >= 0; m--)
            {
                difference = (pllInputClk * pllMulArray[m]) - desiredClkInHz;
                if(difference < 0)
                    difference *= -1;
                if(difference < prev)
                {
                    mulSelect = pllMulLookup[m];
                    prev = difference;
                }
                if(difference == 0)
                {
                    break;
                }
            }
            RCC_PLLConfig(RCC_PLLSource_HSI_Div2, mulSelect);
        }
        else
        {
            /* Else, our selected clock source was HSE */
            for(d = sizeof(pllDivArray) - 1; d >= 0; d--)
            {
                for(m = sizeof(pllMulArray) - 1; m >= 0; m--)
                {
                    difference = (pllInputClk / pllDivArray[d] * pllMulArray[m]) - desiredClkInHz;
                    if(difference < 0)
                        difference *= -1;
                    if(difference < prev)
                    {
                        divSelect = pllDivLookup[d];
                        mulSelect = pllMulLookup[m];
                        prev = difference;
                    }
                    if(difference == 0)
                    {
                        match = true;
                        break;
                    }
                }
                if(match)
                    break;
            }
            RCC_PLLConfig(divSelect, mulSelect);
        }

        /* Enable PLL */
        RCC->CR |= RCC_CR_PLLON;

        /* Wait until PLL is ready */
        while(!(RCC->CR & RCC_CR_PLLRDY)){}

        /* Perform clock switch to PLL */
        selectedClkSource = RCC_SYSCLKSource_PLLCLK;
        uint32_t cfgrReg = RCC->CFGR & ~RCC_CFGR_SW; // clear SW bits
        cfgrReg |= RCC_SYSCLKSource_PLLCLK;
        RCC->CFGR = cfgrReg;

        /* Update the global variable */
        systemClockInHz = pllInputClk / pllDivArray[d] * pllMulArray[m];
    }

    /* HCLK/AHB bus clock. Max is 72 MHz */
    RCC_HCLKConfig(RCC_SYSCLK_Div1);

    /* Max clock for APB2 (PCLK2) is 72 MHz */
    RCC_PCLK2Config(RCC_HCLK_Div1);

    /* Max clock for APB1 (PCLK1) is 36 MHz */
    if(systemClockInHz > 36000000UL)
        RCC_PCLK1Config(RCC_HCLK_Div2);
    else
        RCC_PCLK1Config(RCC_HCLK_Div1);

    /* Max ADC clock is 14 MHz */
    if(systemClockInHz > 50000000UL)
        RCC_ADCCLKConfig(RCC_PCLK2_Div6);
    else if(systemClockInHz > 26000000UL)
        RCC_ADCCLKConfig(RCC_PCLK2_Div4);
    else
        RCC_ADCCLKConfig(RCC_PCLK2_Div2);

    /* Wait until selected clock source is ready */
    while(((RCC->CFGR & 0x0000000C) >> 2) != selectedClkSource){}

    /* Set the SysTick reload value. SysTick clock is HCLK / 8 by default.
    For SysTick = 1 ms: clkInHz / 8 / 1000 */
    SysTick->LOAD = (systemClockInHz / 8 / 1000);

    /* Enable SysTick interrupt and start the SysTick counter */
    SysTick->CTRL |= (SysTick_CTRL_TICKINT | SysTick_CTRL_ENABLE);

    period1us = systemClockInHz / 1000000UL;

    return systemClockInHz;
}

// *****************************************************************************

void MCU_DelayUs(uint16_t microseconds)
{
    /* The SysTick timer can be configured to use a prescale of 1 or 8. The 
    counter is 24-bits. For a 1 ms tick the reload value at the maximum clock
    speed of 72 MHz would be 1 ms / (1 / 72 MHz) = 72000. As soon as the 
    counter is enabled, it loads the RELOAD value and begins counting down. 
    The extra subtraction is to shave off some time that it takes to do the
    instructions inside the delay loop. */
    uint32_t period = period1us - 15;
    sysTickCtrlReg = SysTick->CTRL;
    /* Store the reload value and change prescale to 1 */
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE;
    sysTickLoadReg = SysTick->LOAD;
    if(!(SysTick->CTRL & SysTick_CTRL_CLKSOURCE))
        SysTick->LOAD <<= 3;
    sysTickCount = SysTick->LOAD - period1us - 5;
    /* Writing anything to the value register will clear the count and the 
    COUNTFLAG bit. When the enable bit is set, the count will be equal to the 
    reload value */
    SysTick->VAL = 0;
    SysTick->CTRL |= (SysTick_CTRL_CLKSOURCE | SysTick_CTRL_ENABLE);
    while(microseconds > 0)
    {
        while(SysTick->VAL > sysTickCount);
        if(period > sysTickCount)
            SysTick->VAL = 0;
        sysTickCount = SysTick->VAL - period;
        microseconds--;
    }
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE;
    SysTick->LOAD >>= 3;
    SysTick->LOAD = sysTickLoadReg;
    SysTick->CTRL = sysTickCtrlReg;
}

// *****************************************************************************

void MCU_DelayMs(uint16_t milliseconds)
{
    /* In this implementation, the clock frequency is not needed, as the 
    SysTick timer should be set up to provide a 1 ms tick. The counter counts 
    down to zero and automatically reloads the value in SysTick->LOAD. When the
    counter hits zero the COUNTFLAG bit is set.
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