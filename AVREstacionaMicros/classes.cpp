/*
 * classes.cpp
 *
 * Created: 30/10/2019 18:56:47
 *  Author: Lucas
 */ 

#include "classes.h"

DataHora::DataHora(char _dia, char _mes, char _hora, char _min){
		dia =_dia;
		mes = _mes;
		hora = _hora;
		min = _min;
		seg = 0;
}
void DataHora::setByVector(char datahoravetor[4]){
	this->dia = datahoravetor[0];
	this->mes = datahoravetor[1];
	this->hora  = datahoravetor[2];
	this->min = datahoravetor[3];
}
DataHora DataHora::somaMin(int minutos){
	DataHora retorno(this->dia, this->mes, this->hora, this->min);
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
unsigned long int DataHora::diffMin(DataHora comp){
	unsigned long int minutosTotal = (comp.dia - this->dia)*60*24; //Dias
	minutosTotal += (comp.mes - this->mes)*24*60*30; //Meses de 30 dias
	minutosTotal += (comp.hora - this->hora)*60; //Horas
	minutosTotal += (comp.min - this->min); //Minutos
	return minutosTotal;
}

void DataHora::incSeg(char segundos){
	this->seg += segundos;
	if(this->seg >= 60){
		this->min += this->seg/60;
		this->seg %= 60;
	}
	if(this->min >= 60){
		this->hora += this->min/60;
		this->min %= 60;
	}
	if(this->hora >= 24){
		this->dia += this->hora/24;
		this->hora %= 24;
	}
	if(this->dia >= 30){
		this->mes += dia/30;
		this->dia %= 30;
	}
}

Veiculo::Veiculo(char _placa[7], DataHora _dataEntrada){
	for(int i=0; i<7; i++){
		this->placa[i] = _placa[i];
	}
	this->placa[7] = '\0';
	this->dataEntrada = _dataEntrada;
	this->dataPagamento = _dataEntrada;
	calculaSaidaPaga();
}
	
void Veiculo::pagou(DataHora _dataPagamento){
	dataPagamento = _dataPagamento;
	calculaSaidaPaga();
}

void Veiculo::calculaSaidaPaga(){
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

char Veiculo::ehEspecial(){
	//retorna 1 se placa começa com IDE e 0 caso contrário
	if(	this->placa[0] != 'I')
		return 0;
	if( this->placa[1] != 'D')
		return 0;
	if(this->placa[2] != 'E')
		return 0;
	return 1;
}

char Veiculo::ehPlacaIgual(char compara[]){ //retorna 1 se placa for igual ao parâmetro compara
	for(int i=0; i<8; i++){
		if(compara[i] != this->placa[i])
			return 0;
	}
	return 1;
}