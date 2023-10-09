/*
* Main Header file for DMA transfer from Memory to peripheral
* Memory to UART
*/

#include "stm32f446xx.h"

/*Function prototypes of init and configuration functions*/

void button_init(void);
void uart2_init(void);
void dma1_init(void);
void send_some_data(void);
void enable_dma1_stream6(void);
void disable_dma1_stream6(void);
void dma1_interrupt_configuration(void);


/*Function prototypes of call backs*/
void HT_Complete_callback(void);
void FT_Complete_callback(void);
void TE_error_callback(void);
void DME_error_callback(void);
void FE_error_callback(void);
