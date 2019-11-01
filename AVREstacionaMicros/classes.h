/*
 * classes.h
 *
 * Created: 30/10/2019 18:57:07
 *  Author: Lucas
 */ 

#ifndef CLASSES_H_
#define CLASSES_H_

#define NAENTRADA 0
#define DENTRO 1
#define NASAIDA 2
#define FORA 3

class DataHora{
	public:
	char dia;
	char mes;
	char hora;
	char min;
	char seg;
	DataHora(char _dia=1, char _mes=1, char _hora=0, char _min=0);
	void setByVector(char datahoravetor[4]); //entra vetor recebido da Serial SH_dia_mes_hora_min
	DataHora somaMin(int minutos); //retorna data somada com minutos (não muda a data chamada)
	DataHora incSeg(char segundos=1);
	unsigned long int diffMin(DataHora comp); //calcula diferença em minutos
};

class Veiculo {
	public:             // Access specifier
	char placa[8];
	DataHora dataEntrada;
	DataHora dataPagamento;
	DataHora dataSaidaPaga;
	char estado=FORA;
	char reincidente=0;
	
	Veiculo(char _placa[7]="AAA0000", DataHora _dataEntrada = DataHora());
	void pagou(DataHora _dataPagamento);
	char ehEspecial();
	char ehPlacaIgual(char compara[]);
	
	private:
	void calculaSaidaPaga();
	
};

#endif /* CLASSES_H_ */