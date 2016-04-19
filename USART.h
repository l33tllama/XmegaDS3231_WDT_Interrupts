/*
 * USART.h
 *
 *  Created on: 20 Oct 2015
 *      Author: leo
 */

#ifndef USART_H_
#define USART_H_
//#define F_CPU 3200000
#include <stdio.h>
#include <avr/io.h>

typedef struct USART_Data_Struct{
	PORT_t * port;
	USART_t * usart_port;
	int txPin;
	int rxPin;
	int baudRate;
} USART_Data;

class USART {
private:
	PORT_t * port;
	USART_t * usart_port;
public:
	USART();
	USART(USART_Data * usart_data, bool interrupt_en);
	void putChar(int c);
	char getChar();
	void putStr(char * str);
	virtual ~USART();
};

#endif /* USART_H_ */
