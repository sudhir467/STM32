
#include <stdio.h>
#include "RccConfig.h"

void TIM6config(void);
void Delay_us(uint16_t us);
void Delay_ms(uint16_t ms);
void GPIOconfig(void);

void TIM6config(void)
{
	/***************STEPS to follow****************
	1. Enable timer clock
	2. SET the prescalar and the ARR
	3. Enable the timer and wait for the flag to set		
	**************************************************/
	RCC->APB1ENR|=(1<<4);
	TIM6->PSC=90-1; //   90Mhz/90=1Mhz i.e.1us
	TIM6->ARR = 0xFFFF; // Max limit for the count
	
	TIM6->CR1 |=(1<<0);
	while(!(TIM6->SR & (1<<0)));

}

void Delay_us(uint16_t us)
{
	/***********STEPS To follow**************
	1. RESET the counter
	2. Wait for the counter to reach the entered value. 
	As each count will take 1us.
	***************************************************/
	TIM6->CNT =0;
	while(TIM6->CNT <us);
	
}

void Delay_ms(uint16_t ms)
{
	for(uint16_t i=0;i<ms;i++)
	{
		Delay_us(1000);
	}
}

void GPIOconfig(void)
{
	/**************STEPS to follow****************************
	1. Enable the GPIOA clock
	2. SET the  PA5 as output
	3. Configure the output mode
	*********************************************************/
	
	//1.	1. Enable the GPIOA clock
	RCC->AHB1ENR |=(1<<0);
	
	//	2. SET the  PA5 as output
	GPIOA->MODER |=(1<<10);
	
	//3.  Configure the output mode
	GPIOA->OTYPER &= ~(1<<5);
	GPIOA->OSPEEDR |=(1<<11);
	GPIOA->PUPDR &= ~((1<<10) | (1<<11));	
}

int main()
{
	SysClockConfig();
	TIM6config();
	GPIOconfig();
	
	while(1)
	{
		GPIOA->BSRR |=(1<<5);
		Delay_ms(1000);
		GPIOA->BSRR |=(1<<21);
		Delay_ms(1000);
	}
}
	
	
	