/*
 * /home/leo/Applications/arduino-1.6.7/hardware/tools/avr/bin
 * /home/leo/Applications/arduino-1.6.7/hardware/tools/avr/avr/include
 *
 */

#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
//#include <avr/wdt.h>
#include <util/delay.h>

#include "main.h"
#include "USART.h"
#include "USART_Debug.h"
#include "DS3231.h"
#include "CommandReader.h"

volatile uint8_t interrupt_status = 0x00;

USART_Data uc0d;
USART UC0;
TWI_Data twi_d;
DS3231 rtc;
CommandReader cmdReader(&rtc);

static inline void setup_usart_c0(){
	uc0d.baudRate = 9600;
	uc0d.port = &PORTC;
	uc0d.usart_port = &USARTC0;
	uc0d.rxPin = PIN2_bm;
	uc0d.txPin = PIN3_bm;

	UC0 = USART(&uc0d, false);
}

static inline void setup_twi_rtc(uint8_t rtc_address){
	PORTE.DIR = 0x03; // might be needed for I2C to work on pins?
	twi_d.baud_hz = 400000L;
	twi_d.master_addr = 0x00;
	twi_d.maxDataLength = 64;
	twi_d.port = &PORTE;
	twi_d.twi_port = &TWIE;

	rtc = DS3231(&twi_d, rtc_address);
}

int main(){

	init_clocks();						// set clock to internal @ 32MHz
	//wdt_enable(WDTO_1S);
	restart_interrupts();				/* make sure interrupts are reset */
	//wdt_reset();
	setup_porta_interrupts();			/* enable interrupts on port a (falling) */
	//wdt_reset();
	setup_usart_c0();					// start serial comms on USART port C0
	//wdt_reset();
	set_debug_output_port(&USARTC0);	// make printf and scanf redirect to serial port C0
	//wdt_reset();
	setup_twi_rtc(DS3231_ADDR);
	//wdt_reset();

	//wdt_disable();
	printf("\nPROGRAM BEGIN\n\n");
	cmdReader.mainLoop();

	// enable testing LEDs
	PORTB.DIR  = 0b1111;

	// go to sleep... (change pin to #2? - only asynchronous pin?)
	printf("Going to sleep..\n");
	set_sleep_mode(SLEEP_MODE_IDLE);
	sleep_enable();
	sleep_mode();
	printf("Woke up from sleep for first time.\n");

	// main loop..
	while(1){
		if((interrupt_status & ALARM_FLAG) == ALARM_FLAG){
			printf("Alarm triggered!\n");
			//wdt_enable(WDTO_2S);
			//wdt_reset();
			//printf("Setting next alarm.\n");
			rtc.setNextIntervalAlarm();
			interrupt_status &= ~ALARM_FLAG;	//clear alarm flag
			printf("Interrupt status reg reset.\n");
			//wdt_disable();
			sleep_enable();
			sleep_mode();
		}
		printf(".");
		/*
		else {
			printf("We woke up for some reason that wasn't due to port a pin 0..\n");
		} */
	}

}

// DS3231 alarm interrupt pin
ISR(PORTA_INT0_vect){
	sleep_disable();
	interrupt_status |= ALARM_FLAG;
	PORTB.OUTTGL = PIN3_bm;
}
