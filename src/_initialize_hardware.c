//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include "stm32f4xx.h"
//#include "stm32f4xx_hal.h"
//#include "stm32f4xx_hal_cortex.h"

// ----------------------------------------------------------------------------

// The external clock frequency is specified as a preprocessor definition
// passed to the compiler via a command line option (see the 'C/C++ General' ->
// 'Paths and Symbols' -> the 'Symbols' tab, if you want to change it).
// The value selected during project creation was HSE_VALUE=8000000.
//
// The code to set the clock is at the end.
//
// Note1: The default clock settings assume that the HSE_VALUE is a multiple
// of 1MHz, and try to reach the maximum speed available for the
// board. It does NOT guarantee that the required USB clock of 48MHz is
// available. If you need this, please update the settings of PLL_M, PLL_N,
// PLL_P, PLL_Q to match your needs.
//
// Note2: The external memory controllers are not enabled. If needed, you
// have to define DATA_IN_ExtSRAM or DATA_IN_ExtSDRAM and to configure
// the memory banks in system/src/cmsis/system_stm32f4xx.c to match your needs.

// ----------------------------------------------------------------------------

// Forward declarations.

void
__initialize_hardware(void);

void
SystemClock_Config(void);

void
SystemTimer_Config(void);

// ----------------------------------------------------------------------------

// This is the application hardware initialisation routine,
// redefined to add more inits.
//
// Called early from _start(), right after data & bss init, before
// constructors.
//
// After Reset the Cortex-M processor is in Thread mode,
// priority is Privileged, and the Stack is set to Main.
//
// Warning: The HAL requires the system timer, running at 1000 Hz
// and calling HAL_IncTick().

void
__initialize_hardware(void)
{
	// Initialise the HAL Library; it must be the first function
	// to be executed before the call of any HAL function.
	//HAL_Init();

	// Enable HSE Oscillator and activate PLL with HSE as source
	SystemClock_Config();

	// Call the CSMSIS system clock routine to store the clock frequency
	// in the SystemCoreClock global RAM location.
	SystemCoreClockUpdate();

	/* Set up the timer */
	SystemTimer_Config();

	/* We shall use 4 priority bits */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
}

// Disable when using RTOSes, since they have their own handler.
#if 0

// This is a sample SysTick handler, use it if you need HAL timings.
void __attribute__ ((section(".after_vectors")))
SysTick_Handler(void)
{
#if defined(USE_HAL_DRIVER)
	HAL_IncTick();
#endif
}

#endif

// ----------------------------------------------------------------------------

/**
 * @brief system tick configuration
 * @param None
 * @retval None
 */
void
SystemTimer_Config(void)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
}

/**
 * @brief  System Clock Configuration
 * @param  None
 * @retval None
 */
void
__attribute__((weak))
SystemClock_Config(void)
{
	/*
	 * Set up the flash settings before changing the clock settings. See p80 of
	 * the reference manual
	 */
	/* reset art-accelerator */
	FLASH_DataCacheCmd(DISABLE);
	FLASH_InstructionCacheCmd(DISABLE);
	FLASH_PrefetchBufferCmd(DISABLE);
	FLASH_InstructionCacheReset();
	FLASH_DataCacheReset();

	/* enable i-code prefetch */
	FLASH_PrefetchBufferCmd(ENABLE);

	/* enable i-code cache */
	FLASH_InstructionCacheCmd(ENABLE);

	/* enable d-code cache */
	FLASH_DataCacheCmd(ENABLE);

	/* program flash wait states (speed and voltage dependent) */
	FLASH_SetLatency(FLASH_ACR_LATENCY_5WS);

	/* modify clock settings */
	RCC_DeInit();
	//RCC_HSEConfig(RCC_HSE_ON);
	//RCC_WaitForHSEStartUp();
	RCC_HSICmd(ENABLE);

	PWR_MainRegulatorModeConfig(PWR_Regulator_Voltage_Scale1);
	RCC_PLLConfig(RCC_PLLSource_HSI, 8, 180, 2, 4);
	RCC_PLLCmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

	/* HCLK same as sysclk */
	RCC_HCLKConfig(RCC_SYSCLK_Div1);
	/* APB1 max is 45 MHz */
	RCC_PCLK1Config(RCC_HCLK_Div4);
	/* APB2 max is 90 MHz */
	RCC_PCLK2Config(RCC_HCLK_Div2);

	/* Move sysclk to PLL */
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
}

// ----------------------------------------------------------------------------
