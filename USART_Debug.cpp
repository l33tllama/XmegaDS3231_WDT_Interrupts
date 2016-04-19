#include "USART_Debug.h"
#include <stdio.h>
#include <avr/io.h>
extern "C"{

	static USART_t * stdio_usart;
	static FILE * usart_str;

	static int usart_putchar(char c, FILE * stream){
		if (c == '\n')
			usart_putchar('\r', stream);

		// Wait for the transmit buffer to be empty
		while (  !(stdio_usart->STATUS & USART_DREIF_bm) );

		// Put our character into the transmit buffer
		stdio_usart->DATA = c;

		return 0;
	}

	int usart_getchar(FILE *stream){
		while( !(stdio_usart->STATUS & USART_RXCIF_bm) ); //Wait until data has been received.
		char data = stdio_usart->DATA; //Temporarily store received data
		if(data == '\r')
			data = '\n';
		usart_putchar(data, stream); //Send to console what has been received, so we can see when typing
		return data;
	}

	void setDebugOutputPort(USART_t * port){

		// set USART port for debugging
		stdio_usart = port;

		usart_str = fdevopen(usart_putchar, usart_getchar);

		// Tell printf to print via USART..
		stdout = stdin = usart_str;
	}

}
