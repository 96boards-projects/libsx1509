//-----------USEAGE-------------------------------------------------------------

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

//---------EXAMPLE--------------------------------------------------------------
#include "sx1509.h"
#include <time.h>
#include <stdio.h>
#include <signal.h>

int running = 0;
void
sig_handler(int signo)
{
	if (signo == SIGINT)
	{
		printf("closing IO nicely\n");

        	running = -1;
	}
}

int main()
{
	SX1509 a;
	a.init(0x3e, 0);
	
	for(uint8_t i = 0; i<=3; i++)
	{
		a.pinMode(i, OUTPUT);
		a.digitalWrite(i,LOW);
	}
	a.pinMode(8,INPUT_PULLUP);

	for(uint8_t i = 0; i<=3; i++)
	{
		a.digitalWrite(i, HIGH);
		sleep(1);
		a.digitalWrite(i,LOW);
	}
	for(uint8_t i = 0; i<=3; i++)
		a.pinMode(i, ANALOG_OUTPUT);
	uint8_t i=0;
	signal(SIGINT, sig_handler);
	while(1>0 && running==0)
	{
		printf("%u\n",a.digitalRead(8));
		uint8_t k = 255;
		while(a.digitalRead(8)==1 && k>=0 && running==0)
		{
			if(k==0)
			{
				k=255;
				a.analogWrite(i,k);
				i++;
				if(i>3)
					i=0;
			}
			
			a.analogWrite(i,k);
			k--;
			nanosleep((const struct timespec[]){{0, 20000000L}}, NULL);
		}
		if(k!=255)
		{
			a.analogWrite(i,255);
			i++;
			if(i>3)
				i=0;
		}
		nanosleep((const struct timespec[]){{0, 20000000L}}, NULL);
	}
	a.reset();
}
