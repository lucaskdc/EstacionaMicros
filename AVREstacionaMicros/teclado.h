/*
 * IncFile1.h
 *
 * Created: 29/10/2019 19:09:31
 *  Author: User
 */ 


#ifndef TECLADO_H_
#define TECLADO_H_

	#include <avr/io.h>
	#include <avr/io.h>
	#include <avr/interrupt.h>

	#include "atraso.h"
	#define COLUNA0_MASK (1<<PB0) //digital pin 53
	#define COLUNA1_MASK (1<<PB1) //digital pin 52
	#define COLUNA2_MASK (1<<PB2)//digital pin 51
	#define COLUNA3_MASK (1<<PB3)//digital pin 50
	#define COlUNA_PORT PORTB // valor de saida ou setar pull up
	#define COLUNA_DDR DDRB //definir se é entrada ou saida

	#define LINHA0_MASK (1<<PH3) // digital pin 6
	#define LINHA1_MASK (1<<PH4) // digital pin 7
	#define LINHA2_MASK (1<<PH5) // digital pin 8
	#define LINHA3_MASK (1<<PH6) // digital pin 9
	#define LINHA_PIN  PINH      // valor de entrada
	#define LINHA_PORT PORTH    // valor de saida ou setar pull up
	#define LINHA_DDR  DDRH		//definir se é entrada ou saida
	
	char le_linha();
	void solta_tecla(char valor_anterior);
	char debounce_linha(void);
	char converte_leitura(char linha, char coluna);
	char le_teclado();
	




#endif /* TECLADO_H_ */