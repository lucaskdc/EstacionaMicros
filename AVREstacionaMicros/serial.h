/*
 * serial.h
 *
 * Created: 29/10/2019 20:51:59
 *  Author: Lucas
 */ 
#define TX_BUFFER_SIZE 255
#define RX_BUFFER_SIZE 255


#ifndef SERIAL_H_
#define SERIAL_H_

void serialSetup();
char le();
void leVetor(char vetor[], int n);
void escreve(char dado);
void escreveVetor(char dados[], int n);
char novoDado();



#endif /* SERIAL_H_ */