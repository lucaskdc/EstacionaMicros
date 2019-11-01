/*
 * AVREstacionaMicros.cpp
 *
 * Created: 29/10/2019 10:36:24
 * Author : Lucas
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include "atraso.h"
#include "serial.h"
#include "classes.h"
#include "lcd.h"
#include "teclado.h"
#include "rrelogio.h"

#define ESTADOINICIAL 0
#define NUMEROCARTAO 1
#define SENHA 2
#define AGUARDACARTAO 3

void bloqueia();
void desbloqueia();
void cancelaAbre(char entradaSaida);
void cancelaFecha(char entradaSaida);
char procuraPlaca(char placaProcurada[]);
void verificaResposta(char respostaEsperada[], char n, char mensagem[]);
void pedeMapa(char c);

char desbloqueado=0;
char cancelaAberta=0;
char estado=0, estadoAnterior=-1;
//DataHora relogio;

Veiculo carros[120]; //máximo 120 carros no estacionamento


int main(void)
{
    /* Replace with your application code */
	char byteLido;
	char vetor[64];	
	
	char carroEntrada=-1;
	DataHora horaCarroEntrada;
	char carroSaida=-1;
	DataHora horaCarroSaida;
	
	char novoBotao = 0;
	
	char numeroCartao[7];
	char numeroCartaoPos=0;	
	char numeroSenha[7];
	char numeroSenhaPos=0;
	
	char valor[3];
	char valorPos=0;
	
	char posCarro;
	
	char telaNova=1;

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
						relogio.setByVector(vetor);
						break; //H
					case 'N': //Novo Carro
						byteLido=le(); //1 entrada, 2 saída
						leVetor(vetor, 8); //Lê placa 7char+\0
						escreveVetor("EN", 2); //Resposta para Servidor
						posCarro = procuraPlaca(vetor);
						if(posCarro == -1){
							posCarro = procuraPlaca("AAA0000");
						}
						switch(byteLido){
							case '1':
								carros[posCarro].estado = NAENTRADA;
								if(desbloqueado)
									cancelaAbre(byteLido);
								else{
									clear_display();
									lcdWrite("Desligado!");
								}
								
								break;
							case '2':
								carros[posCarro].estado = NASAIDA;
								if(desbloqueado){
									if(carros[posCarro].dataSaidaPaga.diffMin(relogio) > 0){
										clear_display();
										lcdWrite("Realize o Pagamento");
										estado = NUMEROCARTAO;
									}else{
										cancelaAbre(byteLido);
									}
								}else{
									clear_display();
									lcdWrite("Bloqueado");
								}
								break;
						}
						break; //N
					case 'S': //Carro saindo
						byteLido = le(); //1 entrada, 2 saída
						leVetor(vetor,8);
						escreveVetor("ES", 2);
						posCarro = procuraPlaca(vetor);
						switch(byteLido){
							case '1':
								carroEntrada = -1;
								carros[posCarro].estado = DENTRO;
								break;
							case '2':
								carroSaida = -1;
								carros[posCarro].estado = FORA;
								break;
						}
						
						cancelaFecha(byteLido);
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
								break;
							case '1':
								leVetor(mapa[1],5);
							break;
							case '2':
								leVetor(mapa[2],5);
							break;
						}
						break;
						
				}//switch
			}//if bytelido == 'S'
		}// if novoDado
		
		if(carroEntrada != -1){
			if(horaCarroEntrada.diffSec(relogio) > 60){
				cancelaFecha('1');
				carroEntrada = -1;
				carros[carroEntrada].estado = FORA;
			}
		}
		if(carroSaida != -1){
			if(horaCarroSaida.diffSec(relogio) > 60){
				cancelaFecha('1');
				carroSaida = -1;
				carros[carroSaida].estado = DENTRO;
			}
		}
		
		switch(estado){
			case ESTADOINICIAL:
				break;
			case NUMEROCARTAO:
				if(telaNova){
					clear_display();
					lcdWritePos("Digite cartao:",0,0);
					telaNova = 0;
				}
				novoBotao = le_teclado();
				if(novoBotao == '*'){
					numeroCartao[numeroCartaoPos]='\0';
					lcdWritecharPos(novoBotao,numeroCartaoPos,1);
					numeroCartaoPos--;
				}else if(novoBotao == '#'){
					if(numeroCartaoPos==6){
						numeroCartao[6]='\0';
						escreveVetor("EC",2);
						escreve(7);
						escreveVetor(numeroCartao, 7);
					
						telaNova = 1;
						estado = AGUARDACARTAO;
					}
				}else if(novoBotao != -1){
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
					}
				}
				break;
			case SENHA:
				clear_display();
				lcdWritePos("Senha e valor:",0,0);
				novoBotao = le_teclado();
				if(novoBotao == '#'){
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
					}
				}else if(novoBotao == '*' & valorPos == 0){
					numeroSenha[numeroSenhaPos]='\0';
					lcdWritecharPos(novoBotao,numeroSenhaPos,1);
					numeroSenhaPos--;
				}else if(novoBotao != '#' & numeroSenhaPos==6){
					numeroSenha[6]='\0';
					estado = SENHA;
				}else if(novoBotao != -1 & numeroSenhaPos < 6){
					numeroCartao[numeroCartaoPos]=novoBotao;
					lcdWritecharPos('*',numeroCartaoPos,1); //2a linha
					numeroCartaoPos++;
				}
				break;
		}
		
		
		escreveVetor("EO", 2);
		atrasoms(100);
    }
}

void bloqueia(){
	//DESLIGADO NA TELA
	//16 - 9 = 7 -> esceve na coluna 4
	clear_display();
	lcdWritePos("Desligado", 1, 4);
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