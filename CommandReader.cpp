/*
 * CommandReader.cpp
 *
 *  Created on: Mar 2, 2016
 *      Author: leo
 */

#include "CommandReader.h"
#include "USART_Debug.h"
#include <string.h>

CommandReader::CommandReader(DS3231 * rtc) {
	entered_config = false;
	running = false;
	this->rtc = rtc;
}

// sanity check before doing atoi (which can cause 'undefined behavior' if the chars aren't numbers..)
bool sanity_check_int(char * str, uint8_t len){
	for(uint8_t i = 0; i < len; i++){
		if((int)str[i] < 48 || (int)str[i] > 57){
			return false;
		}
	}
	return true;
}

// sanity check for alarm numbers - including check for asterisks
bool sanity_check_alrm(char * str, uint8_t len){
	for(uint8_t i = 0; i < len; i++){
		// check for asterisks
		if(str[i] == '*' && i == 0){
			for(int j = 0; j < len; j++){
				if(str[j] != '*'){
					return false;
				}
			}
		}
		else if((int)str[i] < 48 || (int)str[i] > 57){
			return false;
		}
	}
	return true;
}

bool str_is_astrisks(char * str, int len){
	for(int i = 0; i < len; i++){
		if(str[i] != '*'){
			return false;
		}
	}
	return true;
}

// safe input with fgets.
// Made easier with a helper function that replaces a new line char ('\n') with end of string char ('/0')
static void fgetstr(char * dest, uint16_t len){
	fgets(dest, len, stdin);
	dest[strlen(dest) - 1] = '\0';
}
// same as above but with default max length
static void fgetstr(char * dest){
	fgetstr(dest, MAX_INPUT_LEN);
}


// Get time and date from serial console from user and set the time and date using the RTC library
void CommandReader::setDateTimeInput(){
	// 00/00/0000 00:00:00
	printf("Enter date (format: YYYY/MM/DD HH:mm:SS\n");
	char datetime[64];
	scanf("%s %s", datetime, datetime+11);
	datetime[10] = ' ';

	// if entered the correct amoun of digits including separating characters
	if(strlen(datetime) == 19){
		struct tm time;
		// split string into relevant sections using strtok
		char * yyyy, * mm, * dd, * hh, * m, * ss;
		yyyy = strtok(datetime, "/");
		mm = strtok(NULL, "/");
		dd = strtok(NULL, " ");
		hh = strtok(NULL, ":");
		m = strtok(NULL, ":");
		ss = strtok(NULL, "");

		// sanity check strings to make sure they're ints (see if each char is an ASCII number)
		bool dd_s, mm_s, yyyy_s, hh_s, m_s, ss_s;
		dd_s = sanity_check_int(dd, 2);
		mm_s = sanity_check_int(mm, 2);
		yyyy_s = sanity_check_int(yyyy, 4);
		hh_s = sanity_check_int(hh, 2);
		m_s = sanity_check_int(m, 2);
		ss_s = sanity_check_int(ss, 2);

		// If all are good, set time?
		if(dd_s && mm_s && yyyy_s && hh_s && m_s && ss_s){

			printf("Date and time probably entered correctly. Setting to: \n");
			uint8_t dd_i = atoi(dd);
			uint8_t mm_i = atoi(mm);
			uint16_t yyyy_i = atoi(yyyy);
			uint8_t hh_i = atoi(hh);
			uint8_t m_i = atoi(m);
			uint8_t ss_i = atoi(ss);
			printf("Interpreted ints: \n");
			printf("%d/%d/%d ", dd_i, mm_i, yyyy_i);
			printf("%d:%d:%d\n", hh_i, m_i, ss_i);
			time.tm_mday = dd_i;
			time.tm_mon = mm_i;
			time.tm_year = yyyy_i;
			time.tm_hour = hh_i;
			time.tm_min = m_i;

			rtc->setTime(&time);
			printf("Time set.\n");

		} else {
			printf("You didn't enter a number correctly: ");
			printf("d:%s m:%s y:%s ", dd, mm, yyyy);
			printf("h:%s m:%s s:%s\n", hh, m, ss);
		}
	} else {
		printf("Improper datetime format. Not exectly 19 characters. (was: %d characters and : %s) \n", strlen(datetime), datetime);
	}
}

