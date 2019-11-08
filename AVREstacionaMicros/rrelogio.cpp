/*
 * rrelogio.c
 * gera uma interrupção a cada segundo (sem CTC)
 * outro comentario
 * Created: 31/10/2019 22:58:08
 * Author : oskar
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "classes.h"

DataHora relogio(1,1,6,0); // dia mes hora minuto
/*
char tempoMostraAndares=0;
char tempoCancela1Aberta=0;
char tempoCancela2Aberta=0;
*/

ISR(TIMER1_OVF_vect){
	TCNT1 = 3036; //1s até estourar
	// 65536 - 16M/256 = 3036
	relogio.incSeg();

/*	tempoMostraAndares=0;
	tempoCancela1Aberta++;
	tempoCancela2Aberta++;
*/
}


void timer1_config(){//chamar essa função antes do loop da main
	TIMSK1 = (1<<0); // habilita interrupção no timer 1
	sei();           // habilita interrupções no chip
	TCCR1A = 0;
	TCCR1B = 0x04;   //PRESCALER 256
}

