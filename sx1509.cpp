/*
 * Title: SX1509 GPIO expander interfacing with 96Boards CE
 * Author: Sahaj Sarup
 * Copyright (c) 2017 Linaro Inc.
 * All rights reserved.
 *
 */

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
	SX1509 b;
	//b.init(0x3f, 0);
	signal(SIGINT, sig_handler);
	for(uint8_t i = 0; i<=3 && running==0; i++)
	{
		a.pinMode(i, OUTPUT);
		a.digitalWrite(i,LOW);
	}
	a.pinMode(8,INPUT_PULLUP);

	for(uint8_t i = 0; i<=3 && running==0; i++)
	{
		a.digitalWrite(i, HIGH);
		sleep(1);
		a.digitalWrite(i,LOW);
	}
	for(uint8_t i = 0; i<=3 && running==0; i++)
		a.pinMode(i, ANALOG_OUTPUT);
	uint8_t i=0;
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
