#include "calendar.h"
#include "error.h"
#include <assert.h>
#include <stdio.h>

enum {
	max_buffer_length = 255
};

/* Parse a string in ISO-8601 format to calendar time. This depends on
 * strptime(). */
struct error
parse_iso_8601_time(const char* const string, struct tm* const result) {
	assert(NULL != string);
	assert(NULL != result);

	char* end = strptime(string, "%Y-%m-%dT%H:%M:%S", result);
	struct error error;
	if (NULL == end) {
		error.code = ERROR_BAD_TIME_STRING;
		return error;
	}

	error.code = ERROR_NONE;
	return error;
}

/* Add two dates and normalize the result. Changes the timezone to the local
 * timezone as a side-effect. */
void
add_time(const struct tm* const first, const struct tm* const second,
		struct tm* const result) {
	assert(NULL != first);
	assert(NULL != second);
	assert(NULL != result);
	result->tm_sec = first->tm_sec + second->tm_sec;
	result->tm_min = first->tm_min + second->tm_min;
	result->tm_hour = first->tm_hour + second->tm_hour;
	result->tm_mday = first->tm_mday + second->tm_mday;
	result->tm_mon = first->tm_mon + second->tm_mon;
	result->tm_year = first->tm_year + second->tm_year;
	mktime(result);
}

void
print_time(const struct tm* time) {
	char buffer[max_buffer_length + 1];
	strftime(buffer, max_buffer_length, "%c", time);
	printf("%s\n", buffer);
}

/* Check that two normalized broken-down times are the same dates. */
bool
is_same_date(const struct tm* const first, const struct tm* const second) {
	return (first->tm_year == second->tm_year) &&
		(first->tm_mon == second->tm_mon) && (first->tm_mday == second->tm_mday);
}

/* Determine if the given day is included in the repeating event.
 * The date must be normalized. */
bool
is_in_event(const struct tm* date, const struct event* const event) {
	assert(NULL != date);
	assert(NULL != event);

	struct tm iterating_date = event->start;
	uint64_t repetition;

	for (repetition = 0; repetition < event->repetition;
			repetition++) {
		if (is_same_date(date, &iterating_date)) {
			return true;
		}
		add_time(&iterating_date, &event->period, &iterating_date);
	}
	return false;
}

/* Initialize a calendar. */
void
init_calendar(struct calendar* calendar) {
	assert(NULL != calendar);
	calendar->inclusions_length = 0;
	calendar->exclusions_length = 0;
}

/* Add a work day rule. */
void
add_inclusion(const struct event* const inclusion,
		struct calendar* const calendar) {
	assert(NULL != inclusion);
	assert(NULL != calendar);

	calendar->inclusions[calendar->inclusions_length] = *inclusion;
	calendar->inclusions_length += 1;
}

/* Add a work day rule. */
void
add_exclusion(const struct event* const exclusions,
		struct calendar* const calendar) {
	assert(NULL != exclusions);
	assert(NULL != calendar);

	calendar->exclusions[calendar->exclusions_length] = *exclusions;
	calendar->exclusions_length += 1;
}

/* Compute the calendar time when the task will be completed. */
struct error
compute_completion_date(const struct tm* const start,
	const struct calendar* const calendar,
	const uint64_t seconds_of_work_per_day, const uint64_t seconds_to_work,
	struct tm* const completion_date) {
	assert(NULL != start);
	assert(NULL != calendar);
	assert(NULL != completion_date);

	const struct tm one_day = {
		.tm_year = 0,
		.tm_mon = 0,
		.tm_mday = 1,
		.tm_hour = 0,
		.tm_min = 0,
		.tm_sec = 0
	};

	*completion_date = *start;

	uint64_t seconds_worked = 0;
	const uint64_t max_days = 100000;
	uint64_t day;
	for (day = 0; day < max_days; day++) {
		/* Check if it's normally a work day. */
		bool is_normal_work_day = false;
		size_t inclusion_index;
		for (inclusion_index = 0; inclusion_index < calendar->inclusions_length;
				inclusion_index++) {
			if (is_in_event(completion_date,
						&calendar->inclusions[inclusion_index])) {
				is_normal_work_day = true;
				break;
			}
		}
		if (!is_normal_work_day) {
			continue;
		}

		/* Check if it's really a work day. */
		bool is_actual_work_day = true;
		size_t exclusion_index;
		for (exclusion_index = 0; exclusion_index < calendar->exclusions_length;
				exclusion_index++) {
			if (is_in_event(completion_date,
						&calendar->exclusions[exclusion_index])) {
				is_actual_work_day = false;
				break;
			}
		}
		if (is_actual_work_day) {
			seconds_worked += seconds_of_work_per_day;
		}

		if (seconds_to_work <= seconds_worked) {
			break;
		}
		add_time(completion_date, &one_day, completion_date);
	}

	struct error error;
	if (seconds_worked < seconds_to_work) {
		error.code = ERROR_INCOMPLETE_TASK;
		return error;
	}
	error.code = ERROR_NONE;
	return error;
}