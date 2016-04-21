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
	uint16_t year_count = 0;
	uint8_t mon_count = 0;
	uint8_t day_count = 0;
	uint8_t hour_count = 0;
	uint8_t sec_count = 0;
	uint64_t seconds_count = 0;

	// redundant?
	uint16_t year = 0;
	uint8_t month = 0;
	uint8_t dom = 0;
	uint8_t hour = 0;
	uint8_t min = 0;
	uint8_t sec = 0;

	while(1){
		seconds_count += (year_count * YEAR_S) + IS_LEAP_YEAR(year_count)? DAY_S : 0;
		if (seconds_count > timestamp){
			break;
		}

		year_count++;
	}
	year = year_count;

	// seconds since beginning of year
	uint16_t mon_d_hr_min_sec = timestamp - (year_count * YEAR_S);

	seconds_count = 0;

	bool leap_year = IS_LEAP_YEAR(year);
	/*
	unsigned long * mon_s_ptr;
	if(

	while(1){
		seconds_count += (mon_count * )
	}
*/


	return 0;
}
