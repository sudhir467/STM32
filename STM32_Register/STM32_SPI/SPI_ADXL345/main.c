/* This is Bare metal Firmware for SPI interfacing ADXL345
* Author: Sudhir Borra
*/

#include "RccConfig.h"
#include "Delay.h"


uint8_t data[3]={0x02,0x32,0xf6};
float xg, yg, zg;
int16_t x,y,z;
uint8_t RxData[6];

void SPIconfig(void)
{
	/**************STEPS TO FOLLOW**************
	1.Enable the SPI clock
	2. COnfigure the COntrol Register 1
	3. COnfigure the CR2
	*/
	RCC->APB2ENR |=(1<<12);				 //Enable the SPI Clock
	SPI1->CR1 |=(1<<0)|(1<<1); 	  	//CPOL=1, CPHA=1
	SPI1->CR1 |=(1<<2);					  // MAster mode
	SPI1->CR1 |=(3<<3);						 //BR[2:0]=011 as fpclk/16, PCLK2=80MHz, so SPI1 clk =5MHz
	SPI1->CR1 &=~(1<<7);					// MSB will be transmitted first
	SPI1->CR1 |=(1<<8)|(1<<9);    // SSM=1, SSI=1-> SOftware Slave mangement
	SPI1->CR1 &=~(1<<10);					// RXonly=0, Full duplex
	SPI1->CR1 &= ~(1<<11);				// DFF=0, 8 bit data
	SPI1->CR2=0;

}

void GPIOConfig(void)
{
	
	RCC->AHB1ENR |= (1<<0);    //Enable GPIO clock
	GPIOA->MODER |=(2<<10)|(2<<12)|(2<<14)|(1<<18); // Alternate Functions for PA5,PA6,PA7 and output for PA9
	GPIOA->OSPEEDR |= (3<<10)|(3<<12)|(3<<14)|(3<<18); // High speed for PA5, PA6, PA7, PA 9
	GPIOA->AFR[0] |=(5<<20)|(5<<24)|(5<<28); 		// AF5(SPI1) for PA5, PA6, PA7
	
}


void SPI_Enable(void)
{
	
	SPI1->CR1 |= (1<<6); // SPE=1, Peripheral Enabled
}

void SPI_Disable(void)
{
	SPI1->CR1 &=~(1<<6); 	// SPE=0, Peripheral Disabled
}


void CS_Enable(void)
{
	GPIOA->BSRR |=(1<<9)<<16;
}

void CS_Disable(void)
{
	GPIOA->BSRR |=(1<<9);
}

void SPI_Transmit( uint8_t *data, int size)
{
	/***************STEPS to follow******************
	1. Wait for the TXE bit to set in the status register
	2. Write the data to the data register
	3. AFter the data has been transmitted, wait for the BSY bit to reset in Status Register
	4. Clear the overrun flag by reading DR and SR
	**************************************************/
	int i=0;
	while(i<size)
	{
		while(!((SPI1->SR) & (1<<1))){}; // Wait for the TXE bit to set-> this will indicate that buffer is empty
			SPI1->DR= data[i];	// Load the data into data register
			i++;
		}
		/* During discontinuous communications, there is a 2 APB clock period delay between the write operation to the SPI_DR register
			and BSY bit setting. As a consequence it is mandatory to wait first until TXE is set and then until BSY is cleared after 
			writing the last data.
		*/
			while(!((SPI1->SR) & (1<<1))){};  // Wait for the TXE bit to set-> this will indicate that buffer is empty
			while (((SPI1->SR) & (1<<7))) {}; // Wait for BSY bit to reset-> this will indicate that SPI is not busy in communication
				
		
		// When we send some data in SPI it sends something in return so we have to clear that by dummy reading
		// Clear the overrun flag by reading DR and SR
				uint8_t temp= SPI1->DR;
								temp=SPI1->SR;
}

void SPI_Receive(uint8_t *data, int size)
{
	/*************Steps to follow*******************
	1. Wait for the BSY bit to reset in status register
	2. Send some dummy data before reading DATA
	3. Wait for the RXNE bit to set in status register
	4. read data from data register
	*/
	
	while(size)
	{
		while (((SPI1->SR)&(1<<7))) {};  // wait for BSY bit to Reset -> This will indicate that SPI is not busy in communication
			SPI1->DR=0; //send dummy data
			while(!((SPI1->SR) & (1<<0))) {}; // Wait for RXNE to set-> This will indicate that the Rx buffer is not empty
				*data++ = (SPI1->DR);
				size--;
	}
}

void adxl_write(uint8_t address, uint8_t value)
{
	uint8_t data[2];
	data[0]=address|0x40; // Multibyte write
	data[1]= value;
	CS_Enable ();  // pull the cs pin low
	SPI_Transmit (data, 2);  // write data to register
	CS_Disable ();  // pull the cs pin high
	
}

void adxl_read (uint8_t address)
{
	address |= 0x80;  // read operation
	address |= 0x40;  // multibyte read
	uint8_t rec;
	CS_Enable ();  // pull the pin low
	SPI_Transmit (&address, 1);  // send address
	SPI_Receive (RxData, 6);  // receive 6 bytes data
	CS_Disable (); // pull the pin high
}

void adxl_init (void)
{
	adxl_write (0x31, 0x01);  // data_format range= +- 4g
	adxl_write (0x2d, 0x00);  // reset all bits
	adxl_write (0x2d, 0x08);  // power_cntl measure and wake up 8hz
}
	
	
int main(void)
{
	SysClockConfig();
	GPIOConfig();
	SPIconfig();
	SPI_Enable();
	adxl_init();
	
	while(1)
	{
		adxl_read (0x32);		
		x = ((RxData[1]<<8)| RxData[0]);
		y = ((RxData[3]<<8)| RxData[2]);
		z = ((RxData[5]<<8)| RxData[4]);

	  xg = x *.0078;
    yg = y *.0078;
   	zg = z *.0078;
		
		Delay_ms (500);
		
	}
}