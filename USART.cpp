/*
 * USART.cpp
 *
 *  Created on: 20 Oct 2015
 *      Author: leo
 */

#include "USART.h"
#include <avr/io.h>
#include <string.h>

USART::USART(){
	usart_port = NULL;
	port = NULL;
};

USART::USART(USART_Data * usart_data, bool interrupt_en) {
	this->port = usart_data->port;
	this->usart_port = usart_data->usart_port;

	uint8_t txPin = usart_data->txPin;
	uint8_t rxPin = usart_data->rxPin;

	// Setup USART port pins
	port->OUTSET = txPin;
	port->DIRSET = txPin;

	port->OUTCLR = rxPin;
	port->DIRCLR = rxPin;

	usart_port->BAUDCTRLB = 0; //Just to be sure that BSCALE is 0
	float CTRLA_Calc = (F_CPU / (16 * usart_data->baudRate));
	usart_port->BAUDCTRLA = 0xCF; // temp. pre-calc value for full 32MHz osc and 9600 baud

	// Enable serial interrupts if decided so
	if(interrupt_en){
		usart_port->CTRLA |= USART_RXCINTLVL_LO_gc;
		usart_port->CTRLA |= USART_DREINTLVL_MED_gc;
	} else {
		usart_port->CTRLA = 0; // disable for now..
	}

	// 8 data bits, no parity, 1 stop bit
	usart_port->CTRLC = (uint8_t) USART_CHSIZE_8BIT_gc;

	// Enable serial
	usart_port->CTRLB = USART_RXEN_bm | USART_TXEN_bm;

}

void USART::putChar(int c){
	if (c == '\n')
		putChar('\r');

	// Wait for the transmit buffer to be empty
	while (  !(this->usart_port->STATUS & USART_DREIF_bm) );

	// Put our character into the transmit buffer
	this->usart_port->DATA = c;
}

// send string
void USART::putStr(char * str){
	while(*str){
		putChar(*str++);
	}
}

char USART::getChar(){
	while( !(this->usart_port->STATUS & USART_RXCIF_bm) ); //Wait until data has been received.
	char data = this->usart_port->DATA; //Temporarily store received data
	if(data == '\r')
		data = '\n';
	//this->PutChar(data); //Send to console what has been received, so we can see when typing
	return data;
}

void operator delete(void* p){
	// I need this for some reason! :D
}

USART::~USART() {

}

