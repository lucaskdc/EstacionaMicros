/*
 serial.cpp
 BAUDRATE = 19200
 8 - N - 1
 Fclk = 16MHZ
 by Lucas K Dal Castel
 2019
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include "serial.h"
#include "atraso.h"
char serialTXBuffer[TX_BUFFER_SIZE];
char serialTXPosW = 0;
char serialTXPosR = 0;

char serialRXBuffer[RX_BUFFER_SIZE];
char serialRXPosW = 0;
char serialRXPosR = 0;

void serialSetup(){
	UBRR0L = 51; //registrador de 16 bits divisor clock
	UBRR0H = 0;
	
	UCSR0A = 0;
	UCSR0B = (1<<RXEN0) | (1<<TXEN0) | /*(1<<TXCIE0)*/ (1<<UDRIE0) | (1<<RXCIE0); //ativa rx e tx e ISR de tx e rx
	sei();
	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00); //Async, 0 parity, 1 stop bit, 8 bits, rising edge clock
}

/*char le(){
	while(!UCSR0A & (1<<RXC0));
	return UDR0;
}*/
void leVetor(char vetor[], int n){
	for(int i=0; i<n; i++){
		vetor[i] = le();
	}
}
/*void escreve(char dado){
	while(!UCSR0A & (1<<UDRE0));
	UDR0 = dado;
}*/
void escreveVetor(char dados[], int n){
	for(int i=0; i<n; i++){
		escreve(dados[i]);
	}
	//UDR0 = '\0';
}
char novoDado(){
	char tam = serialRXPosW-serialRXPosR;
	if(tam<0)
		tam += RX_BUFFER_SIZE;
	return tam;
}

char le(){
	char c;
	if(serialRXPosR != serialRXPosW){
		c = serialRXBuffer[serialRXPosR];
		serialRXPosR++;
		if(serialRXPosR >= RX_BUFFER_SIZE)
			serialRXPosR = 0;
	}else{
		atrasoms(1);
		if(serialRXPosR != serialRXPosW){
			c = serialRXBuffer[serialRXPosR];
			serialRXPosR++;
			if(serialRXPosR >= RX_BUFFER_SIZE)
			serialRXPosR = 0;
		}else{
			c = '\0';
		}
	}
	return c;
}
void escreve(char c){
	serialTXBuffer[serialTXPosW] = c;
	serialTXPosW++;
	if(serialTXPosW >= TX_BUFFER_SIZE)
		serialTXPosW = 0;
		
	//UDR0 = '\0';
	if( UCSR0A & (1<<UDRE0) ){
		UCSR0A &= ~(1<<UDRE0);
		UCSR0A |= (1<<UDRE0);
	}
}

ISR(USART0_UDRE_vect){
	if(serialTXPosR != serialTXPosW){
		UDR0 = serialTXBuffer[serialTXPosR];
		serialTXPosR++;
		if(serialTXPosR >= TX_BUFFER_SIZE){
			serialTXPosR = 0;
		}
	}
}

ISR(USART0_RX_vect){
	serialRXBuffer[serialRXPosW] = UDR0;
	serialRXPosW++;
	if(serialRXPosW >= RX_BUFFER_SIZE){
		serialRXPosW = 0;
	}
}
