/*
 * atraso.cpp
 *
 * Created: 29/10/2019 13:03:32
 *  Author: Lucas
 */
/*
 atraso.cpp
 Funções de atraso para ATMEGA, usa Timer0 de 8 bits e Cristal de 16MHz
 Projetado originalmente para ser usada em Arduino Mega 2560
 By Lucas K Dal Castel
 2019
*/
#include <avr/io.h>
void atrasoms(unsigned int ms){
	/*	void atrasoms(int ms);
		aceita entrada de até 65535ms ou 65,535 segundos				
	*/
	TCNT0 = 0;
	TCCR0B = 0x03;	//00000011 CTC mode, prescaller x64 //já inicia contagem
	TCCR0A = 0x02;	//00000010 CTC mode
	OCR0A = 249;	//250 contagens * 64 prescaller / 16MHz = 1ms

	while(ms > 0){
		while(!(TIFR0 & (1<<1))); //congela timer enquanto
		TIFR0 |= 1<<1; //Limpa flag comparador A setando pra 1.
		ms--;
	}
}

void atrasous(unsigned int us){ //aproximado 0,2us a mais
	/*	void atrasous(int us);
		aceita entrada de até 65535us ou 65,535ms
	*/
	//16 contagens / 16MHz = 1 us
	TCNT0 = 0;
	TCCR0B = 0x01; //sem prescaller CTC //já inicia timer
	TCCR0A = 0x02; //CTC
	OCR0A = 15;

	while(us > 0){
		while(!(TIFR0 & (1<<1)));	
		TIFR0 |= 1<<1; //Limpa flag comparador A setando pra 1.
		us--;
	}
}