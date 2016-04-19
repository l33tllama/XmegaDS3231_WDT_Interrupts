/*
 * USART_Debug.h
 *
 *  Created on: 4 Nov 2015
 *      Author: leo
 */

#ifndef USART_DEBUG_H_
#define USART_DEBUG_H_
#include <avr/io.h>
#include <stdio.h>

extern "C"{

	//static int usart_putchar(char c, FILE * stream);

	int usart_getchar(FILE * stream);
	void setDebugOutputPort(USART_t * port);
}

#endif /* USART_DEBUG_H_ */
