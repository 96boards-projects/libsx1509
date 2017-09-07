#include <signal.h>
#include <stdio.h>
#include "sx1509_reg.h"
#include "sx1509.h"
#define ALL 0xFF
#define NONE 0x00

SX1509::SX1509()
{	
	uint8_t blank = 0;
}

uint8_t SX1509::init(uint8_t address, uint8_t bus)
{
	devbus = bus;
    	i2c.address(address);
	reset();
	i2c.writeReg(REG_MISC, 16);
	//00010000
	//LED Driver/PWM Mode set to Linear For Bank A and B
	//Change Bit 3 (For Bank A) and Bit 7 (For Bank B) to 1 for Logarithmic
	//ClkX = fOSC/(2^(RegMisc[6:4]-1))

	i2c.writeReg(REG_CLOCK, 64);
	//Set Oscillator frequency (fOSC) source to Internal 2MHz oscillator

	if (i2c.readReg(REG_INTERRUPT_MASK_A) != ALL)
	{
		fprintf(stderr, "FAILED to initialize SX1509! \n");
		exit(1);
	}
	
}


void SX1509::reset()
{
	i2c.writeReg(REG_RESET, 0x12);
	i2c.writeReg(REG_RESET, 0x34);
}

void SX1509::pinMode(uint8_t pin, uint8_t dir)
{
	if(pin > 7)
	{
		uint8_t pinb=pin-8;
		uint8_t RegDir = i2c.readReg(REG_DIR_B);	
		if ((dir == OUTPUT) || (dir == ANALOG_OUTPUT))
			RegDir &= ~(1<<pinb);
		else
			RegDir |= (1<<pinb);

		i2c.writeReg(REG_DIR_B, RegDir);
	}
	else
	{
		uint8_t RegDir = i2c.readReg(REG_DIR_A);	
		if ((dir == OUTPUT) || (dir == ANALOG_OUTPUT))
			RegDir &= ~(1<<pin);
		else
			RegDir |= (1<<pin);

		i2c.writeReg(REG_DIR_A, RegDir);
	}
	// If INPUT_PULLUP was called, set up the pullup too:
	if (dir == INPUT_PULLUP)
		digitalWrite(pin, HIGH);
	
	if (dir == ANALOG_OUTPUT)
	{
		ledDriverInit(pin);
	}
}

void SX1509::digitalWrite(uint8_t pin, uint8_t state)
{
	if(pin > 7)
	{
		uint8_t pinb = pin-8;
		uint8_t RegDir = i2c.readReg(REG_DIR_B);
	
		if ((ALL^RegDir)&(1<<pinb))	// If the pin is an output, write high/low
		{
			uint8_t RegData = i2c.readReg(REG_DATA_A);
			if (state)
				RegData |= (1<<pinb);
			else
				RegData &= ~(1<<pinb);
			i2c.writeReg(REG_DATA_B, RegData);
		}
		else	// Otherwise the pin is an input, pull-up/down
		{
			uint8_t PullUp = i2c.readReg(REG_PULL_UP_B);
			uint8_t PullDown = i2c.readReg(REG_PULL_DOWN_B);
			
			if (state)	// if HIGH, do pull-up, disable pull-down
			{
				PullUp |= (1<<pinb);
				PullDown &= ~(1<<pinb);
				i2c.writeReg(REG_PULL_UP_B, PullUp);
				i2c.writeReg(REG_PULL_DOWN_B, PullDown);
			}
			else	// If LOW do pull-down, disable pull-up
			{
				PullDown |= (1<<pinb);
				PullUp &= ~(1<<pinb);
				i2c.writeReg(REG_PULL_UP_B, PullUp);
				i2c.writeReg(REG_PULL_DOWN_B, PullDown);
			}
		}
	}

	else
	{
		uint8_t RegDir = i2c.readReg(REG_DIR_A);
	
		if ((ALL^RegDir)&(1<<pin))	// If the pin is an output, write high/low
		{
			uint8_t RegData = i2c.readReg(REG_DATA_A);
			if (state)
				RegData |= (1<<pin);
			else
				RegData &= ~(1<<pin);
			i2c.writeReg(REG_DATA_A, RegData);
		}
		else	// Otherwise the pin is an input, pull-up/down
		{
			uint8_t PullUp = i2c.readReg(REG_PULL_UP_A);
			uint8_t PullDown = i2c.readReg(REG_PULL_DOWN_A);
			
			if (state)	// if HIGH, do pull-up, disable pull-down
			{
				PullUp |= (1<<pin);
				PullDown &= ~(1<<pin);
				i2c.writeReg(REG_PULL_UP_A, PullUp);
				i2c.writeReg(REG_PULL_DOWN_A, PullDown);
			}
			else	// If LOW do pull-down, disable pull-up
			{
				PullDown |= (1<<pin);
				PullUp &= ~(1<<pin);
				i2c.writeReg(REG_PULL_UP_A, PullUp);
				i2c.writeReg(REG_PULL_DOWN_A, PullDown);
			}
		}
	}
}

