#include "time_utils.h"

/* uint64_t make_timestamp(TIME_t * t)
 *
 * Takes a TIME_t data struct (date and time) and converts it into
 * seconds since Jan 1 1970.
 *
 */

uint64_t make_timestamp(TIME_t * t){

	// final return value
	uint64_t time_since_epoch = 0;

	// convert year to years since Jan 1 1970
	uint16_t year = t->year - 1970;

	// Add seconds for this year since Jan 1 1970
	time_since_epoch += (year) + IS_LEAP_YEAR(year)? DAY_S : 0;

	// Accumulatively add seconds per month since beginning of the year
	for(uint8_t i = 0; i < t->mon + 1; i++){
		time_since_epoch += IS_LEAP_YEAR(year)?
				seconds_per_month_leap[i] : seconds_per_month_no_leap[i];
	}

	// add seconds for each day per month
	time_since_epoch += t->dom * DAY_S;

	// add seconds per hour in the day
	time_since_epoch += t->hour * HOUR_S;

	// add seconds per minute in the hour
	time_since_epoch += t->min * MIN_S;

	// add final seconds per minute
	time_since_epoch += t->sec;

	return time_since_epoch;
}

TIME_t * timestamp_to_struct(uint64_t timestamp){
	TIME_t * t;

	uint16_t year = 0;
	uint8_t month = 0;
	uint8_t day_of_month = 0;
	uint8_t hour = 0;
	uint8_t minute = 0;
	uint8_t seconds = 0;

	uint64_t seconds_count = 0;
	uint64_t last_seconds_count = 0;

	// Get seconds since beginning of the year (and find the current year)
	while(1){
		seconds_count += (year * YEAR_S) + IS_LEAP_YEAR(year)? DAY_S : 0;
		if (seconds_count > timestamp){
			break;
		}
		last_seconds_count = seconds_count;
		year++;
	}

	// seconds since beginning of year
	uint16_t seconds_since_year_begin = timestamp - last_seconds_count;


	// Get seconds since the beginning of the month (and find current month)
	unsigned long * mon_ptr = IS_LEAP_YEAR(year) ?  &seconds_per_month_no_leap :
			&seconds_per_month_leap;

	seconds_count = 0;
	last_seconds_count = 0;

	while(1){
		seconds_count += (month * (*mon_ptr)++);
		if(seconds_count > timestamp){
			break;
		}
		month++;
		last_seconds_count = seconds_count;
	}

	uint16_t seconds_since_month_begin = timestamp - (seconds_since_year_begin + last_seconds_count);

	// Get seconds since the start of the day (and find day of month)
	seconds_count = 0;
	last_seconds_count = 0;

	while(1){
		seconds_count += (day_of_month * DAY_S);
		if(seconds_count > timestamp){
			break;
		}
		day_of_month++;
		last_seconds_count = seconds_count;
	}

	uint16_t seconds_since_day_begin = timestamp - (seconds_since_year_begin
			+ seconds_since_month_begin + last_seconds_count);

	// Get seconds since the start of the current hour (and find current hour)
	seconds_count = 0;
	last_seconds_count = 0;

	while(1){
		seconds_count += (hour * HOUR_S);
		if(seconds_count > timestamp){
			break;
		}
		hour++;
		last_seconds_count = seconds_count;
	}

	uint16_t seconds_since_hour_begin = timestamp - (seconds_since_year_begin
			+ seconds_since_month_begin + seconds_since_day_begin);

	if(seconds_since_hour_begin > HOUR_S){
		printf("Uh oh.. too many seconds in this hour.. %d\n", seconds_since_hour_begin);
	}

	// Get seconds since the start of the current minute (and find the current minute)
	while(1){
		seconds_count += (minute * MIN_S);
		if(seconds_count > timestamp){
			break;
		}
		minute++;
		last_seconds_count = seconds_count;
	}

	uint16_t seconds_since_minute_begin = timestamp - (seconds_since_year_begin
			+ seconds_since_month_begin + seconds_since_day_begin
			+ seconds_since_hour_begin);

	if(seconds_since_minute_begin > 59) {
		printf("Uh oh... too many seconds this minute..");
		seconds = 59;
	} else {
		seconds = seconds_since_minute_begin;
	}
	t->year = year;
	t->mon = month;
	t->dom = day_of_month;
	t->hour = hour;
	t->min = minute;
	t->sec = seconds;


	return t;
}


