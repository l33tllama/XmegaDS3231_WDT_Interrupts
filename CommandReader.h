/*
 * CommandReader.h
 *
 *  Created on: Mar 2, 2016
 *      Author: leo
 */

#ifndef COMMANDREADER_H_
#define COMMANDREADER_H_
#include <stdio.h>
#include "DS3231.h"


#define HELP "HELP"
#define SETDT "SETDT"
#define SETALRM "SETALRM"
#define EXIT "EXIT"
#define MAX_INPUT_LEN 64

class CommandReader {
private:
	bool entered_config;
	bool running;
	DS3231 * rtc;
public:
	CommandReader(DS3231 * rtc);
	void setDateTimeInput();
	void setAlarmInput();
	void mainLoop();
	virtual ~CommandReader();
};

#endif /* COMMANDREADER_H_ */
