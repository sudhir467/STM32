
/* This is bare metal firmware for ADC- Multi channel 
* Author : Sudhir Borra.
*/

#include "RccConfig_F446.h"
#include "Delay_F446.h"


void ADC_Init(void)
{
	/**********************Steps to follow*****************************
	1. Enable ADC and GPIO clock
	2. Set the prescalar in the Common control register
	3. Set the Scan mode and Resolution in the CR 1
	4. Set continous conversion, EOC and Data alignment in the CR 2
	5. Set the sampling time for the channels in ADC_SMPRx
	6. Set the Regular channel sequence length in ADC_SQR1
	7. Set the Respective GPIO pins in the Analog mode
	*******************************************************************/
	
	/* 1. Enable ADC and GPIO clock*/
	RCC->APB2ENR  |= (1<<8); // Enable the ADC1 clock
	RCC->AHB1ENR  |= (1<<0); // Enable GPIOA clock
	
	/* 2. Set the Prescalar in the CCR register*/
	ADC->CCR |= 1<<16; // Set the Prescalar divided by 4
	
	/* 3. Set the Scan mode and resolution in the CR1*/
  ADC1->CR1	= (1<<8); // Scan mode Enabled because we are using multi channel
	ADC1->CR1 &= ~(1<<24); // 12 bit resolution
	
	/*4. Set the continous conversion, EOC and Data alignment in the CR 2*/
	ADC1->CR2 =(1<<1); // Continous Conversion
	ADC1->CR2 |=(1<<10); // EOC after each conversion
	ADC1-> CR2 &= ~(1<<11); // Data Alignement RIGHT
	
	/*5. Set the Sampling time for Channels*/
	ADC1->SMPR2 &= ~(1<<7)|(7<<12); // Sampling time of 3 Cycles for Channel 1 and channel 4
	
	/*6. Set the Regular channel sequence length in ADC_SQR1*/
	ADC1->SQR1 |=(1<<20); // SQR1_L=1 for 2 conversions
	
	/*7. Set the Respective GPIO pins in Analog mode*/
	GPIOA-> MODER |= (3<<2); // Analog mode for PA1
	GPIOA-> MODER |= (3<<8); // Analog mode for PA4
	
}

void ADC_Enable(void)
{
	/********************STEPS to follow*****************************
	1. Enable the ADC  by setting ADON bit in CR2
	2. Wait for ADC to stabilize(approx 10us)
	****************************************************************/
	ADC1->CR2  |= (1<<0);  // ADON=1 enable ADC1
	uint32_t delay =10000;
	while(delay --);
}

void ADC_Start(int channel)
{
	/*********************STEPs to follow***************************************
	1. Set the channel Sequence in the SQR register -->  This register is used to 
	set the sequence in which you want the channel to do conversion i.e. if 
	if i want a channel 4 to do conversion first then we use this register to set 
	the priority.
	----------------------------------------------------------------------------
	2. Clear the status register
	3. Start the conversion by setting the SWSTART bit in CR2
	***************************************************************************/
	
	ADC1-> SQR3 =0; // First clear the sequence register 
	ADC1->SQR3 |=(channel<<0);  // Conversion in regular channel
	ADC1-> SR=0;  // Clear the status register
	ADC1->CR2 |= (1<<30); // start the conversion
}

void ADC_WaitforConv(void)
{
/* EOC flag will be set, once the conversion is finished*/
	while(!(ADC1->SR & (1<<1)));  // wait for EOC flag to set
	
}

uint16_t ADC_GetVal(void)
{
	return ADC1-> DR;  // Read the Data register
}

void ADC_Disable(void)
{
	/*Clear the ADON bit to disable the ADC*/
		ADC1->CR2 &= ~(1<<0); // Disable the ADC
}


uint16_t ADC_VAL[2] = {0,0};


int main(void)
{
	SysClockConfig();
	ADC_Init();
	ADC_Enable();
	
	while(1)
	{
		ADC_Start(1);
		ADC_WaitforConv();
		ADC_VAL[0]= ADC_GetVal();
		
		ADC_Start(4);
		ADC_WaitforConv();
		ADC_VAL[1]=ADC_GetVal();
		
	}
}



