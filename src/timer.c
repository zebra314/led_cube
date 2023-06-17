#include "timer.h"

// 1 6 10 16 40
void SystemClock_Config(int speed){
	// system clock -> MSI
	RCC->CFGR &= ~RCC_CFGR_SW_Msk;
	RCC->CFGR |= RCC_CFGR_SW_MSI;

	while(!(((RCC->CFGR & RCC_CFGR_SWS_Msk)>> RCC_CFGR_SWS_Pos) == 0));    // Make sure system clock is ready

	RCC->CR &= ~RCC_CR_PLLON;                 // Disable PLL
	while((RCC->CR & RCC_CR_PLLRDY) != 0);    // Make sure PLL is ready (unlocked)

	// Set PLL to MSI
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLSRC_Msk;
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_MSI;

	// R / 0~3  2,4,6,8
	// N * 8~86 8~86
	// M / 0~7  1~8
	// initial
	int set_R=0, set_N=0, set_M=0;
	// Change R N M
	if(speed==40){
		set_R = 1;
		set_N = 40;
		set_M = 0;
	}
	else if(speed==16){
		set_R = 0;
		set_N = 8;
		set_M = 0;
	}
	else if(speed==10){
		set_R = 0;
		set_N = 5;
		set_M = 0;
	}
	else if(speed==6){
		set_R = 0;
		set_N = 12;
		set_M = 3;
	}
	else if(speed==1){
		set_R = 3;
		set_N = 8;
		set_M = 3;
	}
	else{
		// Default 4 MHz
		set_R = 3;
		set_N = 8;
		set_M = 0;
	}

	// Set PLLR
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLR_Msk;
	RCC->PLLCFGR |= (set_R << RCC_PLLCFGR_PLLR_Pos);
	// Set PLLN
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN_Msk;
	RCC->PLLCFGR |= (set_N << RCC_PLLCFGR_PLLN_Pos);
	// Set PLLM
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM_Msk;
	RCC->PLLCFGR |= (set_M << RCC_PLLCFGR_PLLM_Pos);

	// Enable PLLR
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN;

	// Enable PLL
	RCC->CR |= RCC_CR_PLLON;

	// system clock -> PLL
	RCC->CFGR &= ~RCC_CFGR_SW_Msk;
	RCC->CFGR |= RCC_CFGR_SW_PLL;

	while(!(((RCC->CFGR & RCC_CFGR_SWS_Msk)>>RCC_CFGR_SWS_Pos) == 3));    // Make sure system clock is ready
}

void SystemClock_Config_Interrupt(int speed, int load){
	SystemClock_Config(speed);
	SysTick->LOAD = load;
	SysTick->CTRL |= (1 << SysTick_CTRL_CLKSOURCE_Pos);
	SysTick->CTRL |= (1 << SysTick_CTRL_TICKINT_Pos);
	SysTick->CTRL |= (1 << SysTick_CTRL_ENABLE_Pos);
}

void timer_enable(TIM_TypeDef *timer){
	if(timer==TIM2){
		RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;    // TIM2 clock enable
	}
	else if(timer==TIM3){
		RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN;    // TIM3 clock enable
	}
}

void timer_disable(TIM_TypeDef *timer){
	if(timer==TIM2){
		RCC->APB1ENR1 &= ~RCC_APB1ENR1_TIM2EN;    // TIM2 clock enable
	}
	else if(timer==TIM3){
		RCC->APB1ENR1 &= ~RCC_APB1ENR1_TIM3EN;    // TIM3 clock enable
	}
}

void timer_init(TIM_TypeDef *timer, int psc, int arr){
	timer->PSC = (uint32_t)(psc-1);          // PreScalser
	timer->ARR = (uint32_t)(arr-1);          // Reload value
	timer->EGR |= TIM_EGR_UG;                // Reinitialize the counter
}
void timer_reload(TIM_TypeDef *timer){
	timer->EGR |= TIM_EGR_UG;
}
void timer_start(TIM_TypeDef *timer){
	timer->CR1 |= TIM_CR1_CEN;              // Set counter enabled
}
void timer_stop(TIM_TypeDef *timer){
	timer->CR1 &= ~TIM_CR1_CEN;             // Set counter disabled
}

// USE PA0 as the PWM signal output
// USE TIM2 as clock
void GPIO_init_AF(){
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	// Set to Alternate function mode
	GPIOA->MODER &= ~GPIO_MODER_MODE0_Msk;
	GPIOA->MODER |= (2 << GPIO_MODER_MODE0_Pos);
	// Set AFRL
	GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL0_Msk;
	GPIOA->AFR[0] |= (1 << GPIO_AFRL_AFSEL0_Pos);
}
void PWM_channel_init(){
	// p.883 915 920 924
	// PA0 for PWM
	// PWM mode 1
	TIM2->CCMR1 &= ~TIM_CCMR1_OC1M_Msk;
	TIM2->CCMR1 |= (6 << TIM_CCMR1_OC1M_Pos);
	// OCPreload_Enable
	TIM2->CCMR1 &= ~TIM_CCMR1_OC1PE_Msk;
	TIM2->CCMR1 |= (1 << TIM_CCMR1_OC1PE_Pos);
	// Active high for channel 1 polarity
	TIM2->CCER &= ~TIM_CCER_CC1P_Msk;
	TIM2->CCER |= (0 << TIM_CCER_CC1P_Pos);
	// Enable for channel 1 output
	TIM2->CCER &= ~TIM_CCER_CC1E_Msk;
	TIM2->CCER |= (1 << TIM_CCER_CC1E_Pos);
	// Set Compare Register
	TIM2->CCR1 = 10;
	// Set PreScaler
	TIM2->PSC = 0;
}