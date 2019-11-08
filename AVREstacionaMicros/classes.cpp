/*
 * classes.cpp
 *
 * Created: 30/10/2019 18:56:47
 *  Author: Lucas
 */ 

#include "classes.h"
#include "serial.h"
#define debugMin 1
#define debugPagar 1
#define debugSec 0
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
void DataHora::setByDataHoraZeroSeconds(DataHora min){
	this->dia = min.dia;
	this->mes = min.mes;
	this->hora = min.hora;
	this->min = min.min;
}
void DataHora::setByDataHora(DataHora a){
	this->dia = a.dia;
	this->mes = a.mes;
	this->hora = a.hora;
	this->min = a.min;
	this->seg = a.seg;
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
int DataHora::diffMin(DataHora comp){
	//int minutosTotal = (comp.dia - this->dia)*60*24; //Dias
	//minutosTotal += (comp.mes - this->mes)*24*60*30; //Meses de 30 dias
	int minutosTotal = (comp.hora - this->hora)*60; //Horas
	minutosTotal += (comp.min - this->min); //Minutos
	#if debugMin == 1
		escreveVetor("diffMin",8);
		//escreve(minutosTotal>>8);
		escreve((char)minutosTotal);
		escreve('\n');
	#endif
	return minutosTotal;
}
int DataHora::diffSec(DataHora comp){
	int segundos = (comp.dia - this->dia)*60*24*60; //Dias
	segundos += (comp.mes - this->mes)*24*60*30*60; //Meses de 30 dias
	segundos += (comp.hora - this->hora)*60*60; //Horas
	segundos += (comp.min - this->min)*60; //Minutos
	segundos += (comp.seg - this->seg);
	#if debugSec == 1
		escreveVetor("diffMin",8);
		escreve(segundos>>24);
		escreve(segundos>>16);
		escreve(segundos>>8);
		escreve(segundos);
		escreve('\n');
	#endif
	return segundos;
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

Veiculo::Veiculo(DataHora _dataEntrada){
	this->valorpago=0;
	this->estEspecial = 0;
	this->estEspecialAntes=0;
	this->estado=FORA;
	for(int i=0; i<7; i++){
		this->placa[i] = '#';
	}
	this->placa[7] = '\0';
	this->dataEntrada = _dataEntrada;
}

int Veiculo::calculaPgto(DataHora agora){//valor a pagar
	//se retornar 0, libera saída, caso contrário cobra pagamento
	int minutos = this->dataEntrada.diffMin(agora);
	//int minutos = 70;
	//int tmp;
	int pagar=0;

	if(minutos<=15){		
		pagar = 0;
	}else if(minutos<=60){
		pagar = 10-this->valorpago;
	}else{
		//if(minutos%30){//se n divide por 30
		//	tmp = (minutos/30+1)*30; //arredonda para cima
		//	pagar = 10+((tmp-60)/30)*4-this->valorpago;
		//}else{
			pagar = (10+((minutos-60)/30+1)*4) - this->valorpago; //0 a 29 e 30 a 59...
		//}
	}
	
	if(this->estEspecial && this->estEspecialAntes){
		pagar += (minutos/30+1)*2;
	}
	
	if(this->dataPagamento.diffMin(agora)<=15)
		pagar = 0;
		
	//pagar = 10;
	//pagar = 10+((minutos-60)/30+1)*4;
	//pagar = (int)(10+((minutos-60)/30+1)*4);
	//pagar = (int)(this->valorpago);

	#if debugPagar == 1
		escreveVetor("calculaPgto",13);
		//escreve(minutosTotal>>8);
		escreve((char)pagar);
		escreve('\n');
	#endif
	
	return pagar;
}

int Veiculo::pagar(int valor, DataHora agora){
	if(valor > 0)
		this->dataPagamento.setByDataHora(agora);
	this->valorpago += valor;
	return this->valorpago;
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

char Veiculo::ehPlacaIgual(char compara[]){ 
	//retorna 1 se placa for igual ao parâmetro compara
	for(int i=0; i<7; i++){
		if(compara[i] != this->placa[i])
			return 0;
	}
	return 1;
}

void Veiculo::setPlaca(char _placa[]){
	for(int i=0; i<7; i++){
		this->placa[i] = _placa[i];
	}
	placa[7]='\0';
}