uint8_t SX1509::digitalRead(uint8_t pin)
{

	if(pin > 7)
	{
		uint8_t pinb = pin-8;
		uint8_t RegDir = i2c.readReg(REG_DIR_B);
	
		if (RegDir & (1<<pinb))	// If the pin is an input
		{
			uint8_t RegData = i2c.readReg(REG_DATA_B);
			if (RegData & (1<<pinb))
				return 1;
		}
		
		return 0;
	}

	else
	{
		uint8_t RegDir = i2c.readReg(REG_DIR_A);
		
		if (RegDir & (1<<pin))	// If the pin is an input
		{
			uint8_t RegData = i2c.readReg(REG_DATA_A);
			if (RegData & (1<<pin))
				return 1;
		}
		
		return 0;
	}
}

void SX1509::ledDriverInit(uint8_t pin)
{

	if(pin > 7)
	{
		uint8_t pinb = pin-8;
		uint8_t temp;
	
		// Disable input buffer
		// Writing a 1 to the pin bit will disable that pins input buffer
		temp = i2c.readReg(REG_INPUT_DISABLE_B);
		temp |= (1<<pinb);
		i2c.writeReg(REG_INPUT_DISABLE_B, temp);
		
		// Disable pull-up
		// Writing a 0 to the pin bit will disable that pull-up resistor
		temp = i2c.readReg(REG_PULL_UP_B);
		temp &= ~(1<<pinb);
		i2c.writeReg(REG_PULL_UP_B, temp);
		
		// Set direction to output (REG_DIR_B)
		temp = i2c.readReg(REG_DIR_B);
		temp &= ~(1<<pinb); // 0=output	
		i2c.writeReg(REG_DIR_B, temp);
	
		
		// Enable LED driver operation (REG_LED_DRIVER_ENABLE)
		temp = i2c.readReg(REG_LED_DRIVER_ENABLE_B);
		temp |= (1<<pinb);
		i2c.writeReg(REG_LED_DRIVER_ENABLE_B, temp);
		
		// Set REG_DATA bit low ~ LED driver started
		temp = i2c.readReg(REG_DATA_B);
		temp &= ~(1<<pinb);
		i2c.writeReg(REG_DATA_B, temp);
	}

	else
	{

		uint8_t temp;
		
		// Disable input buffer
		// Writing a 1 to the pin bit will disable that pins input buffer
		temp = i2c.readReg(REG_INPUT_DISABLE_A);
		temp |= (1<<pin);
		i2c.writeReg(REG_INPUT_DISABLE_A, temp);
		
		// Disable pull-up
		// Writing a 0 to the pin bit will disable that pull-up resistor
		temp = i2c.readReg(REG_PULL_UP_A);
		temp &= ~(1<<pin);
		i2c.writeReg(REG_PULL_UP_A, temp);
		
		// Set direction to output (REG_DIR_A)
		temp = i2c.readReg(REG_DIR_A);
		temp &= ~(1<<pin); // 0=output	
		i2c.writeReg(REG_DIR_A, temp);
	
		
		// Enable LED driver operation (REG_LED_DRIVER_ENABLE)
		temp = i2c.readReg(REG_LED_DRIVER_ENABLE_A);
		temp |= (1<<pin);
		i2c.writeReg(REG_LED_DRIVER_ENABLE_A, temp);
		
		// Set REG_DATA bit low ~ LED driver started
		temp = i2c.readReg(REG_DATA_A);
		temp &= ~(1<<pin);
		i2c.writeReg(REG_DATA_A, temp);
	}
}

void SX1509::analogWrite(uint8_t pin, uint8_t cycle)
{
	// Write the on intensity of pin
	i2c.writeReg(REG_I_ON[pin], cycle);
}
