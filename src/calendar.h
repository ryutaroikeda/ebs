#ifndef _ebs_calendar_h_
#define _ebs_calendar_h_

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

enum {
  MAX_EVENT_NAME_LENGTH = 255,
  MAX_CALENDAR_INCLUSIONS_LENGTH = 1023,
  MAX_CALENDAR_EXCLUSIONS_LENGTH = 1023,
  MAX_CALENDAR_DAYS = 100000
};

enum comparison {
  COMPARISON_EQUAL,
  COMPARISON_LESS_THAN,
  COMPARISON_GREATER_THAN
};

extern const struct tm DAY;

extern const struct tm WEEK;

enum day_of_week {
  SUNDAY,
  MONDAY,
  TUESDAY,
  WEDNESDAY,
  THURSDAY,
  FRIDAY,
  SATURDAY
};

struct event {
  struct tm start;
  struct tm end;
  struct tm period;
  uint64_t repetition;
  //char name[max_event_name_length];
};

struct calendar {
  struct event inclusions[MAX_CALENDAR_INCLUSIONS_LENGTH];
  struct event exclusions[MAX_CALENDAR_EXCLUSIONS_LENGTH];
  size_t inclusions_length;
  size_t exclusions_length;
};

struct error
parse_iso_8601_time(const char*, struct tm*);

struct error
add_time(const struct tm*, const struct tm*, struct tm*);

struct error
add_days(const struct tm*, const int, struct tm*);

enum comparison
compare_time(const struct tm*, const struct tm*);

bool
is_same_date(const struct tm*, const struct tm*);

struct error
get_next_week(const struct tm*, const int, struct tm*);

void
print_time(const struct tm*);

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
    int64_t, int64_t, struct tm*);

#endif
