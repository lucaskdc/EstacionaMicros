/*
 * rrelogio.c
 *
 * Created: 31/10/2019 22:58:08
 * Author : oskar
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

ISR(TIMER1_OVF_vect){
	TCNT1 = 3036; //1s at� estourar
	// 65536 - 16M/256 = 3036
}

DataHora relogio;

void timer1_config(){//chamar essa fun��o antes do loop da main
	TIMSK1 = (1<<0); // habilita interrup��o no timer 1
	sei();           // habilita interrup��es no chip
	TCCR1A = 0;
	TCCR1B = 0x04;   //PRESCALER 256
}

