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
	void setByDataHora(DataHora a);
	void setByDataHoraZeroSeconds(DataHora min);
	DataHora somaMin(int minutos); //retorna data somada com minutos (não muda a data chamada)
	void incSeg(char segundos=1);
	int diffMin(DataHora comp); //calcula diferença em minutos comp - this
	int diffSec(DataHora comp); //calcula diferença em minutos comp - this
};

class Veiculo {
	public:             // Access specifier
	char placa[8];
	DataHora dataEntrada;
	DataHora dataPagamento;
	int valorpago;
	char estado;
	char estEspecial;
	char estEspecialAntes;
	
	Veiculo(DataHora _dataEntrada = DataHora());
	void pagou(DataHora _dataPagamento);
	char ehEspecial();
	char ehPlacaIgual(char compara[]);
	int pagar(int valor, DataHora agora);
	int calculaPgto(DataHora agora);
	void setPlaca(char _placa[]);
	private:
	
};

#endif /* CLASSES_H_ */