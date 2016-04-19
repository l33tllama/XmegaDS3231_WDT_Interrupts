/*
 * DS3231.h
 *
 *  Created on: Feb 16, 2016
 *      Author: leo
 */

#ifndef DS3231_H_
#define DS3231_H_

#include "time.h"
#include "TWI.h"

/* DS3231 Real-time clock library
 * By Leo Febey
 *
 * Default functionality:
 * - Get time data from RTC
 * - Convert into struct tm for convenience
 *
 * Planned functionality:
 * - Set system time from RTC timestamp
 * - Using a 1Hz interrupt, keep the system clock ticking
 * - Then user can call the time independent from the RTC
 *  - For frequent time reading - lower power consumption as we don't need to use I2C bus to get time from RTC, then parse it
 *  - For less frequent time reading (eg every 60+ mins) probably better to use I2C so no system tick required
 *
 *
 *	useful reference https://github.com/vancegroup-mirrors/avr-libc/blob/master/avr-libc/include/time.h
 *
 */
#define DS3231_A1REG 0x07
#define DS3231_A2REG 0x0B
#define DS3231_CONTROLREG 0x0E
#define CONTROLREG_A1IE 	(1 << 0)
#define CONTROLREG_A2IE 	(1 << 1)
#define CONTROLREG_INTCN 	(1 << 2)
#define CONTROLREG_RATESEL	(1 << 3)
#define CONTROLREG_CONVTMP1	(1 << 4)
#define CONTROLREG_CONVTMP2 (1 << 5)
#define CONTROLREG_BBSQWV	(1 << 6)
#define CONTROLREG_EOSC 	(1 << 7)
#define DS3231_STATUSREG 0x0F
#define STATUSREG_A1F 0x01
#define STATUSREG_A2F 0x02
#define STATUSREG_EN32kHz 0x08

enum WMDay { weekDay, dayOfMonth };
enum AlarmType { disabled, seconds, interval, daily, minute, once_off };

typedef struct AlarmIntervalData2{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t days;
	WMDay wm;
} alarmData;

class DS3231 : public TWI{
private:
	time_t system_time;
	struct tm sys_time_strc;
	alarmData alarm_data;
	AlarmType alarmType;
	bool alarm1_en;
	bool alarm2_en;
	uint8_t address;
	void printControlRegisters();
	void printStatusRegisters();



//TODO: change get/set time to pointers
public:
	DS3231();
	DS3231(TWI_Data * twi_data, uint8_t address);
	DS3231(TWI_Data * twi_data, uint8_t address, bool high_update_frequency);
	uint8_t readI2C_Register(uint8_t addr, uint8_t reg);
		void writeI2C_Register(uint8_t addr, uint8_t reg, uint8_t val);
	void disable32kHzOut();
	void setTime(struct tm * time);
	struct tm * getTime();
	void resetAlarm1Flag();
	void resetAlarm2Flag();
	void setDailyAlarm(struct tm * time);
	void setMinuteAlarm(uint8_t seconds);
	void setAlarmInterval(struct tm * time, WMDay wm);
	void readCurrentAlarm1();
	void enableAlarm1();
	void enableAlarm2();
	void disableAlarm();
	void setNextIntervalAlarm();
	void setAlarmInterval(uint8_t seconds, uint8_t minutes, uint8_t hours, uint8_t days);
	void setAlarmDate(struct tm * time);
	virtual ~DS3231();
};

#endif /* DS3231_H_ */
