/*
 * AVREstacionaMicros.cpp
 *
 * Created: 29/10/2019 10:36:24
 * Author : Lucas
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#include "atraso.h"


int main(void)
{
    /* Replace with your application code */
	DDRB = 1<<7;
    while (1) 
    {
		PORTB ^= 1<<7;
		atrasoms(500);
    }
}

