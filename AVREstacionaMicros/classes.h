/*
 * classes.h
 *
 * Created: 30/10/2019 18:57:07
 *  Author: Lucas
 */ 


#ifndef CLASSES_H_
#define CLASSES_H_

class DataHora{
	public:
	char dia;
	char mes;
	char hora;
	char min;
	DataHora(char _dia=1, char _mes=1, char _hora=0, char _min=0);
	void setByVector(char datahoravetor[4]); //entra vetor recebido da Serial SH_dia_mes_hora_min
	DataHora somaMin(int minutos); //retorna data somada com minutos (n�o muda a data chamada)
	unsigned long int diffMin(DataHora comp); //calcula diferen�a em minutos
};

class Veiculo {
	public:             // Access specifier
	char placa[8];
	DataHora dataEntrada;
	DataHora dataPagamento;
	DataHora dataSaidaPaga;
	
		
	Veiculo(char _placa[7]="AAA0000", DataHora _dataEntrada = DataHora());
	void pagou(DataHora _dataPagamento);
	private:
	void calculaSaidaPaga();
	char ehEspecial();
};

#endif /* CLASSES_H_ */