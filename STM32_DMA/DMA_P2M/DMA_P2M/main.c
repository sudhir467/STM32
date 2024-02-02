
/* 
* Firmware for Peripheral to Memory Data Transfer using DMA
*     -> We have used  ADC- Multi channel and Internal temperature sensor and transfer the 
*		     Data using DMA from Peripheral to Memory 
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
	ADC1->SQR1 |=(2<<20); // SQR1_L=1 for 3 conversions
	
	/*7. Set the Respective GPIO pins in Analog mode*/
	GPIOA-> MODER |= (3<<2); // Analog mode for PA1
	GPIOA-> MODER |= (3<<8); // Analog mode for PA4
	
	/**********************************************************************************/
	/*For  Internal temperature Sensor*/
	ADC1->SMPR1 |= (7<<24); // sampling time of 21 us as 10us is min sampling time
	/*Set the TSVREFE bit to wake the sensor*/
	ADC->CCR  |=(1<<23); // Set bit 23 to set the bit
	/***********************************************************************************/
	/*Enable DMA for ADC*/
	ADC1->CR2 |= (1<<8);
	
	/*Enable Continous Request*/
	ADC1->CR2 |=(1<<9); // DMA requests are issued as long as data are converted and DMA=1
	
	/*Set the Sequence of conversion of each channel*/
	ADC1->SQR3 |= (1<<0); // SEQ1 for channel 1
	ADC1->SQR3 |= (4<<5); //SEQ2 for channel 4 i.e. at 5 th bit write 4  as 5 bit set sq 2
	ADC1->SQR3 |= (18<<10); //SEQ3 for channel 10 i.e. temperature sensor
	
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

void ADC_Start(void)
{
	/*********************STEPs to follow**************************
	1. Clear the status register
	2. Start the conversion by setting the SWSTART bit in CR2
	**************************************************************/
	
	ADC1-> SR=0;  // Clear the status register
	ADC1->CR2 |= (1<<30); // start the conversion
}

void DMA_Init(void)
{
	/********************** STEPs to follow ***********************
	1. Enable DMA clock
	2. Set the DATA Direction in DMA stream cinfiguration register DMA_SxCR
	3. Enable/Disable the Circular mode
	4. Enable/Disable the Memory Increment and Peripheral Increment
	5. Set the Data size
	6. Select the channel for the Stream
	***************************************************************/
	/* 1. Enable the DMA 2 clock */
	RCC->AHB1ENR |=(1<<22); // DMA2 Enable 
	/* 2. Select the Data Direction Stream 0 is where the ADC1 is connected to DMA2*/
	DMA2_Stream0->CR &= ~(3<<6); // Peripheral to Memory
	/* 3. Select Circular Mode*/
	DMA2_Stream0->CR |= (1<<8); // Circular Mode
	/* 4. Enable Memory Address Increment*/
	DMA2_Stream0-> CR |=(1<<10); // Memory address pointer is incremented after each data transfer
	/* 5. Set the size for data- Memory data size and Peripheral data size*/
	DMA2_Stream0-> CR |= (1<<11)|(1<<13);  // 16 bit data size as we are Using 12 bit ADC so 16 bit data size
	/* 6. Select the channel for the Stream*/
	DMA2_Stream0->CR &= ~ (7<<25); // Channel 0 selected
			
}

void DMA_Config(uint32_t srcAdd, uint32_t destAdd, uint16_t size)
{
	/****************************STEPS to follow***************************************************
	1. Set the data size(to be transferred through DMA) in the NDTR register (it tells the amount of data
	still to be transferred-> We know that after every transfer the value will decrease, if the value 
	reaches 0 the DMA transfer will stop even if DMA is enabled. We can update this only when the stream 
	is disabled.(So DMA is still not enabled).
	2. Set the Peripheral Address and Memory Address
	3. Enable the DMA stream
	
	Note: Some peripherals dont need a start condition, like UART. So as soon as you enable the DMA
	the transfer will begin. While peripherals like ADC needs the start condition, so start the ADC 
	later in the program to enable the transfer
	***********************************************************************************************/
	/*1. Set the Size*/
	DMA2_Stream0->NDTR = size; // Set the Size of the transfer
	
	/*2. Set the Peripheral add and Memory Address*/
	DMA2_Stream0->PAR = srcAdd; // Source address is peripheral address
	
	DMA2_Stream0->M0AR= destAdd; // Destination address is memory address
	
	/*3. Enable the DMA stream*/
	DMA2_Stream0->CR |=(1<<0);  // Enable the DMA	
	
}

/*
This function should be used if we dont want circular mode in DMA
  -> We will disable the Circular  mode transfer
  -> It can called at a point in the program, and once the conversion 
     is complete the DMA will stop
*/
void DMA_Go(uint16_t datasize)
{
/*
	If the Overrun occurs(OVR=1)
	Disable and re/enable the ADC to prevent the data corruption
*/
	if((ADC1->SR) & (1<<5))
	{
		ADC1->CR2 &= ~(1<<0); // ADON =0 disable ADC1
		ADC1->CR2 |= 1<<0 ;   // ADON=1 enable ADC1
	}
	
	/* 
	To start the DMA again, we need to update the NDTR counter and also the the
	Interrupt flag must be cleared, NDTR can be updated only when DMA is disabled
	*/
	
	/*Disable the DMA2*/
	DMA2_Stream0->CR &= ~(1<<0);
	
	/*Clear the Interrupt pending Flags. Important before restarting the DMA*/
	DMA2->LIFCR = 0xffffffff;
	DMA2->HIFCR = 0xffffffff;
	
	/*Set the data size in NDTR register*/
	DMA2_Stream0 ->NDTR =datasize;
	
	/*Enable the DMA2*/
	DMA2_Stream0->CR  |=(1<<10);
	
	/*Start the ADC again*/
	ADC_Start();	
}


uint16_t RxData[3];
float Temperature;

int main(void)
{
	SysClockConfig();
	TIM6Config();
	
	ADC_Init();
	ADC_Enable();
	DMA_Init();
	
	DMA_Config((uint32_t)&ADC1->DR, (uint32_t) RxData, 3);
	
	ADC_Start();
	
	
	while(1)
	{
		
		/********************************************************
		Formula: ((Vsense-V25)/Avg_slope) + 25
							where:  Vsense= ADC value
										  V25=0.76
										  Avgslope=2.5mv/°C ->0.0025v
		********************************************************/
		
		Temperature = (((float)(3.3*RxData[2]/(float)4095) - 0.76) / 0.0025) + 25;
		
		//DMA_Go();
		
		Delay_ms(1000);
		
	}
}



