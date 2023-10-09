/* 
*********************** OPERATION OF CODE*********************
* This program is to Transfer data from Memory to Peripheral using DMA
* Author: Sudhir Borra.
* ->Here we will be using 16 Mhz default system clock
* ->When ever the button is pressed the data transfer happens from Memory to UART
***************************************************************
*/


#include<stdint.h>
#include "stm32f446xx.h"
#include "main.h"


#define BASE_ADDR_OF_GPIOC_PERI 	GPIOC

char data_stream[] ="Hello World\r\n";

/* Function to test the UART communication*/
void send_some_data(void)
{
	char somedata[]="Hello world \r\n";
	USART_TypeDef *pUART2;
	pUART2 = USART2;
	
	/*
	* We put data byte by byte into the data register for Tx, so we need to make sure
	* that in status register TXE is set i.e.( Transmit data register is empty)
	* if TXE is 1 i.e. empty then put the byte
	*/
	
	uint32_t len=sizeof(somedata);
	
	for(uint32_t i=0; i<len;i++)
	{
		while(!(pUART2->SR & (1<<7))); //Wait until its set
		pUART2->DR=somedata[i];
	}
}

/*Function for Button*/
void button_init(void)
{
	/*Button is connected to PC13. So GPIOC pin number 13*/
	GPIO_TypeDef *pGPIOC;
	pGPIOC= BASE_ADDR_OF_GPIOC_PERI;
	
	RCC_TypeDef *pRCC;
	pRCC= RCC;
	
	EXTI_TypeDef *pEXTI;
	pEXTI=EXTI;
	
	SYSCFG_TypeDef *pSYSCFG;
	pSYSCFG= SYSCFG;	
	
	/*1. Enable the peripheral clock for the GPIOC peripheral*/
	pRCC->AHB1ENR |=(1<<2);	
	
	/*2. Keep the GPIO pin in input mode*/
	pGPIOC->MODER &=~(0x3 <<26);
	
	/*3. Enable the interrupt over the GPIO pin*/
	pEXTI->IMR |=(1<<13);
	
	/*4. Enable the clock of SYSCFG*/
	pRCC->APB2ENR |=(1<<14);
	
	/*5. Configure the SYSCFG CR4 register*/
	pSYSCFG->EXTICR[3] &= ~(0xF<<4); 	// clearing
	pSYSCFG->EXTICR[3] |= (0x2 <<4);	//Set
	
	/*6. Configure the edge detection over the GPIO pin*/
	pEXTI->FTSR |=(1<<13);
	
	/*7. Enable the IRQ related to that GPIO pin in NVIC register of the processor*/
	NVIC_EnableIRQ(EXTI15_10_IRQn);
	
}

/*Function for UART*/
void uart2_init(void)
{
	RCC_TypeDef *pRCC;
	pRCC=RCC;
	
	GPIO_TypeDef *pGPIOA;
	pGPIOA=GPIOA;
	
	USART_TypeDef *pUART2;
	pUART2=USART2;
	
	/*1. Enable the peripheral clock for the UART2 peripheral*/
	pRCC->APB1ENR |=(1<<17);
	
	
	/****
	**** 2.Configure the GPIO pins for UART_Tx and UART_Rx Functionality 
	**** PA2-> Tx
	**** PA3-> Rx
	*****/
	
	/*2.1 Enable the clock for the GPIOA peripheral*/
	pRCC->AHB1ENR |=(1<<0);
	
	/*2.2 Change the mode of PA2 to alternate function mode*/
	pGPIOA->MODER &= ~(0x3 <<4);  //clear the MODE before setting the Alternate functionality
	pGPIOA->MODER |=(0x2 <<4);		// Set the Alternate functionality for PA2
	pGPIOA->AFR[0] &=~(0xF<<8);  //clear first, AFR low register so AFR[0], AFR high register then AFR[1]
	pGPIOA->AFR[0] |=(0x7<<8);	 // Set the Pin to Seven mode
	
	/*2.3 Enable or disable pull up resistor of GPIO pin if required*/
	pGPIOA->PUPDR |=(0x1<<4); // Enables the pull up resistor on GPIOA pin number 2
	
	/*2.4 Change the mode of the PA3 to alterante function mode*/
	pGPIOA->MODER &= ~(0x3<< 6); 
	pGPIOA->MODER |= (0x2<< 6);
	pGPIOA->AFR[0] &= ~( 0xF<< 12);
	pGPIOA->AFR[0] |= ( 0x7<< 12);
	
	/*2.5 Enable or disable pull up resistor for PA3*/
	pGPIOA->PUPDR |= (0x1 << 6);	
	
	/*3.Configure Baudrate*/
	pUART2->BRR=0x8B;   // From ref manual 8.6875 for 16Mhz, so convert to hex 8->8, 0.6875*16 becoz hex =11 so-> 8B
	
	
	/*4. Configure the data width, no of stop bits*/
	/* Use default values 1 stop bit, Even parity, 8 data bits
	*  So no need to set any thing
	*/
	
	
	
	/*5.Enable the Tx engine of the UART peripheral*/
	pUART2->CR1|=(1<<3); // Enable the TX
	
	
	/*6. Enable the UART peripheral*/
	pUART2->CR1|=(1<<13);	
	
}

