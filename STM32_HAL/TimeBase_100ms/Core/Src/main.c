/*
 *In this application we are Toggling the USER LED after every 100ms using TIM6
 *
 * Author: Sudhir Borra.
 *
 * */

#include "stm32f4xx_hal.h"
#include "main.h"
#include <string.h>



void Error_handler(void);
void SystemClock_Config(void);
void Timer6_Init(void);
void GPIO_Init(void);

TIM_HandleTypeDef htimer6;

int main(void) {



	HAL_Init();

	SystemClock_Config();

	GPIO_Init();

	Timer6_Init();

	HAL_TIM_Base_Start(&htimer6);

	while(1)
	{

	/*Loop until the update event flag is set*/

	while(!(TIM6->SR & (1<<0)));

	/*Required time delay has elapsed so we reached here */
	/*The bit has to be cleared by the software as its set by the hardware*/

	TIM6->SR =0;

	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
	}

	return 0;
}

void Timer6_Init(void)
{
	htimer6.Instance=TIM6;
	htimer6.Init.Prescaler=24;
	htimer6.Init.Period=64000-1;
	if(HAL_TIM_Base_Init(&htimer6)!= HAL_OK)
	{
		Error_handler();
	}

}

void GPIO_Init(void)
{
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitTypeDef ledgpio;
	ledgpio.Pin= GPIO_PIN_5;
	ledgpio.Mode=GPIO_MODE_OUTPUT_PP;
	ledgpio.Pull=GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &ledgpio);
}


void SystemClock_Config(void)
{

}


void Error_handler(void)
{
	while(1);
}
