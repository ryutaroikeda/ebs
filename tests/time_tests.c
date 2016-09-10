#include "calendar.h"
#include "error.h"
#include <assert.h>
#include <stdio.h>
#include <time.h>

enum {
  max_buffer_length = 1023
};

void
test_parser_errors_for_invalid_input(void);

void
test_parser(void);

void
test_is_in_event(void);

void
test_completion_date(void);

/* Test correct error code is returned when parsing invalid string. */
void test_parser_errors_for_invalid_input(void) {
	const char invalid_input[] = "1800-01-03";
	struct tm time;
	struct error error = parse_iso_8601_time(invalid_input, &time);
	assert(ERROR_BAD_TIME_STRING == error.code);
}

/* Test correct time is parsed. */
void
test_parser(void) {
	char date_string[] = "1800-02-3T13:01:31";
	char buffer[max_buffer_length + 1];
	struct tm time;
	struct error error = parse_iso_8601_time(date_string, &time);
	strftime(buffer, max_buffer_length, "%c\n", &time);
	printf("%s", buffer);

	assert(ERROR_NONE == error.code);
	assert(-100 == time.tm_year);
	assert(1 == time.tm_mon);
	assert(3 == time.tm_mday);
	assert(13 == time.tm_hour);
	assert(1 == time.tm_min);
	assert(31 == time.tm_sec);
}

/* Test repeating events work. */
void
test_is_in_event(void) {
	struct event e;
  parse_iso_8601_time("1900-01-01T00:00:00", &e.start);
	e.period.tm_year = 0;
	e.period.tm_mon = 0;
	e.period.tm_mday = 1;
	e.period.tm_hour = 0;
	e.period.tm_min = 0;
	e.period.tm_sec = 0;
	e.repetition = 32;

	struct tm date;
  parse_iso_8601_time("1900-02-01T10:00:00", &date);

	assert(is_in_event(&date, &e));

	date.tm_mday = 11;
	assert(!is_in_event(&date, &e));
}

/* Test completion date calculation. */
void
test_completion_date(void) {
	struct tm start;
	parse_iso_8601_time("2016-09-08T12:12:12", &start);

	struct tm sunday_date;
	parse_iso_8601_time("2016-09-11T12:12:12", &sunday_date);

	struct tm daily_period = {
		.tm_year = 0,
		.tm_mon = 0,
		.tm_mday = 1,
		.tm_hour = 0,
		.tm_min = 0,
		.tm_sec = 0
	};

	struct tm weekly_period = {
		.tm_year = 0,
		.tm_mon = 0,
		.tm_mday = 7,
		.tm_hour = 0,
		.tm_min = 0,
		.tm_sec = 0
	};

	struct event work = {
		.start = start,
		.period = daily_period,
		.repetition = 10
	};

	struct event sunday = {
		.start = sunday_date,
		.period = weekly_period,
		.repetition = 10
	};

	struct calendar calendar;
	init_calendar(&calendar);
	add_inclusion(&work, &calendar);
	add_exclusion(&sunday, &calendar);

	uint64_t work_per_day = 100;
	uint64_t total_work = 400;

	struct tm completion_date;
	struct error error = compute_completion_date(&start, &calendar, work_per_day,
			total_work, &completion_date);

	print_time(&completion_date);
	assert(ERROR_NONE == error.code);
	assert((2016 - 1900) == completion_date.tm_year);
	assert((9 - 1) == completion_date.tm_mon);
	assert(12 == completion_date.tm_mday);
}

int main(void) {
	test_parser_errors_for_invalid_input();
	test_parser();
	test_is_in_event();
	test_completion_date();
	return 0;
}
