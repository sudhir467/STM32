

#include "stm32f446xx.h"
#include "main.h"

void HAL_MspInit(void)
{
	//Here will do low level processor specific inits.
	//1. Set up the priority grouping of the arm cortex mx processor

	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

	//2. Enable the required system exceptions of the arm cortex mx processor
	SCB->SHCSR |= 0x7 << 16; //usage fault, memory fault and bus fault system exceptions

	//3. configure the priority for the system exceptions
	HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
	HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
	HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);


}

void HAL_UART_MspInit(UART_HandleTypeDef *haurt2)
{
	GPIO_InitTypeDef ugpio;

	//1.Enable the clock
	__HAL_RCC_USART2_CLK_ENABLE();

	//2. Pin-configurations for UART2
	ugpio.Pin=GPIO_PIN_2;
	ugpio.Mode=GPIO_MODE_AF_PP;
	ugpio.Speed=GPIO_SPEED_FREQ_LOW;
	ugpio.Alternate=GPIO_AF7_USART2;
	ugpio.Pull=GPIO_PULLUP;
	HAL_GPIO_Init(GPIOA, &ugpio);

	ugpio.Pin=GPIO_PIN_3;
	HAL_GPIO_Init(GPIOA, &ugpio);

	//3.NVIC settings
	HAL_NVIC_EnableIRQ(USART2_IRQn);
	HAL_NVIC_SetPriority(USART2_IRQn,15,0);


}

void HAL_CAN_MspInit(CAN_HandleTypeDef *hcan)
{
	GPIO_InitTypeDef ucan;
	__HAL_RCC_CAN1_CLK_ENABLE();
	ucan.Pin=GPIO_PIN_11 | GPIO_PIN_12;
	ucan.Mode= GPIO_MODE_AF_PP;
	ucan.Alternate=GPIO_AF9_CAN1;
	ucan.Speed=GPIO_SPEED_FREQ_VERY_HIGH;
	ucan.Pull=GPIO_NOPULL;

	HAL_GPIO_Init(GPIOA, &ucan);

}
