/*
 * This application is to Operate the CAN in loop Back mode
 *
 * Author:Sudhir Borra.
 *
 */

#include <string.h>
#include "stm32f4xx_hal.h"
#include "main.h"

void GPIO_Init(void);
void Error_handler(void);
void UART2_Init(void);
void SystemClock_Config_HSI(uint8_t clock_freq);
void CAN1_Init(void);
void CAN1_Tx(void);

UART_HandleTypeDef huart2;
CAN_HandleTypeDef hcan;

char msg[50];

int main()
{
	HAL_Init();

	SystemClock_Config_HSI(SYS_CLOCK_FREQ_50MHZ);

	GPIO_Init();

	UART2_Init();

	CAN1_Init();

	CAN1_Tx();

	while(1)
	{

	}





}

void SystemClock_Config_HSI(uint8_t clock_freq) {
	RCC_OscInitTypeDef osc_init;
	RCC_ClkInitTypeDef clk_init;
	uint32_t FLatency = 0;

	osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	osc_init.HSIState = RCC_HSI_ON;
	osc_init.HSICalibrationValue = 16;
	osc_init.PLL.PLLState = RCC_PLL_ON;
	osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSI;

	switch (clock_freq)
	{

	case SYS_CLOCK_FREQ_50MHZ:
	{
		osc_init.PLL.PLLM = 16;
		osc_init.PLL.PLLN = 100;
		osc_init.PLL.PLLP = 2;
		osc_init.PLL.PLLQ = 2;
		osc_init.PLL.PLLR = 2;

		clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK
				| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
		clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
		clk_init.APB1CLKDivider = RCC_HCLK_DIV2;
		clk_init.APB2CLKDivider = RCC_HCLK_DIV2;

		FLatency = FLASH_ACR_LATENCY_1WS;
		break;

	}

	case SYS_CLOCK_FREQ_84MHZ:

	{
		osc_init.PLL.PLLM = 16;
		osc_init.PLL.PLLN = 168;
		osc_init.PLL.PLLP = 2;
		osc_init.PLL.PLLQ = 2;
		osc_init.PLL.PLLR = 2;

		clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK
				| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
		clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
		clk_init.APB1CLKDivider = RCC_HCLK_DIV2;
		clk_init.APB2CLKDivider = RCC_HCLK_DIV2;

		FLatency = FLASH_ACR_LATENCY_2WS;
		break;
	}

	case SYS_CLOCK_FREQ_120MHZ: {
		osc_init.PLL.PLLM = 16;
		osc_init.PLL.PLLN = 240;
		osc_init.PLL.PLLP = 2;
		osc_init.PLL.PLLQ = 2;
		osc_init.PLL.PLLR = 2;

		clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK
				| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
		clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
		clk_init.APB1CLKDivider = RCC_HCLK_DIV4;
		clk_init.APB2CLKDivider = RCC_HCLK_DIV2;

		FLatency = FLASH_ACR_LATENCY_3WS;
		break;
	}

	default:
		return;
	}
	if (HAL_RCC_OscConfig(&osc_init) != HAL_OK) {
		Error_handler();
	}
	if (HAL_RCC_ClockConfig(&clk_init, FLatency) != HAL_OK) {
		Error_handler();
	}
	//systick configuration

	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

}

void CAN1_Init(void)
{

	hcan.Instance=CAN1;
	hcan.Init.Mode=CAN_MODE_LOOPBACK;
	hcan.Init.AutoWakeUp=DISABLE;
	hcan.Init.AutoRetransmission=ENABLE;
	hcan.Init.AutoBusOff=DISABLE;
	hcan.Init.ReceiveFifoLocked=DISABLE;
	hcan.Init.TimeTriggeredMode=DISABLE;
	hcan.Init.TransmitFifoPriority=DISABLE;

	//CAN bit timings
	hcan.Init.Prescaler=5;
	hcan.Init.SyncJumpWidth=CAN_SJW_1TQ;
	hcan.Init.TimeSeg1=CAN_BS1_8TQ;
	hcan.Init.TimeSeg2=CAN_BS1_2TQ ;

	if(	HAL_CAN_Init(&hcan)!=HAL_OK)
	{
		Error_handler();
	}
}


void CAN1_Tx(void)
{
	CAN_TxHeaderTypeDef TxHeader;
	uint32_t TxMailbox;
	uint8_t our_message[5]={'H','E','L','L','O'};

	TxHeader.DLC=5;
	TxHeader.StdId=0x65D;
	TxHeader.IDE=CAN_ID_STD;
	TxHeader.RTR=CAN_RTR_DATA;

	if(HAL_CAN_AddTxMessage(&hcan, &TxHeader, our_message, &TxMailbox)!=HAL_OK)
	{
		Error_handler();
	}

	while(HAL_CAN_IsTxMessagePending(&hcan, TxMailbox));
	sprintf(msg,"Message Transmitted\r\n");
	HAL_UART_Transmit(&huart2, (uint8_t*)msg,strlen(msg), HAL_MAX_DELAY);



}

void GPIO_Init(void) {
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitTypeDef ledgpio;
	ledgpio.Pin = GPIO_PIN_5;
	ledgpio.Mode = GPIO_MODE_OUTPUT_PP;
	ledgpio.Pull = GPIO_NOPULL;

	HAL_GPIO_Init(GPIOA, &ledgpio);

}

void UART2_Init(void) {
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.Parity = UART_PARITY_NONE;

	if (HAL_UART_Init(&huart2) != HAL_OK) {
		Error_handler();

	}
}

void Error_handler(void) {
	while (1);
}
