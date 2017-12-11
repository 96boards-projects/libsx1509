
/* libsx1509 library */

// -----------------------------------------------------------------------------
// init(uint8_t address, uint8_t bus): This function initializes the SX1509.
//  	It begins the mraa library, resets the IC, and tries to read some 
//  	registers to prove it's connected.
// Inputs:
//		- address: should be the 7-bit address of the SX1509. This should be  
//		 one of four values - 0x3E, 0x3F, 0x70, 0x71 - all depending on what the
//		 ADDR0 and ADDR1 pins ar se to. This variable is required.
//		- bus: This is the I2C bus ID. If working on a 96Boards Development
//		Platform, It should be either 0 or 1 for the Low Speed
// Output: Returns a 1 if communication is successful, 0 on error.
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// pinMode(uint8_t pin, uint8_t dir): This function sets one of the SX1509's 16 
//		outputs to either an INPUT, OUTPUT or an extender function there of.
//
//	Inputs:
//	 	- pin: should be a value between 0 and 15
//	 	- dir: The Arduino INPUT and OUTPUT constants should be used for the 
//		 dir parameter. They do what they say!
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// digitalWrite(uint8_t pin, uint8_t state): This function writes a pin to either high 
//		or low if it's configured as an OUTPUT. If the pin is configured as an 
//		INPUT, this method will activate either the PULL-UP	or PULL-DOWN
//		resistor (HIGH or LOW respectively).
//
//	Inputs:
//		- pin: The SX1509 pin number. Should be a value between 0 and 15.
//		- state: should be Arduino's defined HIGH or LOW constants.
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// digitalRead(uint8_t pin): This function reads the HIGH/LOW status of a pin.
//		The pin should be configured as an INPUT, using the pinDir function.
//
//	Inputs:
//	 	- pin: The SX1509 pin to be read. should be a value between 0 and 15.
//  Outputs:
//		This function returns a 1 if HIGH, 0 if LOW
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// analogWrite(uint8_t pin, uint8_t cycle):This function can be used to control the intensity 
//		of an output pin connected to an LED.
//
//	Inputs:
//		- pin: The SX1509 pin connecte to an LED.Should be 0-15.
//		- cycne: should be a 0-255 value setting the intensity of the LED
//			- 255 is completely off, 0 is 100% on.
//
//	Note: ledDriverInit should be called on the pin before calling this.
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// reset(): This fuction soft resets the SX1509. A software 
//		reset writes a 0x12 then 0x34 to the REG_RESET as outlined in the
//		datasheet.
// -----------------------------------------------------------------------------

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
	deviceBus = bus;
	deviceAddress = address;
	mraa::I2c i2c(deviceBus);
	//i2c(bus);
    	i2c.address(deviceAddress);
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
	else
		fprintf(stderr, "ya \n");
	
}

void SX1509::reset()
{
	mraa::I2c i2c(deviceBus);
	i2c.address(deviceAddress);
	i2c.writeReg(REG_RESET, 0x12);
	i2c.writeReg(REG_RESET, 0x34);
}

void SX1509::pinMode(uint8_t pin, uint8_t dir)
{
	mraa::I2c i2c(deviceBus);
	i2c.address(deviceAddress);
	//printf("%u", deviceBus);
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
	mraa::I2c i2c(deviceBus);
	i2c.address(deviceAddress);
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
	mraa::I2c i2c(deviceBus);
	i2c.address(deviceAddress);

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
	mraa::I2c i2c(deviceBus);
	i2c.address(deviceAddress);
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
	mraa::I2c i2c(deviceBus);
	i2c.address(deviceAddress);
	// Write the on intensity of pin
	i2c.writeReg(REG_I_ON[pin], cycle);
}
