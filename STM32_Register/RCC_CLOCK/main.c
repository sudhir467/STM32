

#include "stm32f446xx.h"

#define PLL_M 	4
#define PLL_N 	180
#define PLL_P 	0     // PLLP=2

void SysClockconfig(void);
void GPIO_Config( void);
void delay(uint32_t time);

void SysClockconfig(void)
{
	/* Steps to be followed are as follows:
	1. Enable HSE and wait for the HSE to become Ready
	2. Set The power enable clock and voltage regulator
	3. Configure the Flash prefetch and the latency related settings
	4. Configure the prescalar HCLK,PCLK,PCLK2
	5. Configure the main PLL
	6. Enable the PLL and wait for it to become ready
	7. Select the clock source and wait for it to be set
	*/ 
	
	
	// 1.Enable HSE and wait for the HSE to become Ready
	RCC->CR |=RCC_CR_HSEON;
	while(!(RCC->CR & RCC_CR_HSERDY));
	
	// 2. Set The power enable clock and voltage regulator
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	PWR->CR |=PWR_CR_VOS;
	
	//3.Configure the Flash prefetch and the latency related settings
	FLASH->ACR |=FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN | FLASH_ACR_LATENCY_5WS;
	
	//4.Configure the prescalar HCLK,PCLK,PCLK2
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
	
	RCC->CFGR |=RCC_CFGR_PPRE1_DIV4;
	
	RCC->CFGR |=RCC_CFGR_PPRE2_DIV2;
	
	//5.Configure the main PLL
	
	RCC->PLLCFGR |= (PLL_M << 0) | (PLL_N<<6) | (PLL_P <<16) |(RCC_PLLCFGR_PLLSRC_HSE);
	
	//6. Enable the PLL and wait for it to become ready
	
	RCC->CR |= RCC_CR_PLLON;
	while(!(RCC->CR & RCC_CR_PLLRDY));
	
	//7.Select the clock source and wait for it to be set
	
	RCC->CFGR |=RCC_CFGR_SW_PLL;
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
		
	
}

void GPIO_Config( void)
{
	/* 
	1.Enable the GPIO CLOCK
	2.Set the PIN as OUTPUT
	3.COnfigure the OUTPUT MODE	
	*/
	
	//1. Enable the GPIO CLOCK i.e PA5 for LED blinking
	RCC->AHB1ENR |=(1<<0);

	//2. Set the PIN as OUTPUT
	GPIOA->MODER |=(1<<10); 
	
	//3. COnfigure the output mode
	GPIOA->OTYPER=0;
	GPIOA->OSPEEDR=0;
	
}
	
void delay(uint32_t time)
{
	while(time--);
}


int main(void)
{
	SysClockconfig();
	GPIO_Config();
	
	while(1)
	{
		GPIOA->BSRR |=(1<<5); //Set the pin PA5
		delay(1000000);
		GPIOA->BSRR |=(1<<21); //Reset pin PA5
		delay(1000000);
	}
	
}