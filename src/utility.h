#ifndef _ebs_utility_h_
#define _ebs_utility_h_

#include <stddef.h>
#include <stdio.h>

struct tm;

/* Return a random number between min and max inclusive. */
int random_from_range(int min, int max);

/* Parse a string in ISO-8601 format to calendar time. Change the timezone to
 * the local timezone as a side-effect. If the time is not within a valid range
 * (1902 to 2038 on some environments), ERROR_INVALID_TIME is returned. */ 
struct error
parse_iso_8601_time(const char* string, struct tm* result);

/* Format time in ISO-8601 writing up to max_result including the terminating
 * null.
 * @param time The time to format.
 * @param result The result buffer.
 * @param max_result The size of the result buffer.
 * @return The errors. */
struct error
format_iso_8601_time(const struct tm* time, char* result, size_t max_result);

/* Read up to and excluding the next new line character or the end of file.
 * Store up to max_buffer bytes including the terminating NULL in the buffer.
 * bytes_read is set to the number of bytes read. If the end of file is reached
 * or a file error occurs, ERROR_END_OF_FILE is returned. If the end of the
 * buffer is reached before the next new line, ERROR_BUFFER_LIMIT is returned.
 * */
struct error get_line(FILE* fp, char* buffer, size_t max_buffer, size_t*
    bytes_read);

/* Copy a file. This is used for backing up files. */
struct error copy(const char* src, const char* dst);

#endif