/*Function for DMA1*/
void dma1_init(void)
{
	RCC_TypeDef *pRCC;
	pRCC = RCC;
	
	DMA_TypeDef *pDMA;
	pDMA=DMA1;
	
	DMA_Stream_TypeDef *pSTREAM6;
	pSTREAM6 = DMA1_Stream6;
	
	USART_TypeDef *pUART2;
	pUART2 = USART2;	
	
	/*1. Enable the peripheral clock for the DMA1*/
	pRCC->AHB1ENR |=(1<<21);
	
	
	/*2. Identify the stream which is suitable for your peripheral ref to DMA section,USART2->channel 4, stream6*/
	
	
	/*3. Identify the channel number on which UART2 peripheral sends DMA request i.e. channel 4*/
	pSTREAM6->CR &=~(0x7 <<25);
	pSTREAM6->CR |=(0x4 <<25); //100-> 0x4
	
	
	/*4. Program the source address (memory)*/
	pSTREAM6->M0AR=(uint32_t)data_stream;
	
	
	/*5.Program the destination address (peripheral)*/
	pSTREAM6->PAR = (uint32_t)&pUART2->DR;
	
	
	/*6. Program number of data items to send*/
	uint32_t len=sizeof(data_stream);
	pSTREAM6->NDTR=len; //Number of data items to be transferred are 0 to 65535 i.e 2 power 16
	
	
	/*7. The direction of data transfer -> m2p,p2m,m2m*/
	pSTREAM6->CR |=(0x1 <<6);    //01 for m2p 
	
	
	/*8.Program the source and destination data width i.e. How you want dma to send the data
			Byte by Byte or Word by Word or Half word by Half word
			Here UART peripheral can take byte by byte only USART->DR register only 8 bits are enabled
	*/
	pSTREAM6->CR &=~(0x3 <<13); // Memory data size
	pSTREAM6->CR &=~(0x3 <<11);	// Peripheral data size
	
	/*8.a. Auto increment the memory or it will send just H several times as the memory is not incremented */
	pSTREAM6->CR |= ( 1 << 10);
	
	
	/*9. Direct mode or FIFO mode*/
	pSTREAM6->FCR |=(1<<10); // Writing 1 will disable the direct mode thus its FIFO mode

	
	/*10. Select the FIFO thresold*/
		pSTREAM6->FCR &=~(0x3 << 0); // Clearing
		pSTREAM6->FCR |=(0x3 << 0);	 // Setting  FULL FIFO thresold
		
	/*11. Enable or Disable circular mode -> Its disabled  by default*/
	
	/*12. Single tranfer or burst transfer -> Its single transfer by default*/
	
	/*13. Configure stream priority- Its by default low priority*/
	
	
}

/*Function to Enable DMA1*/
void enable_dma1_stream6(void)
{
	DMA_Stream_TypeDef *pSTREAM6;
	pSTREAM6 = DMA1_Stream6;
	
	pSTREAM6->CR |= ( 1 << 0); //Enable the stream
}


/*Function to Disable DMA1*/
void disable_dma1_stream6(void)
{
	DMA_Stream_TypeDef *pSTREAM6;
	pSTREAM6 = DMA1_Stream6;
	
	pSTREAM6->CR &= ~( 1 << 0); // Disable the stream
	
}


/* Function for DMA interrupt*/
void dma1_interrupt_configuration(void)
{
	DMA_Stream_TypeDef *pSTREAM6;
	pSTREAM6 = DMA1_Stream6;
	
	/*1. Half Transfer Interrupt Enable (HTIE)*/
	pSTREAM6->CR |= (1 << 3);
	
	/*2. Transfer complete Interrupt Enable (TCIE)*/
	pSTREAM6->CR |= (1 << 4);
	
	/*3. Transfer Error Interrupt Enable (TEIE)*/
	pSTREAM6->CR |= (1 << 2);
	
	/*4. FIFO overrun/underrun IE(FEIE)*/
	pSTREAM6->FCR |= (1 << 7);
	
	/*5. Direct Mode Error(DMEIE)*/
	pSTREAM6->CR |= (1 << 1);
	
	/*6. Enable the IRQ for DMA1 stream6 global interrupt in NVIC*/
	NVIC_EnableIRQ(DMA1_Stream6_IRQn);
	
}

/*Half transfer Interrupt call back function*/
void HT_Complete_callback(void)
{

}


/* Full transfer complete interrupt call back function*/
void FT_Complete_callback(void)
{
	USART_TypeDef *pUART2;
	pUART2 = USART2;
	
	DMA_Stream_TypeDef *pSTREAM6;
	pSTREAM6 = DMA1_Stream6;
	
	//Program number of data items to send 
	uint32_t len = sizeof(data_stream);
	pSTREAM6->NDTR = len;
	
	pUART2->CR3 &= ~( 1 << 7);
	
	enable_dma1_stream6();
		
}


/*Transfer Error interrupt call back function*/
void TE_error_callback(void)
{
	while(1);
}

/*FIFO Error interrupt call back function*/
void FE_error_callback(void)
{
	
	while(1);
}

/*Direct Memory Interrupt call back function*/
void DME_error_callback(void)
{
	while(1);
}


int main(void)
{
	button_init();
	uart2_init();
	dma1_init();
	dma1_interrupt_configuration();
	enable_dma1_stream6();
	
	
	while(1)
	{
	}
}

