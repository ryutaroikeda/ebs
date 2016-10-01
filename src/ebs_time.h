#ifndef _ebs_ebs_time_h_
#define _ebs_ebs_time_h_

#include <stddef.h>

struct tm;

/* Parse a string in ISO-8601 format to calendar time. Change the timezone to
 * the local timezone as a side-effect. If the time is not within a valid range
 * (1902 to 2038 on some environments), ERROR_INVALID_TIME is returned. */ 
struct error
parse_iso_8601_time(const char* string, struct tm* result);

/* Format time in ISO-8601.
 * @param time The time to format.
 * @param result The result buffer.
 * @param max_result The size of the result buffer.
 * @return The errors. */
struct error
format_iso_8601_time(const struct tm* time, char* result, size_t max_result);

#endif
