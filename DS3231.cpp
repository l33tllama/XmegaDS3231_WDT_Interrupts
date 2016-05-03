/*
 * DS3231.cpp
 *
 *  Created on: Feb 16, 2016
 *      Author: leo
 *
 *  @about
 *  This class controls a DS3231 real-time clock via I2C/TWI.
 *  Functions:
 *  - set time
 *  - read time
 *  - set alarm
 *  - reset alarm
 *  - and other relevant functions
 *
 *  It is a subclass of TWI so it inherits all the TWI functions.
 *
 *  TODO:
 *  * set alarm (daily, intervals, once per second)
 *  * move printf's into debug
 *  * interrupt-driven TWI..
 */

#include "DS3231.h"
#include "TWI.h"
#include "time_utils.h"
#include <string.h>
#include <stdio.h>
#include <util/delay.h>

DS3231::DS3231() : TWI() {
	address = 0x00;
	alarmType = disabled;
	alarm1_en = false;
	alarm2_en = false;
}

DS3231::DS3231(TWI_Data * twi_d, uint8_t address) : TWI(twi_d){
	this->address = address;
	alarmType = disabled;
	alarm1_en = false;
	alarm2_en = false;
	//printf("Initial control registers:\n");
	//printControlRegisters();
	//printf("Initial status registers:\n");
	//printStatusRegisters();

	disable32kHzOut();
	readCurrentAlarm1();
	enableAlarm1();

	//printf("Final setup control registers:\n");
	//printControlRegisters();
	//printf("Final setup status registers:\n");
	//printStatusRegisters();
}

// TODO: using interrupts to update system clock internally
DS3231::DS3231(TWI_Data * twi_d, uint8_t address, bool high_update_frequency){

	if(high_update_frequency){
		//TODO: get system time and setup interrupts
		this->address = address;
	} else {
		DS3231(twi_d, address);
	}
	alarm1_en = false;
	alarm2_en = false;
	alarmType = disabled;
}

void DS3231::disable32kHzOut(){

	//printf("Disabling 32kHz out.\n");
	// read current status register
	uint8_t stat_reg = readI2C_Register(address, DS3231_STATUSREG);

	//printf("Updating status register..\n");
	//_delay_ms(8);
	// and with not 32khz output and update
	stat_reg &= ~STATUSREG_EN32kHz;
	writeI2C_Register(address, DS3231_STATUSREG, stat_reg);

	//_delay_ms(8);
	//printf("Reading status reg..\n");

	stat_reg = 0;
	stat_reg = readI2C_Register(address, DS3231_STATUSREG);
	//_delay_ms(4);
	//printf("\nStatus reg val: %d\n", stat_reg);

}

// functions for converting binary <-> binary-coded-decimal
static uint8_t bin2bcd(uint8_t val) { return val + 6 * (val / 10); }
static uint8_t bcd2bin (uint8_t val) { return val - 6 * (val >> 4); }

// Read a single value from a register in DS3231
uint8_t DS3231::readI2C_Register(uint8_t addr, uint8_t reg) {
	uint8_t reg_val = 0;
	//printf("Reading i2c register.\n");
	//_delay_ms(8);
	beginWrite(addr);
	putChar(reg);
	endTransmission();
	//_delay_ms(5);
	//beginRead(addr);
	//_delay_ms(5);
	reg_val = beginReadFirstByte(address);
	//printf("next byte for testing.. %d\n", getChar());
	endTransmission();
	//printf("Read i2c register val: %d\n", reg_val);
	return reg_val;
}

// write a single value to a register in the DS3231
// Best to read it first..
void DS3231::writeI2C_Register(uint8_t addr, uint8_t reg, uint8_t val) {
	//printf("About to write staus reg..\n");
	//_delay_ms(8);
	beginWrite(addr);
	putChar(reg);
	putChar(val);
	endTransmission();
	//printf("Done.\n");
}

// Set the time by writing to the beginning address of the DS3231
// and incrementing until at the end of the time registers.
// Then ensure the oscillator is running (important)
void DS3231::setTime(TIME_t * time){
	beginWrite(address);
	putChar(0x00);
	printf("Setting time.\nOrder:\ns:\t%d\nmin:\t%d\nh:\t%d\nd:\t%d\nmon:\t%d\ny:\t%d\n",
				time->sec, time->min, time->hour,
				time->dom, time->mon, time->year);
	putChar(bin2bcd(time->sec));
	putChar(bin2bcd(time->min));
	putChar(bin2bcd(time->hour));
	putChar(bin2bcd(0x00));
	putChar(bin2bcd(time->dom));
	putChar(bin2bcd(time->mon));
	putChar(bin2bcd(time->year - 2000));
	endTransmission();

	// ensure main oscillator is running!
	// read current OSF (oscillator flag) value (0b10000000)
	uint8_t ctrl_reg = readI2C_Register(address, DS3231_CONTROLREG);
	ctrl_reg &= ~CONTROLREG_EOSC;	// set to NOT 1 (0) (when set to 1 it's not running)

	// update status register, with oscillator set to running
	writeI2C_Register(address, DS3231_CONTROLREG, ctrl_reg);
	printf("Done.\n");
}

