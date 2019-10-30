/*
 serial.cpp
 BAUDRATE = 19200
 8 - N - 1
 Fclk = 16MHZ
 by Lucas K Dal Castel
 2019
*/
#include <avr/io.h>

void serialSetup(){
	UBRR0L = 51; //registrador de 16 bits divisor clock
	UBRR0H = 0;
	
	UCSR0A = 0;
	UCSR0B = (1<<RXEN0) | (1<<TXEN0); //ativa rx e tx
	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00); //Async, 0 parity, 1 stop bit, 8 bits, rising edge clock
}

char le(){
	while(!UCSR0A & (1<<RXC0));
	return UDR0;
}
void leVetor(char vetor[], int n){
	for(int i=0; i<n; i++){
		vetor[i] = le();
	}
}
void escreve(char dado){
	while(!UCSR0A & (1<<UDRE0));
	UDR0 = dado;
}
void escreveVetor(char dados[], int n){
	for(int i=0; i<n; i++){
		escreve(dados[i]);
	}
}
char novoDado(){
	return UCSR0A & (1<<RXC0);
}