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
char procuraPlaca(char placaProcurada[]);
void verificaResposta(char respostaEsperada[], char n, char mensagem[]);
void pedeMapa(char c);
char countMapa(char mapa[3][5], char contAndar[3], char c);

char desbloqueado=0;
char cancelaAberta=0;
char estado=0, estadoAnterior=-1;
//DataHora relogio;

char telaNova=1;


Veiculo carros[120]; //máximo 120 carros no estacionamento


int main(void)
{
    /* Replace with your application code */
	char byteLido;
	char vetor[64];	
	char tmp;
	
	char carroEntrada=-1;
	DataHora horaCarroEntrada;
	char carroSaida=-1;
	DataHora horaCarroSaida;
	DataHora horaLetreiro;
	
	char novoBotao = 0;
	
	char numeroCartao[7];
	char numeroCartaoPos=0;	
	char numeroSenha[7];
	char numeroSenhaPos=0;
	
	char valor[3];
	char valorPos=0;
	
	char posCarro;
	
	
	


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
	
	char recontar = 0;
	
	DataHora ultMostraVagas;
	DataHora ultEO;
	
	char tLetreiro; //tempo letreiro preços
	char indice=0; //indice letreiro preços
	char letreiro[44] = "R$10 primeira hora, R$4 meia hora adicional";
	
	DataHora tempoTMP;
	
	DDRB = 1<<7;
	
	serialSetup();
	lcd_config();
	timer1_config();

    while (1) 
    {		
	
		PORTB ^= 1<<7;
		if(novoDado()){ //loop padrão
			byteLido = le();
			if(byteLido == 'S'){
				lcdWritePos("RECEBEU", 0,1);
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
						byteLido=le(); //1 entrada, 2 saída
						leVetor(vetor, 8); //Lê placa 7char+\0
						escreveVetor("EN", 2); //Resposta para Servidor
						posCarro = procuraPlaca(vetor);
						if(posCarro == -1){
							posCarro = procuraPlaca("###0000");
						}
						switch(byteLido){
							case '1':
								carros[posCarro].estado = NAENTRADA;
								carroEntrada = posCarro;
								if(desbloqueado){
									cancelaAbre(byteLido);
									horaCarroEntrada.setByDataHora(relogio);
								}else{
									clear_display();
									lcdWrite("Desligado!");
								}
								
								break;
							case '2':
								carros[posCarro].estado = NASAIDA;
								if(desbloqueado){
									if(carros[posCarro].calculaPgto(relogio) > 0){
										clear_display();
										lcdWrite("Realize o Pagamento");
										estado = NUMEROCARTAO;
										telaNova = 1;
									}else{
										cancelaAbre(byteLido);
										horaCarroSaida.setByDataHora(relogio);
									}
								}else{
									clear_display();
									lcdWrite("Desligado!");
								}
								break;
						}
						break; //N
					case 'S': //Carro saindo
						byteLido = le(); //1 entrada, 2 saída
						leVetor(vetor,8);
						escreveVetor("ES", 2);
						posCarro = procuraPlaca(vetor);
						atrasoms(2000);
						cancelaFecha(byteLido);
						recontar = 7;
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
								estado = SAIUENTRADA;
								break;
							case '2':
								if(carros[posCarro].estEspecial)
									carros[posCarro].estEspecialAntes=1;
								carroSaida = -1;
								carros[posCarro].estado = FORA;
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
							le();
							case '0':
								leVetor(mapa[0],5);
								countMapa(mapa, contAndar, '0');
								recontar &= ~(1);
								break;
							case '1':
								leVetor(mapa[1],5);
								countMapa(mapa, contAndar, '1');
								recontar &= ~(1<<1);
								break;	
							case '2':
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
		
		
		if(carroEntrada != (char)-1){
			if(horaCarroEntrada.diffSec(relogio) > 60){
				cancelaFecha('1');
				carroEntrada = -1;
				carros[carroEntrada].estado = FORA;
			}
		}
		if(carroSaida != (char)-1){
			if(horaCarroSaida.diffSec(relogio) > 60){
				cancelaFecha('2');
				carroSaida = -1;
				carros[carroSaida].estado = DENTRO;
			}
		}
		
		switch(estado){
			case ESTADOINICIAL:
				if(recontar & (1<<0)) pedeMapa('0');
				if(recontar & (1<<1)) pedeMapa('1');
				if(recontar & (1<<2)) pedeMapa('2');
				
				if(ultMostraVagas.diffSec(relogio)>15){
					ultMostraVagas = relogio;
					clear_display();
					lcdWritePos("Vagas Terreo:",0,0);
					tmp = 40-contAndar[0];					
					lcdWritechar(tmp/10+0x30);
					lcdWritechar(tmp%10+0x30);
					atrasoms(1000);
					
					clear_display();
					lcdWritePos("Vagas 1 andar:",0,0);
					tmp = 40-contAndar[1];
					lcdWritechar(tmp/10+0x30);
					lcdWritechar(tmp%10+0x30);
					atrasoms(1000);
					
					clear_display();
					lcdWritePos("Vagas 2 andar:",0,0);
					tmp = 40-contAndar[2];
					lcdWritechar(tmp/10+0x30);
					lcdWritechar(tmp%10+0x30);
					atrasoms(1000);
					telaNova = 1;
				}
				if(telaNova){
					clear_display();
					lcdWritePos("EstacionaMicros",0,0);					
					indice=0;
					telaNova=0;			
				}
				char horastr[3];
				if(horaLetreiro.diffSec(relogio) >= 1 || 1){
					
					setCursor(0,1);
					itoa(relogio.dia, horastr, 10);
					lcdWrite(horastr);
					itoa(relogio.mes, horastr, 10);
					lcdWrite(horastr);
					itoa(relogio.hora, horastr, 10);
					lcdWrite(horastr);
					itoa(relogio.min, horastr, 10);
					lcdWrite(horastr);
					itoa(horaLetreiro.dia, horastr, 10);
					lcdWrite(horastr);
					itoa(horaLetreiro.mes, horastr, 10);
					lcdWrite(horastr);
					itoa(horaLetreiro.hora, horastr, 10);
					lcdWrite(horastr);
					itoa(horaLetreiro.min, horastr, 10);
					lcdWrite(horastr);
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
				break;
			case NUMEROCARTAO:
				if(telaNova){
					if(carros[carroSaida].estEspecial){
						clear_display();
						lcdWritePos("Vaga Ilegal",0,0);
						lcdWritePos("Multa se reincid",0,1);
						if (carros[carroSaida].estEspecialAntes)
						{
							lcdWrite("Reincidente");
						}
					}
					clear_display();
					lcdWritePos("Pague R$ ",0,0);
					int pgto = carros[carroSaida].calculaPgto(relogio);
					lcdWritechar(carros[carroSaida].calculaPgto(relogio)%1000/100 + 0x30);//converte para ascii 0x30
					lcdWritechar(carros[carroSaida].calculaPgto(relogio)%100/10 + 0x30);//converte para ascii 0x30
					lcdWritechar(carros[carroSaida].calculaPgto(relogio)%10 + 0x30);//converte para ascii 0x30
					
					lcdWritePos("CARTAO",0,1);
					telaNova = 0;
					
					for(int i=0; i<numeroCartaoPos; i++){
						setCursor(0,1);
						lcdWritechar(numeroCartao[i]);
					}
				}
				novoBotao = le_teclado();
				if(novoBotao == (char)-1){
					if(novoDado()){
						if(le() == 'T')
						novoBotao = le();
					}
				}
				if(novoBotao == '*'){
					numeroCartao[numeroCartaoPos]='\0';
					lcdWritecharPos(' ',numeroCartaoPos,1);
					numeroCartaoPos--;
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
					}else{
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
										itoa(numeroSenhaPos, horastr, 10);
										lcdWritePos(horastr,0,1);
					if(numeroSenhaPos==6){
						numeroSenha[6]='\0';
						valor[2] = '\0';
						if(valorPos == 1){
							valor[1] = valor[0];
							valor[0] = '0';
						}
						escreveVetor("EP", 2);
						escreve(10);
						escreveVetor(numeroSenha,7);
						escreveVetor(valor, 3);
						estado=AGUARDASENHA;
					}
				}else if(novoBotao == '*' && valorPos == 0){
					numeroSenha[numeroSenhaPos]='\0';
					lcdWritecharPos(' ',numeroSenhaPos+6,1);
					numeroSenhaPos--;
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
					if(!strcmp(cartaoResposta, "Cartao Invalido")){ //se cartão é inválido, vai ESTADO INICIAL
						estado = ESTADOINICIAL;
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
			case SAIUENTRADA:
				tempoTMP.setByDataHora(relogio);
				if(tempoTMP.diffSec(relogio)>=5){
					cancelaFecha('1');
					estado = ESTADOINICIAL;
				}					
				break;
			
			case SAIUSAIDA:
				tempoTMP.setByDataHora(relogio);
				if(tempoTMP.diffSec(relogio)>=5){
					cancelaFecha('2');
					estado = ESTADOINICIAL;
				}
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
	char vetorSerial[2];
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

void erro(char vetor[]){ //Imprime erro e dá pausa de 1s.
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

char procuraPlaca(char placaProcurada[]){
	for(int i=0; i<120; i++){
		if(carros->ehPlacaIgual(placaProcurada))
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
				if(mapa[0][i] & (1<<j)){
					cont++;
				}
			}
		}
	}
	contAndar[andar] = cont;
	return cont;
}