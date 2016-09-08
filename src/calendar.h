#ifndef _ebs_calendar_h_
#define _ebs_calendar_h_

#define _XOPEN_SOURCE
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

enum {
	max_event_name_length = 255,
	max_calendar_inclusions_length = 1023,
	max_calendar_exclusions_length = 1023
};

enum rule_type {
	include = 0,
	exclude
};

struct event {
	struct tm start;
	struct tm period;
	uint64_t repetition;
	//char name[max_event_name_length];
};

struct calendar {
	struct event inclusions[max_calendar_inclusions_length];
	struct event exclusions[max_calendar_exclusions_length];
	size_t inclusions_length;
	size_t exclusions_length;
};

void
add_time(const struct tm*, const struct tm*, struct tm*);

void
print_time(const struct tm*);

struct error
parse_iso_8601_time(const char*, struct tm*);

bool
is_same_date(const struct tm*, const struct tm*);

bool
is_in_event(const struct tm*, const struct event*);

void
init_calendar(struct calendar* calendar);

void
add_inclusion(const struct event*, struct calendar*);

void
add_exclusion(const struct event*, struct calendar*);

struct error
compute_completion_date(const struct tm*, const struct calendar*,
		uint64_t, uint64_t, struct tm*);

#endif
