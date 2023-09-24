
#include "RccConfig.h"

void delay(uint32_t time);
void GPIO_Config(void);
void Interrupt_Config(void);
void EXTI1_IRQHandler(void);

void GPIO_Config(void)
{
	/**********STEPs to be followed*************************
	1. Enable GPIO Clock
	2. Set the required pin as INPUT mode
	3. COnfigure the PULL up /PULL down accordingly to your requirements
	**************************************************************/
	RCC->AHB1ENR |=(1<<0);
	
	//2.Set the required pin as INPUT mode
	GPIOA->MODER &= ~(3<<2);
	
	//3.COnfigure the PULL up /PULL down accordingly to your requirements
	GPIOA->PUPDR |=(1<<2);

}

void Interrupt_Config(void)
{
	/**************STEPS FOLLOWED***************************
	1. Enable the SYSCNFG bit in RCC register
	2. COnfigure the EXTI configuration Register in the SYSCNFG
	3. Enable the EXTI using Interrupt MAsk register(IMR)
	4. COnfigure the rising edge/falling edge trigger
	5. Set the Interrupt priority
	6. Enable the Interrupt
	**********************************************************/
	RCC->APB2ENR |= (1<<14);
	SYSCFG->EXTICR[0] &= ~(0xf<<4);
	EXTI->IMR |=(1<<1);
	EXTI->RTSR |=(1<<1);
	EXTI->FTSR &= ~(1<<1);
	NVIC_SetPriority(EXTI1_IRQn , 1);
	NVIC_EnableIRQ(EXTI1_IRQn);
	
}

int flag=0;
int count=0;

void EXTI1_IRQHandler(void)
{
	/*************STEPS FOLLOWED********************
	1. Check the Pin which triggerd the interrupt
	2. Clear the Interrupt Pending Bit
	************************************************/
	if(EXTI->PR & (1<<1))
	{
		EXTI->PR |= (1<<1);
 		flag=1;
	}
	
}

void delay(uint32_t time)
{
	while(time --)
	{
	}
}



int main()
{ 
	SysClockConfig();
	GPIO_Config();
	Interrupt_Config();
		
	while(1)
	{
		if(flag)
		{
			delay(10000);
			count++ ;
			flag=0;
		}			
			
	}
}