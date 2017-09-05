#include "sx1509.h"
#include "mraa.hpp"

int main()
{
	
	SX1509 a;
	a.init(0x3e, 0);
	a.pinMode(1,OUTPUT);
	a.pinMode(9,INPUT);
	a.pinMode(15,ANALOG_OUTPUT);
	a.digitalWrite(1,HIGH);
	sleep(1);
	a.digitalWrite(1,LOW);
	uint8_t out = a.digitalRead(9);
	printf("%u \n",out);
	uint8_t k = 0;
	while(k!=255)
	{
		a.analogWrite(15,k);
		k++;
		nanosleep((const struct timespec[]){{0, 50000000L}}, NULL);
		//printf("%u\n",k);
	}
	a.reset();
}
