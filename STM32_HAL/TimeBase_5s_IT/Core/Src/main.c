/*
 *In this application we are Toggling the USER LED after every 5s using TIM6 using interrupt mode
 *
 * Author: Sudhir Borra.
 *
 * */

#include "stm32f4xx_hal.h"
#include "main.h"


void Error_handler(void);
void SystemClock_Config(void);
void Timer6_Init(void);
void GPIO_Init(void);

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

TIM_HandleTypeDef htimer6;

int main(void) {



	HAL_Init();

	SystemClock_Config();

	GPIO_Init();

	Timer6_Init();
	/*Lets start the timer in the Interrupt mode*/
	HAL_TIM_Base_Start_IT(&htimer6);

	while(1);

	return 0;
}

void Timer6_Init(void)
{
	htimer6.Instance=TIM6;
	htimer6.Init.Prescaler=4999;
	htimer6.Init.Period=16000-1;
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

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
}


void SystemClock_Config(void)
{

}


void Error_handler(void)
{
	while(1);
}
