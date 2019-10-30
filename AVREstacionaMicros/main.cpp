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
class DataHora{
	public:
		char dia;
		char mes;
		char hora;
		char min;
		DataHora(char _dia=1, char _mes=1, char _hora=0, char _min=0){
			dia =_dia;
			mes = _mes;
			hora = _hora;
			min = _min;			
		}
		void setByVector(char datahoravetor[4]){
			dia = datahoravetor[0];
			mes = datahoravetor[1];
			hora  = datahoravetor[2];
			min = datahoravetor[3];
		}
		DataHora somaMin(int minutos){
			DataHora retorno(dia, mes, hora, min);
			retorno.hora += minutos/60;
			retorno.min += minutos%60;
			
			retorno.dia += retorno.hora/24;
			retorno.hora = retorno.hora%24;
			
			if(retorno.min<0){
				retorno.min += 60;
				retorno.hora += 1;
			}
			if(retorno.hora<0){
				retorno.hora +=24;
				retorno.dia--;
			}
			if(retorno.dia<0){
				retorno.dia +=30;
				retorno.mes--;
			}
			return retorno;
		}
		unsigned long int diffMin(DataHora comp){
			unsigned long int minutosTotal = (comp.dia - dia)*60*24; //Dias
			minutosTotal += (comp.mes - mes)*24*60*30; //Meses de 30 dias
			minutosTotal += (comp.hora - hora)*60; //Horas
			minutosTotal += (comp.min - min); //Minutos
			return minutosTotal;
		}
};
class Veiculo {       // The class
	public:             // Access specifier
		char placa[8];
		DataHora dataEntrada;
		DataHora dataPagamento;
		DataHora dataSaidaPaga;
		
		Veiculo(char _placa[7]="AAA0000", DataHora _dataEntrada = DataHora()){
			for(int i=0; i<7; i++){
				placa[i] = _placa[i];
			}
			placa[7] = '\0';
			dataEntrada = _dataEntrada;
			dataPagamento = _dataEntrada;
			calculaSaidaPaga();		
		}
		
		void pagou(DataHora _dataPagamento){
			dataPagamento = _dataPagamento;
			calculaSaidaPaga();
		}
	private:
		void calculaSaidaPaga(){
			unsigned long int minutosPagamento = dataEntrada.diffMin(dataPagamento);
			if(minutosPagamento<=45){
				dataSaidaPaga = dataEntrada.somaMin(60);
			}else if(minutosPagamento%30){
				dataSaidaPaga = dataEntrada.somaMin(minutosPagamento-minutosPagamento%60+30);
			}else{
				dataSaidaPaga = dataEntrada.somaMin(minutosPagamento);
			}
			if(dataSaidaPaga.diffMin(dataPagamento)<15){
				dataSaidaPaga = dataPagamento.somaMin(15);
			}
		}
};

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