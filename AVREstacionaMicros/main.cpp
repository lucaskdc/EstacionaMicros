/*
 * AVREstacionaMicros.cpp
 *
 * Created: 29/10/2019 10:36:24
 * Author : Lucas
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#include "atraso.h"
#include "serial.h"
#include "classes.h"

void bloqueia();
void desbloqueia();
void cancelaAbre(char entradaSaida);

char desbloqueado=0;
char cancelaAberta=0;

DataHora tempoAtual;

Veiculo carros[256];


int main(void)
{
    /* Replace with your application code */
	char byteLido;
	char vetor[64];

	
	DDRB = 1<<7;
	
	serialSetup();

    while (1) 
    {
		PORTB ^= 1<<7;
		if(novoDado()){ //loop padrão
			byteLido = le();
			if(byteLido == 'S'){
				byteLido = le();
				switch (byteLido)
				{
					case 'B': //Bloqueia
						bloqueia();
						escreveVetor("EB",2);
						break;
					case 'D': //Desbloqueia
						desbloqueia();
						escreveVetor("ED", 2);
						break;
					case 'H': //Recebe DataHora
						leVetor(vetor, 4);
						tempoAtual.setByVector(vetor);
						break;
					case 'N': //Novo Carro
						byteLido=le(); //1 entrada, 2 saída
						leVetor(vetor, 8); //Lê placa 7char+\0
						escreveVetor("EN", 2); //Resposta para Servidor
						if(desbloqueado)
							cancelaAbre(byteLido);
						break;
						
				}			
			}
		}
		escreveVetor("EO", 2);
		atrasoms(100);
    }
}

void bloqueia(){
	//DESLIGADO NA TELA
	desbloqueado = 0;
}

void desbloqueia(){

	desbloqueado = 1;
}

void cancelaAbre(char entradaSaida){
	char vetorSerial[2];
	escreveVetor("EA", 2);
	escreve(entradaSaida);
	leVetor(vetorSerial, 2);
}

void cancelaFecha(char entradaSaida){
	char vetorSerial[2];
	escreveVetor("EF", 2);
	escreve(entradaSaida);
	leVetor(vetorSerial, 2);
}


char *enviaCartao(char vetorDados[]){
	char resposta[256];
	escreveVetor("EC", 2);
	escreve(7);
	vetorDados[6]='\0';
	escreveVetor(vetorDados, 7);
	leVetor(resposta, 3);
	leVetor(resposta, resposta[2]);
	return resposta;
}