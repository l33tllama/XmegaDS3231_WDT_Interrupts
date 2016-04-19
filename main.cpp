#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

#include "main.h"
#include "USART.h"
#include "USART_Debug.h"
#include "DS3231.h"

int main(){

	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();
	sleep_cpu();

}
