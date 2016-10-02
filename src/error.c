#include "error.h"
#include <stdio.h>

/* Print the error message. */
void
print_error(const struct error* const error) {
  switch (error->code) {
    case ERROR_NONE:
      puts("none");
      break;
    case ERROR_FILE:
      puts("file error");
      break;
    case ERROR_TASK_LIMIT:
      puts("task limit exceeded");
      break;
    case ERROR_BAD_TIME_STRING:
      puts("input time overflowed");
      break;
    case ERROR_INCOMPLETE_TASK:
      puts("tasks can't be completed in the given schedule");
      break;
    case ERROR_TIME_RECORD_MISSING_FIELDS:
      puts("time record missing fields");
      break;
    case ERROR_INVALID_TIME:
      puts("time overflowed");
      break;
    case ERROR_TIME_UNAVAILABLE:
      puts("couldn't get current time");
      break;
    case ERROR_END_OF_FILE:
      puts("end of file");
      break;
    case ERROR_BUFFER_LIMIT:
      puts("buffer limit reached");
      break;
    case ERROR_UNKNOWN_STATUS:
      puts("unknown task status");
      break;
    case ERROR_TASK_MISSING_FIELDS:
      puts("task is missing fields");
      break;
    default:
      puts("unknown");
      break;
  }
}
