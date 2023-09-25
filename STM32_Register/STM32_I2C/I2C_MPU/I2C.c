/* This is I2C firmware for STM32F446RE in master mode 
*  Author: Sudhir Borra
*/

#include "RccConfig.h"
#include "Delay.h"
#include "I2C.h"

void I2C_Config(void)
{
	/************Steps to be followed*****************
	1. Enable the I2C clock and GPIO clock
	2. Configure the I2C PINS for alternate functions
		a. Select the Alternate function in MODER register
		b. Select the open drain output
		c. Select High speed for the pins
		d. Select the pull up for both the pins
		e. Configure the Alternate function in AFR register
	3. Reset the I2C
	4. Program the peripheral input clock in the I2C_CR2 register in order to generate correct timings
	5. COnfigure the Clock control registers
	6. COnfigure the rise time register
	7.Program the I2C_CR1 register to enable the peripheral
	*/
	
	/*Enable the I2C clock and GPIO clock*/
	RCC->APB1ENR |=(1<<21); //Enable I2C clock
	RCC->AHB1ENR |=(1<<1);	//Enable GPIOB clock
	
	//Configure the I2C PINS for alternate functions
	GPIOB->MODER |= (2<<16) |(2<<18); // For PB8 and PB9 - alternate function is 10 so we write 2 in 16 and 18 bit
	GPIOB->OTYPER |=(1<<8)|(1<<9);		// Set the output as open drain
	GPIOB->OSPEEDR |=(3<<16) | (3<<18); // High speed(11 so 3) for PB8 and PB9
	GPIOB->PUPDR |=(1<<16) |(1<<18); //Set pull up setting for PB8 and PB9
	GPIOB->AFR[1] |=(4<<0) | (4<<4);  // Set the alternate Function high register because it 8 anf 9 pins
	
	// Reset the I2C
	I2C1->CR1 |=(1<<15);
	I2C1->CR1 &= ~(1<<15);
	
	// Program the peripheral input clock in I2C_CR2 register in order to generate correct timings
	
	I2C1->CR2 |=(45 <<0); // Its connected to APBCLK1 Frequency max is 45Mhz
	
	// COnfigure the Clock control registers
	
	I2C1->CCR= (225<<0); // The value is set based on the calculation
	
	//COnfigure the rise time register
	I2C1->TRISE=46;	// Calculation based on data sheet
	
	//Program the I2C_CR1 register to enable the peripheral
	I2C1->CR1 |=(1<<0); //Enable I2C
		
}

void I2C_Start(void)
{
	/**************** Steps Followed**********
	1. Send the START condition
	2. Wait for the SB(Bit 0 in SR1) to set. This indicated that the start condition is generated
	*/
	I2C1->CR1 |=(1<<10); //Enable the ACK
	I2C1->CR1 |=(1<<8); // Generate START
	while(I2C1->SR1 & (1<<0)); //Wait for SB bit to set 
}

void I2C_Write(uint8_t data)
{
	/*************Steps followed*******
	1.Wait for the TXE(bit 7 in SR1) to set. This indicated that the DR is empty
	2.Send the data to DR register
	3. Wait for the BTF(bit 2 in SR1) to set. This indicated the end of LAST data transmission
	*/
	while(!(I2C1->SR1 & (1<<7))); // Wait for TXE bit to set
	I2C1->DR =data;
	while(!(I2C1->SR1 & (1<<2))); // wait for the BTF(byte transfer finished to set
	
}

void I2C_Address (uint8_t Address)
{
	/************** Steps followed****************
	1. Send the salve address to the DR register
	2. Wait for the ADDR(bit 1 in SR1) to set. This indicated the end of Address transmission
	3. Clear the ADDR by reading the SR1 and SR2
	*/
	
	I2C1->DR= Address; //Send the address
	while(!(I2C1->SR1 & (1<<1))); // Wait for the ADDR bit to set
	uint8_t temp = I2C1->SR1 | I2C1->SR2; // read SR1 and SR2 to clear the ADDR bit
}  

