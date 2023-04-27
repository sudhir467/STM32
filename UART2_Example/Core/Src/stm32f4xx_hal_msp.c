

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

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
  //Here we are going to do the low level inits of the USART2 peripheral
	//1.Enable the clock for the USART2 peripheral
	__HAL_RCC_USART2_CLK_ENABLE();
	//2.Pin Muxing configuration

	//3.Enable the IRQ and set up the priority(NVIC setting)
}
