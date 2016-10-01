#include "time_sheet.h"
#include "ebs_time.h"
#include "error.h"
#include <assert.h>
#include <stdio.h>
#include <time.h>

enum {
  MAX_BUFFER = 255
};

struct error add_time_sheet_entry(const char* const filename, const char* const
    task_name) {
  assert(NULL != filename);
  assert(NULL != task_name);

  struct error error;
  FILE *fp = fopen(filename, "a");
  if (NULL == fp) {
    error.code = ERROR_FILE;
    return error;
  }
  {
    char time_buffer[MAX_BUFFER + 1];
    time_t now = time(NULL);
    if (-1 == now) {
      error.code = ERROR_TIME_UNAVAILABLE;
      fclose(fp);
      return error;
    }
    // This is not thread-safe.
    const struct tm* structured_now = localtime(&now);
    if (NULL == structured_now) {
      error.code = ERROR_TIME_UNAVAILABLE;
      fclose(fp);
      return error;
    }
    format_iso_8601_time(structured_now, time_buffer, MAX_BUFFER);
    fprintf(fp, "%s\n", time_buffer);
  }
  fprintf(fp, "%s\n", task_name);
  fclose(fp);
  error.code = ERROR_NONE;
  return error;
}
