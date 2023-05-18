

#include"stm32f4xx_hal.h"

void HAL_MspInit(void)
{
 //Here will do low level processor specific inits.
	//1. Set up the priority grouping of the arm cortex mx processor
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
	//2. Enable the required system exceptions of the arm cortex mx processor
	//This will enable the System Control Block exceptions such as memory fault, bus fault and usage fault in the  System Handler control and state register

	SCB->SHCSR |=0x7 << 16;

	//3. configure the priority for the system exceptions
	HAL_NVIC_SetPriority( MemoryManagement_IRQn ,0,0);
	HAL_NVIC_SetPriority( BusFault_IRQn  ,0,0);
	HAL_NVIC_SetPriority(  UsageFault_IRQn  ,0,0);

}

void  HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htimer)
{
	//1.Enable the clock for the peripheral
	__HAL_RCC_TIM6_CLK_ENABLE();
	//2.Enable the IRQ of timer 6
	HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
	//3.setup the priority
	HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 15, 0);
}
