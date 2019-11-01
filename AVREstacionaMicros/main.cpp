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
char estado=0, estadoAnterior=-1;
DataHora tempoAtual;

Veiculo carros[120]; //máximo 120 carros no estacionamento


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
						break; //B
					case 'D': //Desbloqueia
						desbloqueia();
						escreveVetor("ED", 2);
						break; //D
					case 'H': //Recebe DataHora
						leVetor(vetor, 4);
						tempoAtual.setByVector(vetor);
						break; //H
					case 'N': //Novo Carro
						byteLido=le(); //1 entrada, 2 saída
						leVetor(vetor, 8); //Lê placa 7char+\0
						escreveVetor("EN", 2); //Resposta para Servidor
						switch(byteLido){
							case '1':
								carros[posCarro].estado = NAENTRADA;
								if(desbloqueado)
									cancelaAbre(byteLido);
								else{
									LCD::limpa();
									LCD::print("Bloqueado");
								}
								
								break;
							case '2':
								carros[posCarro].estado = NASAIDA;
								if(desbloqueado){
									if(carros[posCarro].dataSaidaPaga.diffMin(tempoAtual) > 0){
										LCD::limpa();
										LCD::print("Realize o Pagamento");
									}else{
										cancelaAbre(byteLido);
									}
								}else{
									LCD::limpa();
									LCD::print("Bloqueado");
								}
								break;
						}
						break; //N
					case 'S': //Carro saindo
						byteLido = le(); //1 entrada, 2 saída
						leVetor(vetor,8);
						escreveVetor("ES", 2);
						char posCarro = procuraPlaca(vetor);
						if(posCarro == -1){
							posCarro = procuraPlaca("AAA0000");
						}
						switch(byteLido){
							case '1':
								carros[posCarro].estado = DENTRO;
								break;
							case '2':
								carros[posCarro].estado = FORA;
								break;
						}
						
						cancelaFecha(byteLido);
						break; //S
				}//switch
			}//if bytelido == 'S'
		}// if novoDado
		
		switch(estado){
			case ESTADOINICIAL:
				break;
			case 
		}
		
		escreveVetor("EO", 2);
		atrasoms(100);
    }
}

void bloqueia(){
	//DESLIGADO NA TELA
	//16 - 9 = 7 -> esceve na coluna 4
	LCD::limpa();
	LCD::printPos("Desligado", 1, 4);
	cancelaFecha(1);
	cancelaFecha(2);
	desbloqueado = 0;
}

void desbloqueia(){
	desbloqueado = 1;
}

void cancelaAbre(char entradaSaida){ //'E','A',n
	escreveVetor("EA", 2);
	escreve(entradaSaida);
	atrasoms(2);
	verificaResposta("SA", 2, "Erro Abertura Cancela");
}

void cancelaFecha(char entradaSaida){ //'E','F',n
	char vetorSerial[2];
	escreveVetor("EF", 2);
	escreve(entradaSaida);
	atrasoms(2);
	verificaResposta("SF", 2, "Erro Fechamento Cancela");
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

void erro(char vetor[]){ //Imprime erro e dá pausa de 1s.
	LCD::limpa();
	LCD::print(vetor);
	atrasoms(1000);
}

void verificaResposta(char respostaEsperada[], char n, char mensagem[]){
	for(int i=0; i<n; i++){
		if(le() != respostaEsperada){
			erro(mensagem);
		}
	}
}

char procuraPlaca(char placaProcurada[]){
	for(int i=0; i<120; i++){
		if(carros->ehPlacaIgual(placaProcurada))
			return i;
	}
	return -1;
}