TIME_t DS3231::getTime(){
	TIME_t t;

	beginWrite(address);
	putChar(0x00);
	endTransmission();

	t.sec = bcd2bin(beginReadFirstByte(address) & 0x7F);
	t.min = bcd2bin((uint8_t)getChar());
	t.hour = bcd2bin((uint8_t)getChar());
	getChar();
	t.dom = bcd2bin((uint8_t)getChar());
	t.mon = bcd2bin((uint8_t)getChar());
	t.year = bcd2bin((uint16_t)getChar()) + 2000 - 1900;

	endTransmission();

	return t;
}

// reset alarm 1 - usually called when alarm was triggered
void DS3231::resetAlarm1Flag(){
	printf("Resetting alarm 1 flag..\n");
	//printStatusRegisters();
	_delay_ms(8);
	// get current status register
	uint8_t stat_reg = readI2C_Register(address, DS3231_STATUSREG);

	// logical and with not A1F (ensure it's 0)
	stat_reg &= ~STATUSREG_A1F;
	writeI2C_Register(address, DS3231_STATUSREG, stat_reg);
	//printStatusRegisters();
}

// reset alarm 2 - same idea as alarm 1
void DS3231::resetAlarm2Flag(){
	// get current status register
	uint8_t stat_reg = readI2C_Register(address, DS3231_STATUSREG);

	// logical and with not A2F (ensure it's 0)
	stat_reg &= ~STATUSREG_A2F;
	writeI2C_Register(address, DS3231_STATUSREG, stat_reg);
}

// enable alarm 1
void DS3231::enableAlarm1(){
	//printf("Enabling alarm 1..\n");
	//printControlRegisters();
	uint8_t ctrl_reg = readI2C_Register(address, DS3231_CONTROLREG);

	//logical and with A2IE & INTCN
	ctrl_reg |= (CONTROLREG_A1IE | CONTROLREG_INTCN);
	//printf("Writing %d to ctrl reg.. \n", ctrl_reg);
	writeI2C_Register(address, DS3231_CONTROLREG, ctrl_reg);

	//printControlRegisters();
	//ctrl_reg = 0;
	//ctrl_reg = readI2C_Register(address, DS3231_CONTROLREG);
	//printf("Double checking control reg status: %d\n", ctrl_reg);
}

// enable alarm 2
void DS3231::enableAlarm2(){
	uint8_t ctrl_reg = readI2C_Register(address, DS3231_CONTROLREG);

	//logical and with A2IE & INTCN
	ctrl_reg |= (CONTROLREG_A2IE | CONTROLREG_INTCN);
	writeI2C_Register(address, DS3231_CONTROLREG, ctrl_reg);
}

void DS3231::printControlRegisters(){
	uint8_t control_reg;
	const char *ctrl_reg_names[] = {"A1IE", "A2IE", "INTCN", "RS1", "RS2", "CONV", "BBSQW", "EOSC"};

	beginWrite(address);
	putChar(DS3231_CONTROLREG);
	endTransmission();

	//beginRead(address);
	control_reg = beginReadFirstByte(address);
	endTransmission();
	printf("Control Register:\n");

	for(uint8_t i = 0; i < 8; i++){
		printf("%06s", ctrl_reg_names[i]);
	}
	printf("\n");
	for(uint8_t i = 0; i < 8; i++){
		uint8_t reg = control_reg & (1 << i);
		printf("     %d", reg);
	}
	printf("\n");
}

void DS3231::printStatusRegisters(){
	uint8_t status_reg;
	const char *status_reg_names[] = {"A1F", "A2F", "BSY", "EN32kHz", "0", "0", "0", "OSF"};
	beginWrite(address);
	putChar(DS3231_STATUSREG);
	endTransmission();

	//beginRead(address);
	status_reg = beginReadFirstByte(address);
	endTransmission();
	printf("Status Register:\n");

	for(uint8_t i = 0; i < 8; i++){
		printf("%07s", status_reg_names[i]);
	}
	printf("\n");
	for(uint8_t i = 0; i < 8; i++){
		uint8_t reg = status_reg & (1 << i);
		printf("      %d", reg);
	}
	printf("\n");
}


/*
 * Alarm configs
 * * once per second
 * * when seconds match (ie on the nth second?) - may have to increment seconds for an interval
 * * when minutes and seconds match (eg 2m 30s on this hour)
 * * when hours, minutes and seconds match (eg 4:30:00 PM every day)
 * * When date, hours, minutes and seconds match (eg 12th of each month, 3:30:00PM)
 * * when day, hours, minutes and seconds match (eg every Monday, 4:15:00PM)
 */

// TODO: set alarm manually

/* Set daily alarm
 *
 * The DS3231 will alarm when hours, minuts and seconds match.
 * the day/date register has to be 1 << 7 to tell the RTC to ignore it
 */
