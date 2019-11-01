/*
 * CPPFile1.cpp
 *
 * Created: 29/10/2019 19:08:57
 *  Author: Jose Miola
 */ 
#include "teclado.h"
#include "lcd.h"

char le_linha(){ // retorna linha precionada, -1 se todas desativadas
	
	
	char valor_atual=-1;
	if(!(LINHA0_MASK & LINHA_PIN)) //verifica valor atual (0->BOTAO ATIVO)
		valor_atual=0;
	
	if(!(LINHA1_MASK & LINHA_PIN))
		valor_atual=1;
	
	if(!(LINHA2_MASK & LINHA_PIN))
	valor_atual=2;
	
	if(!(LINHA3_MASK & LINHA_PIN))
	valor_atual=3;
	
	return valor_atual;
	
	
	
	
}
void solta_tecla(char valor_anterior){
	char contador_2=0;
	char contador=0;
	char flag=1;
	while(flag){
		if(le_linha()!=valor_anterior)
			contador++;
		else
			contador=0;	
		if((contador>=(char)10)|(contador_2>=(char)100))
			flag=0;
		
		contador_2++;
		atrasoms(10);
	}
}
char debounce_linha(void){ //retorna linha pós debounce
	char contador=0;
	char valor_atual;
	char valor_anterior;
	
	valor_anterior=le_linha();

	if(valor_anterior!=(char)-1){
		do{
			valor_atual=le_linha();
			if(valor_atual!=valor_anterior)
				return -1;
			
			if(valor_atual==valor_anterior)
				contador++;								
			
			if(contador>=(char)10){
				
				solta_tecla(valor_atual); // fica preso na funçao até trocar de tecla ou solta-la				
				return valor_atual;
			}
			atrasoms(10);
			
			
		}while(1);
	}
	
	return -1;
	
}
char converte_leitura(char linha, char coluna){
	//converte linha coluna para o equivalente do teclado
	
	char leitura;
	switch(linha){
		
		case 0:
		switch(coluna){
			case 0:
			leitura='1';
			break;
			
			case 1:
			leitura='2';
			break;
			
			case 2:
			leitura='3';
			
			break;
			case 3:
			leitura='A';
			break;
		}
		
		break;
		case 1:
		switch(coluna){
			case 0:
			leitura='4';
			break;
			case 1:
			leitura='5';
			break;
			case 2:
			leitura='6';
			break;
			case 3:
			leitura='B';
			break;
		}
		
		break;
		case 2:
		switch(coluna){
			case 0:
			leitura='7';
			break;
			case 1:
			leitura='8';
			break;
			case 2:
			leitura='9';
			break;
			case 3:
			leitura='C';
			break;
		}
		break;
		case 3:
		switch(coluna){
			case 0:
			leitura='*';
			break;
			case 1:
			leitura='0';
			break;
			case 2:
			leitura='#';
			break;
			case 3:
			leitura='D';
			break;
		}
		break;
		
		
		
		
	}
	return leitura;
}
char le_teclado(){
	
	char leitura;
	char coluna;
	char linha;
	char i;
	LINHA_DDR  &=~((LINHA0_MASK)|(LINHA1_MASK)|(LINHA2_MASK)|(LINHA3_MASK)); // linhas como entrada
	LINHA_PORT |= (LINHA0_MASK)|(LINHA1_MASK)|(LINHA2_MASK)|(LINHA3_MASK); // pullup Linhas
	
	COLUNA_DDR |=((COLUNA0_MASK)|(COLUNA1_MASK)|(COLUNA2_MASK)|(COLUNA3_MASK));//coluas como saida
	
	

		coluna=0;
		COlUNA_PORT &=~ COLUNA0_MASK;
		COlUNA_PORT |= (COLUNA1_MASK|COLUNA2_MASK|COLUNA3_MASK);
		linha=debounce_linha();
		
		if(linha!=(char)-1){
			leitura = converte_leitura(linha,coluna);
			return leitura;
		}
		coluna=1;
		COlUNA_PORT &=~ COLUNA1_MASK;
		COlUNA_PORT |= (COLUNA0_MASK|COLUNA2_MASK|COLUNA3_MASK);
		linha=debounce_linha();
		
		if(linha!=(char)-1){
			leitura=converte_leitura(linha,coluna);
			return leitura;
		}
		coluna=2;
		COlUNA_PORT &=~ COLUNA2_MASK;
		COlUNA_PORT |= (COLUNA1_MASK|COLUNA0_MASK|COLUNA3_MASK);
		linha=debounce_linha();
		
		if(linha!=(char)-1){
			leitura = converte_leitura(linha,coluna);
			return leitura;
		}
		coluna=3;
		COlUNA_PORT &=~ COLUNA3_MASK;
		COlUNA_PORT |= (COLUNA1_MASK|COLUNA2_MASK|COLUNA0_MASK);
		linha = debounce_linha();
		
		if(linha!=(char)-1){
			leitura = converte_leitura(linha,coluna);
			return leitura;
		}
	return -1;
}
  




