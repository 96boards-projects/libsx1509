#include <stdint.h>
#include "mraa.hpp"
#ifndef SX1509_H
#define SX1509_H
#define OUTPUT 1
#define ANALOG_OUTPUT 2
#define INPUT 3
#define INPUT_PULLUP 4
#define HIGH 1
#define LOW 0
class SX1509
{
	private:
		
		uint8_t deviceBus, deviceAddress;
		
		//mraa::I2c i2c(uint8_t bus);
		//mraa::I2c i2c.address(uint8_t address);	
	public:

	// -----------------------------------------------------------------------------
	// Constructor - SX1509:
	// -----------------------------------------------------------------------------
		SX1509();

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
		uint8_t init(uint8_t address, uint8_t bus);
	// -----------------------------------------------------------------------------
	// reset(): This fuction soft resets the SX1509. A software 
	//		reset writes a 0x12 then 0x34 to the REG_RESET as outlined in the
	//		datasheet.
	// -----------------------------------------------------------------------------
		void reset();

	// -----------------------------------------------------------------------------
	// pinMode(uint8_t pin, uint8_t dir): This function sets one of the SX1509's 16 
	//		outputs to either an INPUT, OUTPUT or an extender function there of.
	//
	//	Inputs:
	//	 	- pin: should be a value between 0 and 15
	//	 	- dir: The Arduino INPUT and OUTPUT constants should be used for the 
	//		 dir parameter. They do what they say!
	// -----------------------------------------------------------------------------
		void pinMode(uint8_t pin, uint8_t dir);

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
		void digitalWrite(uint8_t pin, uint8_t state);

	// -----------------------------------------------------------------------------
	// digitalRead(uint8_t pin): This function reads the HIGH/LOW status of a pin.
	//		The pin should be configured as an INPUT, using the pinDir function.
	//
	//	Inputs:
	//	 	- pin: The SX1509 pin to be read. should be a value between 0 and 15.
	//  Outputs:
	//		This function returns a 1 if HIGH, 0 if LOW
	// -----------------------------------------------------------------------------
		uint8_t digitalRead(uint8_t pin);

		void ledDriverInit(uint8_t pin);

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
		void analogWrite(uint8_t pin, uint8_t cycle);
		
};

typedef SX1509 sx1509Class;
#endif
