/*
 * lcd.c
 *
 * Created: 29/10/2019 18:09:10
 * Author : oskar
 * microcontrolador: ATMEGA2560
 */ 

/*
conexões:
Pino de RS:     PC7 (pino 30)
Pino de Enable: PC6 (pino 31)
Porta de dados do LCD --> porta A (PA0 a PA7 <==> pino 22 a pino 29)
*/
/*
Para alterar conexões: 
	1) declarar as portas como saída na função lcd_config()
	2) ajustar o delocamento de bit do pino ENABLE e a porta de DADOS na função porta_lcd()
	3) ajustar o delocamento de bit do pino RS nas funções lcd_comando() e lcdWritechar()
Para utilizar pinos de múltiplas portas nos dados:
	if((data_display & 0b00000001) == 0b00000001) PORTC |= (1<<4);
	else PORTC &= ~(1<<4); 
	//exemplo, bit 0 de dados do lcd no pino PC4. repetir para os outros 7 bits
*/

#include <avr/io.h>
#include <string.h>
#include "atraso.h"

void porta_lcd(char data_display){ // escreve na porta de dados
	PORTC |=  (1<<6);     // E = 1
	PORTA = data_display; //dados
	/* 
	datasheet do HD44780U: "enable pulse width (HL) min = 450ns"
	são necessários 7.2 ciclos de 62.5ns 
	"for" de 3 ciclos rodando 4 vezes é suficiente
	*/
	unsigned char i;
	for (i= 0; i<8; i++)
	{
		i++;
	}
	PORTC &= ~(1<<6);     // E = 0
	atrasous(40);
}

void lcd_comando(char data_display){
	PORTC &= ~(1<<7); // RS = 0
	porta_lcd(data_display);
}

void lcdWritechar(char data_display){
	PORTC |=  (1<<7); // RS = 1
	porta_lcd(data_display);
}


/*void clear_display(){
	lcd_comando(0x01);
	atrasous(1650);
}*/

void setCursor(unsigned int coluna, unsigned int linha){
	lcd_comando(0x80); //coloca o cursor na linha 0
	int i = 0;
	if(linha == 1) lcd_comando(0xC0); //coloca o cursor na linha 1
	while(i < coluna){      //desloca o cursor para a direita até atingir a coluna desejada
		lcd_comando(0x14);
		i++;
	}
}

void lcdWrite(char string1[16]){ //máximo 16 caracteres, tamanho da linha do display
	unsigned int tamanho = 0, i;
	do{ //mede o tamanho da string
		tamanho++;
	} while(string1[tamanho] != '\0');
	for(i = 0; i< tamanho; i++) lcdWritechar(string1[i]); //print
}

void lcdWritePos(char string1[16],char coluna , char linha){
	setCursor(coluna, linha);
	lcdWrite(string1);
}

void lcdWritecharPos(char c, char coluna, char linha){
	setCursor(coluna, linha);
	lcdWritechar(c);
}

void clear_display(){
	lcdWritePos("                ", 0 , 0);
	lcdWritePos("                ", 0 , 1);
	setCursor(0,0);
}

void lcd_config(){
		DDRA = 0xFF;   //dados
		DDRC |= (1<<6);//E
		DDRC |= (1<<7);//RS
	//atrasoms(15);      //esperar HD44780 zerar a busy flag 
	lcd_comando(0x38); //function set
	lcd_comando(0x0E); //display control
	lcd_comando(0x06); //entry mode set
	clear_display();
}





