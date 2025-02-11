/*
 * AVREstacionaMicros.cpp
 *
 * Created: 29/10/2019 10:36:24
 * Author : Lucas
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>
#include "atraso.h"
#include "serial.h"
#include "classes.h"
#include "lcd.h"
#include "teclado.h"
#include "rrelogio.h"

enum{
	ESTADOINICIAL,
	VAGAS,
	DIGITAPLACA,
	NUMEROCARTAO,
	SENHA,
	AGUARDACARTAO,
	AGUARDASENHA,
	BLOQUEADO,
	SAIUENTRADA,
	SAIUSAIDA
};

void bloqueia();
void desbloqueia();
void cancelaAbre(char entradaSaida);
void cancelaFecha(char entradaSaida);
int procuraPlaca(char placaProcurada[]);
int procuraPlacaNumero(char numero[]);
void verificaResposta(char respostaEsperada[], char n, char mensagem[]);
void pedeMapa(char c);
char countMapa(char mapa[3][5], char contAndar[3], char c);
void escreveNota(Veiculo carro);

char desbloqueado=0;
char cancelaAberta=0;
char estado=BLOQUEADO, estadoAnterior=-1;
//DataHora relogio;

char telaNova=1;


Veiculo carros[120]; //m�ximo 120 carros no estacionamento


int main(void)
{   // saindo da entrada = entrando no estacionamento
    /* Replace with your application code */
	char byteLido;
	char vetor[64];	
	char tmp;
	
	int carroEntrada=-1;
	DataHora horaCarroEntrada;
	int carroSaida=-1;
	DataHora horaCarroSaida;
	DataHora horaLetreiro;
	
	char novoBotao = 0;
	
	char numeroPlacaPos=0;
	char numeroPlaca[5];
	int carroPgto;
	
	char numeroCartao[7];
	char numeroCartaoPos=0;	
	char numeroSenha[7];
	char numeroSenhaPos=0;
	
	//char valor[3];
	//char valorPos=0;
	
	int posCarro;
	
	
	


	char bloqueadoPorHorario=0;

	char cartaoResposta[32];
	char cartaoRepostaNova = 0;

	char senhaResposta[32];
	char senhaRespostaNova = 0;
	
	char n;
	
	char mapa[3][5];
	for(int i=0; i<3; i++){
		for(int j=0; j<5; j++){
			mapa[i][j] = 0;
		}
	}
	char contAndar[3]={0,0,0};
	char cont0antes = 0;
	
	char recontar = 7;
	
	DataHora ultMostraVagas;
	DataHora ultEO;
	
	char tLetreiro; //tempo letreiro pre�os
	char indice=0; //indice letreiro pre�os
	char letreiro[44] = "R$10 primeira hora, R$4 meia hora adicional";
	
	DataHora tempoTMP;
	
	int valorAguardaResposta=0;
	
	DDRB |= 1<<7;
	DDRF |= 1<<0; // LED VERDE
	DDRF |= 1<<1; // LED AMARELO
	DDRF |= 1<<2; // LED VERMELHO

    PORTF &= ~(0b00000111);

	serialSetup();
	lcd_config();
	timer1_config();

    while (1) 
    {		
	
		//PORTB ^= 1<<7;
		if(novoDado()){ //loop padr�o
			byteLido = le();
			if(byteLido == 'S'){
				//lcdWritePos("RECEBEU", 0,1);
				byteLido = le();
				switch (byteLido)
				{
					case 'B': //Bloqueia
						escreveVetor("EB",2);
						bloqueia();						
						break; //B
					case 'D': //Desbloqueia
						escreveVetor("ED", 2);
						desbloqueia();
						break; //D
					case 'H': //Recebe DataHora
						leVetor(vetor, 4);
						relogio.setByVector(vetor);
						escreveVetor("EH",2);
						break; //H
					case 'N': //Novo Carro
						byteLido=le(); //1 entrada, 2 sa�da
						leVetor(vetor, 8); //L� placa 7char+\0
						escreveVetor("EN", 2); //Resposta para Servidor
						lcdWritePos("                ", 0,0); //clear parcial display
						lcdWritePos(vetor, 0,0);
						posCarro = procuraPlaca(vetor);
						lcdWrite(" ");
						char pos[4];
						//lcdWritechar(posCarro/100 +0x30);
						//lcdWritechar(posCarro%100/10 +0x30);
						//lcdWritechar(posCarro%100 +0x30);
						
						if(posCarro == -1){
							posCarro = procuraPlaca("#######");
							carros[posCarro].setPlaca(vetor);
						}
						lcdWrite(" ");
						itoa(posCarro, pos, 10);
						lcdWrite(pos);
						
						lcdWrite(" ");
						//itoa(carros[posCarro].valorpago, pos, 10);
						atrasoms(1000);
						
						switch(byteLido){
							case '1':
								carros[posCarro].estado = NAENTRADA;
								carros[posCarro].valorpago = 0; //Valor pago � 0 quando chega na cancela para entrar.
								carroEntrada = posCarro;
								horaCarroEntrada.setByDataHora(relogio);
								
								if(desbloqueado){
									cancelaAbre(byteLido);
								}else{
									clear_display();
									lcdWrite("Desligado!");
								}
								
								break;
							case '2':
								carros[posCarro].estado = NASAIDA;
								carroSaida= posCarro;
								horaCarroSaida.setByDataHora(relogio);
								
								if(desbloqueado){
									if(carros[posCarro].calculaPgto(relogio) > 0){
										clear_display();
										lcdWrite("Realize o Pagamento");
										lcdWritePos("Aperte \"A\".", 0, 1);
										atrasoms(1000);
										telaNova = 1;
									}else{
										cancelaAbre(byteLido);
									}
								}else{
									clear_display();
									lcdWrite("Desligado!");
								}
								break;
						}
						break; //N
					case 'S': //Carro saindo
						byteLido = le(); //1 entrada, 2 sa�da
						leVetor(vetor,8);
						escreveVetor("ES", 2);
						posCarro = procuraPlaca(vetor);
						atrasoms(2000);
						cancelaFecha(byteLido);
						recontar = 7; //0111, os 3 andares
						switch(byteLido){
							case '1':
								cont0antes = contAndar[0];
								pedeMapa('0');
								atrasoms(100);
								countMapa(mapa, contAndar,'0');
								if((contAndar[0] != cont0antes) & !carros[posCarro].ehEspecial()){
									carros[posCarro].estEspecial=1;
								}else{
									carros[posCarro].estEspecial=0;
								}
								carroEntrada = -1;
								carros[posCarro].estado = DENTRO;
								carros[posCarro].dataEntrada.setByDataHora(relogio);
								estado = SAIUENTRADA;
								break;
							case '2':
								if(carros[posCarro].estEspecial)
									carros[posCarro].estEspecialAntes=1;
								carros[posCarro].estado = FORA;
								carroSaida = -1;
								estado = SAIUSAIDA;
								break;
						}
						break; //S
					case 'C': //Resposta Cartao
						n = le();
						leVetor(cartaoResposta, n);
						cartaoRepostaNova = 1;
						break;
					case 'P':
						n = le();
						leVetor(senhaResposta, n);
						senhaRespostaNova = 1;
						break;
					case 'M':
						switch(le()){
							case '0':
								le();
								leVetor(mapa[0],5);
								countMapa(mapa, contAndar, '0');
								recontar &= ~(1);
								break;
							case '1':
								le();
								leVetor(mapa[1],5);
								countMapa(mapa, contAndar, '1');
								recontar &= ~(1<<1);
								break;	
							case '2':
								le();
								leVetor(mapa[2],5);
								countMapa(mapa, contAndar, '2');
								recontar &= ~(1<<2);
								break;
						}
						break;
						
				}//switch
			}//if bytelido == 'S'
		}// if novoDado
		
		if(!bloqueadoPorHorario && relogio.hora<6){
			bloqueadoPorHorario = 1;
			if(desbloqueado || estado != BLOQUEADO)
				bloqueia();
		}
		if(bloqueadoPorHorario && relogio.hora>=6){
			bloqueadoPorHorario = 0;
			if(!desbloqueado || estado == BLOQUEADO)
				desbloqueia();
		}
		
		
		if(carroEntrada != (int)-1){
			if(horaCarroEntrada.diffSec(relogio) > (int)40){
				
				if(relogio.seg%2){
					PORTF |= 1<<0; // LED VERDE ACENDE (CANCELA DE ENTRADA)
				} else {
					PORTF &= ~(1<<0); // LED VERDE APAGA (CANCELA DE ENTRADA)
				}
			}else{
				PORTF &= ~(1<<0); // LED VERDE APAGA (CANCELA DE ENTRADA)
			}
			if(horaCarroEntrada.diffSec(relogio) > (int)60){
				cancelaFecha('1');
				carros[carroEntrada].estado = FORA;
				carroEntrada = -1;

			}
		}
		if(carroSaida != -1){
			if(horaCarroSaida.diffSec(relogio) > (int)40){
				if(relogio.seg%2){
					PORTF |= 1<<1; // LED AMARELO ACENDE (CANCELA DE SA�DA)
					} else {
					PORTF &= ~(1<<1); // LED AMARELO APAGA (CANCELA DE SA�DA)
				}
				}else{
				PORTF &= ~(1<<1); // LED AMARELO APAGA (CANCELA DE SA�DA)
			}
			if(horaCarroSaida.diffSec(relogio) > (int)60){
				cancelaFecha('2');
				carros[carroSaida].estado = DENTRO;
				carroSaida = -1;
			}
		}
		
		switch(estado){
			case ESTADOINICIAL:
				if(recontar & (1<<0)) pedeMapa('0');
				if(recontar & (1<<1)) pedeMapa('1');
				if(recontar & (1<<2)) pedeMapa('2');
				

				if(telaNova){
					clear_display();
					lcdWritePos("EstacionaMicros",0,0);					
					indice=0;
					telaNova=0;
				}
				char horastr[3];
				if(horaLetreiro.diffSec(relogio) >= 1 /*|| 1*/){
					//prints horario:
					setCursor(0,1);
					//lcdWrite("        "); //clear local, posicao do horario
					//setCursor(0,1);
					if(relogio.hora <10) lcdWritechar('0');
					itoa(relogio.hora, horastr, 10);
					lcdWrite(horastr);
					lcdWritechar(':');
					if(relogio.min <10) lcdWritechar('0');
					itoa(relogio.min, horastr, 10);
					lcdWrite(horastr);
					lcdWritechar(':');
					if(relogio.seg <10) lcdWritechar('0');
					itoa(relogio.seg, horastr, 10);
					lcdWrite(horastr);		

					//itoa(horaLetreiro.hora, horastr, 10);
					//lcdWrite(horastr);
					//itoa(horaLetreiro.min, horastr, 10);
					//lcdWrite(horastr);
					
					/*for(char i=0; i<16; i++){
						if(i+indice>=44){
							lcdWritechar(letreiro[i+indice-44]);
						}else{
							lcdWritechar(letreiro[i+indice]);
						}
					}*/
					indice++;
					horaLetreiro.setByDataHora(relogio);
//					lcdWritecharPos(relogio.seg/10+0x30, 0,1);
//					lcdWritecharPos(relogio.seg%10+0x30, 1,1);
					
//					lcdWritecharPos(horaLetreiro.seg/10+0x30, 3,1);
//					lcdWritecharPos(horaLetreiro.seg%10+0x30, 4,1);
				}
				if(ultMostraVagas.diffSec(relogio)>20){
					ultMostraVagas.setByDataHora(relogio);
					estado = VAGAS;
					telaNova = 1;
				}
				if(le_teclado() == 'A'){
					estado = DIGITAPLACA;
					telaNova = 1;
				}
				break;
			case VAGAS:
					if(telaNova){
						clear_display();
						lcdWritePos("                ", 0, 1);
						telaNova = 0;
					}
					switch(ultMostraVagas.diffSec(relogio)){
						case 0:
						case 1:
							lcdWritePos("Vagas Terreo: ",0,0);
							tmp = 36-contAndar[0];
							lcdWritechar(tmp/10+0x30);
							lcdWritechar(tmp%10+0x30);
							break;
						case 2:
						case 3:				
							lcdWritePos("Vagas 1 andar:",0,0);
							tmp = 36-contAndar[1];
							lcdWritechar(tmp/10+0x30);
							lcdWritechar(tmp%10+0x30);
							break;
						case 4:
						case 5:			
							lcdWritePos("Vagas 2 andar:",0,0);
							tmp = 36-contAndar[2];
							lcdWritechar(tmp/10+0x30);
							lcdWritechar(tmp%10+0x30);
							break;
						case 6:
						default:
							estado = ESTADOINICIAL;
							telaNova = 1;
					}
				break;
			case DIGITAPLACA:
				if(telaNova){
					clear_display();
					lcdWritePos("N� placa para",0,0);
					lcdWritePos("Pagamento:",0,1);
					numeroPlacaPos = 0;
					telaNova = 0;
				}
				novoBotao = le_teclado();
				if(novoBotao == (char)-1){
					
				}else if(novoBotao == '*'){
					if(numeroPlacaPos>0){
						numeroPlacaPos--;
						numeroPlaca[numeroPlacaPos]='\0';
						lcdWritecharPos(' ',numeroPlacaPos+10,1);
					}else{
						estado = ESTADOINICIAL;
						telaNova = 1;
					}
				}else if(novoBotao == '#'){
					if(numeroPlacaPos==4){
						numeroPlaca[4] = '\0';
						carroPgto = procuraPlacaNumero(numeroPlaca);
						
						if(carroPgto == -1){
							clear_display();
							lcdWritePos("Placa estranha",0,0);
							lcdWritePos(numeroPlaca, 0,1);
							atrasoms(1000);
							estado = ESTADOINICIAL;
							telaNova = 1;
						}else{
							if(carros[carroPgto].calculaPgto(relogio) == 0){
								clear_display();
								lcdWritePos("Esta Pago!", 0, 1);
								carroPgto = -1;
								telaNova = 1;
								estado = ESTADOINICIAL;
							}else{
								numeroPlacaPos = 0;
								telaNova = 1;
								estado = NUMEROCARTAO;
							}
						}
						
					}
				}else if(novoBotao != (char)-1){
					if(numeroPlacaPos<4){
						numeroPlaca[numeroPlacaPos]=novoBotao;
						lcdWritecharPos(novoBotao,numeroPlacaPos+10,1); //2a linha
						numeroPlacaPos++;
					}
				}
				break;
			case NUMEROCARTAO:
				if(telaNova){
					if(carros[carroPgto].estEspecial){
						if (carros[carroPgto].estEspecialAntes) //se um n�o-especial estacionou ilegalmente na vaga IDE
						{
							lcdWrite("Reincidente");
							atrasoms(2000);
						}else{
							clear_display();
							lcdWritePos("Vaga Ilegal",0,0);
							lcdWritePos("prox = MULTA",0,1);
						}
					}
					clear_display();
					lcdWritePos("Pague R$ ",0,0);
					//int pgto = carros[carroPgto].calculaPgto(relogio);
					char pgto[5];
					itoa(carros[carroPgto].calculaPgto(relogio), pgto, 10);
					lcdWrite(pgto);
					
					lcdWritePos("CARTAO",0,1);
					telaNova = 0;
					
					for(int i=0; i<numeroCartaoPos; i++){
						setCursor(0,1);
						lcdWritechar(numeroCartao[i]);
					}
				}

				novoBotao = le_teclado();
				if(novoBotao == (char)-1){
					
				}else if(novoBotao == '*'){
					if(numeroCartaoPos>0){
						numeroCartaoPos--;
						numeroCartao[numeroCartaoPos]='\0';
						lcdWritecharPos(' ',numeroCartaoPos,1);
					}
				}else if(novoBotao == '#'){
					if(numeroCartaoPos==6){
						numeroCartao[6]='\0';
						escreveVetor("EC",2);
						escreve(7);
						escreveVetor(numeroCartao, 7);
						numeroCartaoPos = 0;
						telaNova = 1;
						estado = AGUARDACARTAO;
					}
				}else if(novoBotao != (char)-1){
					numeroCartao[numeroCartaoPos]=novoBotao;
					lcdWritecharPos(novoBotao,numeroCartaoPos,1); //2a linha
					numeroCartaoPos++;
				}
				break;
			case AGUARDACARTAO:
				if(telaNova){
					clear_display();
					lcdWritePos("Espera Resp...", 0 ,0);
					telaNova = 0;
				}
				if(cartaoRepostaNova){
					cartaoRepostaNova = 0;
					clear_display();
					lcdWritePos(cartaoResposta,0,0);
					atrasoms(1000);
					telaNova = 1;
					if(!strcmp(cartaoResposta, "Cartao Invalido")){
						estado = ESTADOINICIAL;
						telaNova = 1;
					}else{
						numeroSenhaPos = 0;
						estado = SENHA;
						telaNova = 1;
					}
				}
				break;
			case SENHA:
				if(telaNova){
					clear_display();
					lcdWritePos(cartaoResposta, 0,0);
					lcdWritePos("Senha:",0,1);
					telaNova  = 0;
				}
				novoBotao = le_teclado();
				if(novoBotao == (char)-1){

				}else if(novoBotao == '#'){
					if(numeroSenhaPos==6){
						numeroSenha[6]='\0';
						char strpgto[3];
						int valorAguardaResposta = carros[carroPgto].calculaPgto(relogio);
						itoa(valorAguardaResposta,strpgto,10);
						strpgto[2] = '\0';
						escreveVetor("EP", 2);
						escreve(10);
						escreveVetor(numeroSenha,7);
						escreveVetor(strpgto, 3);
						estado=AGUARDASENHA;
						telaNova = 1;
					}
				}else if(novoBotao == '*'){
					if(numeroSenhaPos > 0){
						numeroSenhaPos--;
						numeroSenha[numeroSenhaPos]='\0';
						lcdWritecharPos(' ',numeroSenhaPos+6,1);
					}
				}else if(novoBotao != (char)-1 && numeroSenhaPos < 6){
					numeroSenha[numeroSenhaPos]=novoBotao;
					lcdWritecharPos('*',numeroSenhaPos+6,1); //2a linha
					numeroSenhaPos++;
				}
				break;
			case AGUARDASENHA:
				if(telaNova){
					clear_display();
					lcdWritePos("Espera Resp...", 0 ,0);
					telaNova = 0;
				}
				if(senhaRespostaNova){
					senhaRespostaNova = 0;
					clear_display();
					lcdWritePos(senhaResposta,0,0);
					atrasoms(1000);
					telaNova = 1;
					senhaRespostaNova=0;
					if(!strcmp(senhaResposta, "Cartao Invalido")){ //se cart�o � inv�lido, vai ESTADO INICIAL
						estado = ESTADOINICIAL;
					}else if(!strcmp(senhaResposta, "OK")){
						carros[carroPgto].pagar(valorAguardaResposta, relogio);
						cancelaAbre('2');
						estado = ESTADOINICIAL;
						telaNova = 1;
						//escreveNota(carros[carroPgto]);
					}else if(!strcmp(senhaResposta, "Saldo Invalido")){
						estado = ESTADOINICIAL;
						telaNova = 1;
					}else if(!strcmp(senhaResposta, "Senha Invalida")){
						numeroSenhaPos = 0;
						estado = SENHA;
						telaNova = 1;
					
					}else{
						estado = SENHA;
						telaNova = 1;
					}
				}
				break;
			case BLOQUEADO:
				if(telaNova){
					clear_display();
					lcdWritePos("Desligado", 4, 1);
				}
				break;
			case SAIUENTRADA:
				estado = ESTADOINICIAL;
				/*
				tempoTMP.setByDataHora(relogio);
				if(tempoTMP.diffSec(relogio)>=5){
					cancelaFecha('1');
					estado = ESTADOINICIAL;
				}		*/ 			
				break;
			
			case SAIUSAIDA:
			
				estado = ESTADOINICIAL; 
				/*
				tempoTMP.setByDataHora(relogio);
				if(tempoTMP.diffSec(relogio)>=5){
					cancelaFecha('2');
					estado = ESTADOINICIAL;
				}*/
				break;
			
		}//switch(estado)
		
		if(ultEO.diffSec(relogio)>40){
			escreveVetor("EO", 2);
			ultEO.setByDataHora(relogio);
		}
		atrasoms(50);
    }//FIM loop infinito
} //FIM main