// Read alarm time from user
// User can set alarm types - daily, interval, once-off, every second
// Then parse input using strtok to split string into sections using delimters such as ':' and ' '

// example inputs:
// o DD:MM:YY HH:MM:SS
// i DD HH:MM:SS
// s
// d HH:MM:SS
void CommandReader::setAlarmInput(){

	char * hh, * mm, * ss;
	bool hh_s, mm_s, ss_s;
	char * alarmInput = (char*) malloc(MAX_INPUT_LEN);

	printf("Enter alarm:\n");

	fgetstr(alarmInput);

	printf("You entered: %s\n", alarmInput);

	// get the alarm type by char (d, i, o, s)
	char alarmType = alarmInput[0];

	// daily alarm (d)
	if(alarmType == 'd'){

		// move pointer to the start of the daily alarm time
		alarmInput = alarmInput + 2;

		// If incorrect format - should be DD HH:MM:SS
		if(strlen(alarmInput) != 11){
			printf("Error! Daily alarm format incorrect. Please enter the format HH:MM:SS\n");
			return;
		}

		hh = strtok(alarmInput, ":");
		mm = strtok(NULL, ":");
		ss = strtok(NULL, "");

		hh_s = sanity_check_int(hh, 2);
		mm_s = sanity_check_int(mm, 2);
		ss_s = sanity_check_int(ss, 2);

		if(hh_s && mm_s && ss_s){

			uint8_t hh_i = atoi(hh);
			uint8_t mm_i = atoi(mm);
			uint8_t ss_i = atoi(ss);
			printf("Setting daily alarm for: %d:%d:%d\n", hh_i, mm_i, ss_i);
		}


	} /* interval alarm */
	else if(alarmType == 'i'){

		// move the poiner to the start of the alarm interval time
		alarmInput = alarmInput + 2;

		if(strlen(alarmInput) != 11){
			printf("Error! Alarm interval format incorrect. Please enter the format DD HH:MM:SS\n");
			return;
		}

		char * dd;
		bool dd_s = false;

		dd = strtok(alarmInput, " ");
		hh = strtok(NULL, ":");
		mm = strtok(NULL, ":");
		ss = strtok(NULL, "");

		printf("Read interval: \n");
		printf("Days:  %s\n", dd);
		printf("Hours: %s\n", hh);
		printf("Mins:  %s\n", mm);
		printf("Secs:  %s\n", ss);

		hh_s = sanity_check_int(hh, 2);
		mm_s = sanity_check_int(mm, 2);
		ss_s = sanity_check_int(ss, 2);
		dd_s = sanity_check_int(dd, 2);

		if(hh_s && mm_s && ss_s && dd_s){
			uint8_t hh_i = atoi(hh);
			uint8_t mm_i = atoi(mm);
			uint8_t ss_i = atoi(ss);
			uint8_t dd_i = atoi(dd);

			printf("Setting alarm interval for every: %d days, %d:%d:%d\n", dd_i, hh_i, mm_i, ss_i);
			rtc->setAlarmInterval(ss_i, mm_i, hh_i, dd_i);
			rtc->setNextIntervalAlarm();
		} else {
			printf("Error parsing alarm interval values.\n");
		}
	} /* once-off alarm */
	else if (alarmType == 'o'){

	}
	 /* alarm every second */
	else if (alarmType == 's'){

	} /* alarm on x second every minute (mostly just testing) */
	else if (alarmType == 'm'){
		alarmInput = alarmInput + 2;
		if(strlen(alarmInput) > 2){
			printf("Error! please enter 2 or 1 digits for seconds.\n");
		}

		if(sanity_check_int(alarmInput, 2)){
			uint8_t ss_i = atoi(alarmInput);

			rtc->setMinuteAlarm(ss_i);
		}


	} else {
		printf("Alarm type: %c not recognised. Please try again.\n", alarmType);
	}


	/*
	printf("Enter alarm format [i/d/s/o] DD HH:MM:SS with asterisks to ");
	printf("Enter alarm format [W/M] DD HH:MM:SS with asterisks to denote repeats.\n");
	printf("(WM - weekday / day of month)\n");
	printf("eg. * ** 12:00:00 - alarm at 12PM each day.\n");
	printf("eg. W 01 00:00:00 - alarm at 12AM each Monday.\n");
	printf("eg. * ** **:10:00 - alarm every 10 mins.\n");
	char datetime[64];
	scanf("%s %s %s", datetime, datetime+2, datetime+5);
	datetime[1] = ' ';
	datetime[4] = ' ';

	// If length was correct
	if(strlen(datetime) == 13){
		struct tm time;
		// split string into relevant sections using strtok
		char * wm, * dd, * hh, * mm, * ss;
		wm = strtok(datetime, " ");
		dd = strtok(NULL, " ");
		hh = strtok(NULL, ":");
		mm = strtok(NULL, ":");
		ss = strtok(NULL, "");

		// sanity check strings to make sure they're ints (see if each char is an ASCII number)
		bool dd_s, mm_s, wm_s, hh_s, ss_s;
		wm_s = false;
		dd_s = sanity_check_alrm(dd, 2);
		mm_s = sanity_check_alrm(mm, 2);
		// simple sanity check of Wd/Md char
		wm_s = (*wm == 'W' || *wm == 'w' || *wm == 'M' || *wm == 'm') ? true : false;
		hh_s = sanity_check_alrm(hh, 2);
		mm_s = sanity_check_alrm(mm, 2);
		ss_s = sanity_check_alrm(ss, 2);

		if(dd_s && mm_s && wm_s && hh_s && ss_s){
			WMDay wmds = weekDay;
			uint8_t dd_i = 0;
			uint8_t mm_i = 0;
			uint8_t hh_i = 0;
			uint8_t ss_i = 0;
			if (*wm == 'M' || *wm == 'm'){
				wmds = dayOfMonth;
			}
			printf("Date and time probably entered correctly. Setting to: \n");

			if(!str_is_astrisks(dd, 2)){
				dd_i = atoi(dd);
			}
			if(!str_is_astrisks(mm, 2)){
				mm_i = atoi(mm);
			}
			if(!str_is_astrisks(hh, 2)){
				hh_i = atoi(hh);
			}
			if(!str_is_astrisks(ss, 2)){
				ss_i = atoi(ss);
			}
			printf("Interpreted ints: \n");
			printf("%s %d ", wm, dd_i);
			printf("%d:%d:%d\n", hh_i, mm_i, ss_i);
			time.tm_mday = dd_i;
			time.tm_mon = mm_i;
			time.tm_hour = hh_i;

			rtc->setAlarmInterval(&time, wmds);
			printf("Alarm set.\n");
		}
	} else {
		printf("Error - Alarm interval entered incorrectly. Not exactly 13 characters (was %d characters and %s)\n", strlen(datetime), datetime);
	}

	//rtc->setAlarmInterval(

	//TODO: similar to set time, but read asterisks
	//TODO: maybe only update time values that don't have asterisks in other function?
	 *
	 * */
}

//TODO: timeout
void CommandReader::mainLoop(){
	running = true;
	entered_config = false;

	while(running){
		char enter[MAX_INPUT_LEN];

		if(!entered_config){
			printf("Enter config? [y/N]\n");
			fgetstr(enter);

			if(strcmp(enter, "y") == 0 || strcmp(enter, "Y") == 0){
				entered_config = true;
			}
		}
		if(entered_config){
			char cmd[10];
			printf("Enter command (type HELP for a list)\n");

			fgetstr(cmd, 10);

			if(strcmp(cmd, HELP) == 0){
				printf("HELP - this shows help (current option)\n");
				printf("SETDT - set clock time and date\n");
				printf("SETALRM - set wake up interval \n");
				printf("EXIT - exit command interface \n");
			} else if (strcmp(cmd, SETDT) == 0){
				setDateTimeInput();
			} else if (strcmp(cmd, SETALRM) == 0){
				setAlarmInput();
			} else if(strcmp(cmd, EXIT) == 0){
				running = false;
			} else {
				printf("Unrecognised command: %s, try again.\n", cmd);
			}
		} else {
			running = false;
		}
	}
}

CommandReader::~CommandReader() {
	// TODO Auto-generated destructor stub
}

