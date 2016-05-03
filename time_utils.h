#ifndef TIME_UTILS_H_
#define TIME_UTILS_H_

#include <stdint.h>
#include <stdbool.h>

#define EPOCH_YR 1970
#define YEAR_S 31536000
#define MON31 2678400
#define MON30 2592000
#define MON29 2505600
#define MON28 2419200
#define IS_LEAP_YEAR(year) (year % 4 == 0)
#define DAY_S 86400
#define HOUR_S 3600
#define MIN_S 60

// enums for day and month names (not needed, yet?)
typedef enum DayOfWeek{
	sunday,
	monday,
	tuesday,
	wednesday,
	thursday,
	friday,
	saturday
} day_of_week;

typedef enum Months{
	January,
	February,
	March,
	April,
	May,
	June,
	July,
	August,
	September,
	October,
	November,
	December
} months;

typedef struct TIME{
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
	day_of_week dow;
	uint16_t dom;
	uint8_t mon;
	uint16_t year;
} TIME_t;

typedef struct TIME_DELTA{
	uint8_t seconds;
	uint8_t mins;
	uint8_t hours;
	uint16_t days;
	uint8_t months;
	uint16_t years;
} TIME_dt;

TIME_t timestamp_to_struct(uint64_t timestamp);
TIME_t timestamp_to_struct_v2(uint64_t timestamp);
TIME_t copy_time(TIME_t * t);
TIME_t add_time(TIME_t * base, TIME_dt * a);

uint64_t make_timestamp(TIME_t * t);

uint64_t add_months(TIME_t * t, uint64_t in, uint8_t months);
uint64_t add_years(TIME_t * t, uint64_t in, uint8_t years);

void make_time(TIME_t * t, uint16_t year, uint8_t month, uint8_t day_of_month, uint8_t hour, uint8_t min, uint8_t sec);
void make_dtime(TIME_dt * dt, uint16_t years, uint8_t months, uint8_t days, uint8_t hours, uint8_t mins, uint8_t secs);


#endif