void bloqueia(){
	cancelaFecha('1');
	cancelaFecha('2');
	estado = BLOQUEADO;
	telaNova = 1;
	desbloqueado = 0;
}

void desbloqueia(){
	estado = ESTADOINICIAL;
	telaNova = 1;
	desbloqueado = 1;
}

void cancelaAbre(char entradaSaida){ //'E','A',n
	escreveVetor("EA", 2);
	escreve(entradaSaida);
	atrasoms(2);
	//verificaResposta("SA", 2, "Erro Abertura Cancela");
}

void cancelaFecha(char entradaSaida){ //'E','F',n
	//char vetorSerial[2];
	escreveVetor("EF", 2);
	escreve(entradaSaida);
	atrasoms(2);
	//verificaResposta("SF", 2, "Erro Fechamento Cancela");
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

void erro(char vetor[]){ //Imprime erro e d� pausa de 1s.
	clear_display();
	lcdWrite(vetor);
	atrasoms(1000);
}

void verificaResposta(char respostaEsperada[], char n, char mensagem[]){
	for(int i=0; i<n; i++){
		if(le() != respostaEsperada[i]){
			erro(mensagem);
		}
	}
}

int procuraPlaca(char placaProcurada[]){
	for(int i=0; i<120; i++){
		if(carros[i].ehPlacaIgual(placaProcurada))
			return i;
	}
	return -1;
}

int procuraPlacaNumero(char numero[]){
	for(int i=0; i<120; i++){
		if(carros[i].ehNumeroPlacaIgual(numero))
			return i;
	}
	return -1;
}
void pedeMapa(char c){
	escreveVetor("EM",2);
	escreve(c);
}

char countMapa(char mapa[3][5], char contAndar[3], char c){
	char cont = 0;
	char andar = -1;
	switch(c){
		case '0':
			andar = 0;
			break;
		case '1':
			andar = 1;
			break;
		case '2':
			andar = 2;
			break;
	}
	if(andar != (char)-1){
		for(int i=0; i<5; i++){
			for(int j=0; j<8; j++){
				if(mapa[andar][i] & (1<<j)){
					cont++;
				}
			}
		}
	}
	contAndar[andar] = cont;
	return cont;
}
void escreveNota(Veiculo carro){

	char pagamento[4];

	escreveVetor("EI",2);
	escreve(0);
	escreve(16);


	escreveVetor(carro.placa,8);
	itoa(carro.valorpago,pagamento,10);
	escreveVetor(pagamento,4);
	escreveVetor("aaaa",4);
};