void I2C_Stop(void)
{
	I2C1->CR1 |=(1<<9); //Stop I2C condition
	
}

void I2C_WriteMulti (uint8_t *data, uint8_t size)
{
	/*****************Steps followed*******
	1. Wait for the TXE(bit 7 in SR1) to set. This indicated that the DR is empty
	2. Keep sending DATA to the DR register after performing the check if TXE bit is set
	3. Once the data is sent, Wait for the BTF(bit 2 in SR1) to set. This indicated the end of LAST data transmission
	*/
	while(!(I2C1->SR1 & (1<<7))); // Wait for TXE bit to set
	
	while(size)
	{
	  while(!(I2C1->SR1 & (1<<2))); // wait for the BTF(byte transfer finished to set
	  I2C1->DR =(volatile uint32_t)* data++; //send data
		size--;
	}
	
	while(!(I2C1->SR1 & (1<<2))); // Wait for BTF to set
}

void I2C_Read (uint8_t Address, uint8_t *buffer, uint8_t size)
{
	/***********Steps followed************
	1. If only 1 Byte needs to be read
		a. Write the slave address and wait for the ADDR bit(bit 1 in SR1) to be set
		b. The ack disable is made during EV6(before ADDR flag is cleared) and STOP condition generation is made after EV6
		c. Wait for the RXNE(recieve buffer not empty) bit to set
		d. Read the data from the DR
	2. If Multiple BYTEs needs to be read
		a. Write slave address, wait for the ADDR bit(bit 1 in SR1) to be set
		b. Clear the ADDR bit by reading the SR1 and SR2 registers
		c. Wait for the RXNE bit to set
		d. Read the data from the DR
		e. Generate the ack by setting the ACK(bit 10 in SR1)
		f. To generate the NACK pulse after the last recieved data byte, the ACK bit must be cleared just after reading the second last data byte
		e. To generate the stop/restart condition, software myst set the STOP/START bit after reading the second last data byte
			(after the second last RxNE event
	*/
	
	int remaining =size;
	if(size == 1)
	{
		I2C1->DR = Address; //send the address
		while(!(I2C1->SR1 & (1<<1))); //wait for the ADDR bit to set
		
		I2C1->CR1 &=~(1<<10); //clear the ACK bit
		uint8_t temp =I2C1->SR1 | I2C1->SR2; //read the SR1 and SR2 to clear ADDR bit
		I2C1->CR1 |= (1<<9); //Generate the STOP
		
		while(!(I2C1->SR1 &(1<<6))); //wait for RxNE to set
		
		buffer[size-remaining]=I2C1->DR; // Read the data from the DATA register
			
	}
	/*Condition to read multiple bytes*/
	else
	{
		I2C1->DR=Address; //send the address
		while(!(I2C1->SR1 & (1<<1))); //wait for the ADDR bit to set
		
		uint8_t temp=I2C1->SR1 |I2C1->SR2; //read SR1 and Sr2 to clear the ADDR bit
		
		while(remaining >2)
		{
			while(!(I2C1->SR1 & (1<<6))); //Wait for RxNe to set
			
			buffer[size-remaining]= I2C1->DR; //COpy the data into the buffer
			
			I2C1->CR1 |= 1<<10;    //set the ACk bit to acknowledge the data received
			
			remaining--;
		}
		
		/* Read the second last byte*/
		while(!(I2C1->SR1 & (1<<6))); //wait for RXNE to set
		buffer[size-remaining]= I2C1->DR;
		
		I2C1->CR1 &=~(1<<10); //clear the ACK bit
		
		I2C1->CR1 |=(1<<9); //stop I2C
		
		remaining--;
		
		/*Read the last byte*/
		
		while(!(I2C1->SR1 &(1<<6))); //Wait for RxNE to set
		buffer[size-remaining]= I2C1->DR; // COpy the data into buffer
			
}

}
