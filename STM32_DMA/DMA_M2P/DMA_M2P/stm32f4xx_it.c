/* 
* Interrupt Handlers of DMA transfer
* Author: Sudhir Borra
*/

#include "stm32f446xx.h"
#include "main.h"

extern void HT_Complete_callback(void);
extern void FT_Complete_callback(void);
extern void TE_error_callback(void);
extern void DME_error_callback(void);
extern void FE_error_callback(void);


#define is_it_HT() 	DMA1->HISR & ( 1 << 20)
#define is_it_FT() 	DMA1->HISR & ( 1 << 21)
#define is_it_TE() 	DMA1->HISR & ( 1 << 19)
#define is_it_FE() 	DMA1->HISR & ( 1 << 16)
#define is_it_DME() DMA1->HISR & ( 1 << 18)


void clear_exti_pending_bit(void)
{
	
	EXTI_TypeDef *pEXTI;
	pEXTI = EXTI;
	
	if((EXTI->PR & (1<<13)))
	{
		/*clears the pending bit*/
		EXTI->PR |=(1<<13);
	}
	
}


/****************************** IRQ Handlers******************************/
/*IRQ Handler for the Button */
void EXTI15_10_IRQHandler(void)
{
	USART_TypeDef *pUART2;
	pUART2 = USART2;
	
	/*We have to send the request for tranfer to initialize the transfer of data from
	*	Memory to UART when we press button, so once interrupt is generated it has to generate
	* the request for transfer from M2P i.e.we must enable UART-> CR3->DMAT has to be enabled
	*/
	
	pUART2->CR3 |=(1<<7);
	
	clear_exti_pending_bit();
	
	
}


/*IRQ handler for DMA1*/
void DMA1_Stream6_IRQHandler(void)
{
	/*
	* We have two registers which handle the interrupt status register, each one handling 4 streams
	* so we have 2*4=8 streams,We use stream 6 so we have to check for the High status register 
	* to know which event caused the interrupt.
	*/
	
	if( is_it_HT() )  								// Check for Half transfer
	{
		DMA1->HIFCR |= ( 1 << 20);			// Clear the interrupt flag
		HT_Complete_callback();
		
	}
	else if(is_it_FT() )						  // Check for Full transfer
	{
		DMA1->HIFCR |= ( 1 << 21);		 // Clear the interrupt flag
		FT_Complete_callback();
		
	}
	else if ( is_it_TE() )					 // Check for Tranfer error
	{
		DMA1->HIFCR |= ( 1 << 19);		 // Clear the interrupt flag
		TE_error_callback();
		
	}
	else if (is_it_FE() )						// check for FIFO Error
	{
		DMA1->HIFCR |= ( 1 << 16);		// Clear the interrupt flag
		FE_error_callback();
		
	}
	else if( is_it_DME() ) 					// Check for Direct memory Error
	{
		DMA1->HIFCR |= ( 1 << 18);		// Clear the interrupt flag
		DME_error_callback();
		
	}else{
		  ;
	}
	 
	
}