void DS3231::setDailyAlarm(TIME_t * time){
	uint8_t seconds = bin2bcd(time->sec);
	uint8_t minutes = bin2bcd(time->min);
	uint8_t hours = bin2bcd(time->hour);

	beginWrite(address);
	putChar(DS3231_A1REG);
	putChar(seconds);
	putChar(minutes);
	putChar(hours);
	putChar(_BV(7));
	endTransmission();
}

// disable alarm interval - maybe when changine alarm type
void DS3231::disableAlarm(){
	alarmType = disabled;
}

void DS3231::setAlarmInterval(uint8_t seconds, uint8_t minutes, uint8_t hours, uint8_t days){
	make_dtime(&interval_dt, 0, 0, days, hours, minutes, seconds);
	// TODO: deal with times when alarm was a different type (eg daily)
	alarmType = interval;

	enableAlarm1();
	resetAlarm1Flag();
}

void DS3231::setMinuteAlarm(uint8_t seconds){
	alarmType = minute;
	enableAlarm1();
	resetAlarm1Flag();

	beginWrite(address);
	putChar(DS3231_A1REG);
	putChar(bin2bcd(seconds));
	putChar(_BV(7)); // ignore mins
	putChar(_BV(7)); // ingnore hours
	putChar(_BV(7)); // ignore day/date
	endTransmission();

	_delay_ms(8);
	readCurrentAlarm1();
}

// TODO: thoroughly read Pascal's code and other's code..

// set the next alarm, based on the pre-set interval time
void DS3231::setNextIntervalAlarm(){

	if(alarmType != interval) return;

	resetAlarm1Flag();

	// get current time
	TIME_t * current_tm = NULL;
	(*current_tm) = getTime();
	current_tm->year += 1900;
	TIME_t alarm_tm;

	//printf("Current time (asctime): %s\n", asctime(current_time));
	printf("Current time: %d %d:%d:%d\n", current_tm->dom,
			current_tm->hour, current_tm->min, current_tm->sec);

	//memcpy(alarm_tm, current_tm, sizeof(TIME_t));
	alarm_tm = add_time(current_tm, &interval_dt);

	uint8_t seconds = bin2bcd(alarm_tm.sec);
	uint8_t minutes = bin2bcd(alarm_tm.min);
	uint8_t hours = bin2bcd(alarm_tm.hour);
	uint8_t mday = bin2bcd(alarm_tm.dom);

	if (&alarm_tm == current_tm){
		printf("These two times should not be the same!! :( \n");
	}

	//printf("Current time raw:  %lu\n", rawtime_current);
	//printf("Interval time raw: %lu\n", interval);
	//printf("Next alarm time raw: %lu\n", rawtime_next_alarm);

	printf("Next alarm  : %d %d:%d:%d\n", alarm_tm.dom, alarm_tm.hour,
			alarm_tm.min, alarm_tm.sec);
	/*
	// if minutes match, don't worry about mat
	if(alarm_tm->min == current_tm->min){
		minutes |= _BV(7);
		hours |= _BV(7);
		mday |= _BV(7);
		if(alarm_tm->hour == current_tm->hour){
				hours |= _BV(7);
				mday |= _BV(7);
			if(alarm_tm->dom == current_tm->dom){
				mday |= _BV(7);
			}
		}
	} */

	// could be redundant.. trying anyway

	// TODO: if day is different but hour/min also match..



	// if the next interval is tomorrow, rtc needs to know that day/date is not to be ignored
	if(!(alarm_tm.dom > current_tm->dom)){
		//mday |= _BV(6); //ignore the day/date
	}

	_delay_ms(8);
	// Write alarm time to RTC (starting at alarm 1 address 0x07
	beginWrite(address);
	putChar(DS3231_A1REG);
	putChar(seconds);
	putChar(minutes);
	putChar(hours);
	putChar(mday);
	endTransmission();

	_delay_ms(15);

	readCurrentAlarm1();

}

void DS3231::readCurrentAlarm1(){

	printf("Getting current alarm.. ");
	_delay_ms(8);
	uint8_t seconds = 0;
	uint8_t minutes = 0;
	uint8_t hours = 0;
	uint8_t daydt = 0;

	uint8_t tm_mask = 0b01111111;
	uint8_t tm_ddt_mask = 0b00111111;

	beginWrite(address);
	putChar(DS3231_A1REG);
	endTransmission();

	seconds = bcd2bin((uint8_t)beginReadFirstByte(address)) & tm_mask;
	minutes =  bcd2bin((uint8_t)getChar()) & tm_mask;
	hours = bcd2bin((uint8_t)getChar()) & tm_mask;
	daydt = bcd2bin((uint8_t)getChar()) & tm_mask & tm_ddt_mask;
	endTransmission();

	_delay_ms(8);
	printf("%d %d:%d:%d\n", daydt, hours, minutes, seconds);

}

DS3231::~DS3231() {
	// TODO Auto-generated destructor stub
}
