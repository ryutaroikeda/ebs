#include "utility.h"
#include "error.h"
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

int random_from_range(const int min, const int max) {
  assert(min <= max);
  const unsigned int bucket_size = (unsigned int) (max - min + 1L);
  assert(bucket_size <= RAND_MAX + 1L);
  const int r = rand();
  return (int) ((unsigned int) r % bucket_size) + min;
}

struct error parse_iso_8601_time(const char* const string, struct tm* const
    result) {
  assert(NULL != string);
  assert(NULL != result);

  struct error error;

  // strptime is not always available, so avoid it.
  const int expected_match_count = 6;
  int match_count = sscanf(string, "%d-%2d-%2dT%2d:%2d:%2d",
      &result->tm_year, &result->tm_mon, &result->tm_mday, &result->tm_hour,
      &result->tm_min, &result->tm_sec);

  if (expected_match_count != match_count) {
    error.code = ERROR_BAD_TIME_STRING;
    return error;
  }

  result->tm_year -= 1900;
  result->tm_mon -= 1;
  result->tm_yday = 0;
  result->tm_wday = 0;
  result->tm_isdst = -1;

  /* Try to normalize the time. */
  if (((time_t) -1) == mktime(result)) {
    error.code = ERROR_INVALID_TIME;
    return error;
  }

  error.code = ERROR_NONE;
  return error;
}

struct error format_iso_8601_time(const struct tm* const time, char* const
    result, const size_t max_result) {
  assert(NULL != time);
  assert(NULL != result);

  struct error error;

  // strftime does not always allow width specifications, so avoid it.
  int bytes_num = snprintf(result, max_result, "%.4d-%.2d-%.2dT%.2d:%.2d:%.2d",
      time->tm_year + 1900, time->tm_mon + 1, time->tm_mday, time->tm_hour,
      time->tm_min, time->tm_sec);

  if (bytes_num < 0) {
    // This is probably an error.
    error.code = ERROR_INVALID_TIME;
    return error;
  }

  error.code = ERROR_NONE;
  return error;
}

struct error get_line(FILE* const fp, char* const buffer, const size_t
    max_buffer, size_t* const bytes_read) {
  assert(NULL != fp);
  assert(NULL != buffer);
  assert(NULL != bytes_read);

  struct error error;
  error.code = ERROR_NONE;
  *bytes_read = 0;

  while (true) {
    int c = fgetc(fp);
    if (EOF == c) {
      error.code = ERROR_END_OF_FILE;
      break;
    }
    if ('\n' == c) {
      break;
    }
    if (*bytes_read < max_buffer - 1) {
      buffer[*bytes_read] = (char) c;
      *bytes_read += 1;
    } else {
      error.code = ERROR_BUFFER_LIMIT;
    }
  }

  buffer[*bytes_read] = '\0';
  return error;
}
