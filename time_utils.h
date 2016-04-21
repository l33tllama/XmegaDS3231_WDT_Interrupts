#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include <stdint.h>
#include <stdbool.h>

#define JANUARY 0
#define FEBRUARY 1
#define MARCH 2
#define APRIL 3
#define MAY 4
#define JUNE 5
#define JULY 6
#define AUGUST 7
#define SEPTEMBER 8
#define OCTOBER 9
#define NOVEMBER 10
#define DECEMBER 11

#define YEAR_S 31536000
#define MON31 2678400
#define MON30 2592000
#define MON29 2505600
#define MON28 2419200
#define IS_LEAP_YEAR(year) year % 4 == 0
#define DAY_S 86400
#define HOUR_S 3600
#define MIN_S 60

const unsigned long seconds_per_month_no_leap[12] = {
	MON31,	MON28,	MON31,	MON30,	MON31,	MON30,
	MON31,	MON31,	MON30,	MON31,	MON30,	MON31
};

const unsigned long seconds_per_month_leap[12] = {
	MON31,	MON29,	MON31,	MON30,	MON31,	MON30,
	MON31,	MON31,	MON30,	MON31,	MON30,	MON31
};

typedef enum DayOfWeek{
	sunday,
	monday,
	tuesday,
	wednesday,
	thursday,
	friday,
	saturday
} day_of_week;

typedef union DayOfWeekUnion{
	uint8_t d;
	day_of_week de;
} day_of_week_u;

typedef struct TIME{
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
	day_of_week_u dow;
	uint16_t dom;
	uint8_t mon;
	uint8_t year;
} TIME_t;

uint64_t make_timestamp(TIME_t * t);
TIME_t * timestamp_to_struct(uint64_t timestamp);

#endif
