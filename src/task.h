#ifndef _ebs_task_h_
#define _ebs_task_h_

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

enum {
  MAX_TASK_NAME = 127
};

enum task_status {
  STATUS_ACTIVE,
  STATUS_DONE,
  MAX_STATUS
};

struct task {
  intmax_t estimated_seconds;
  intmax_t actual_seconds;
  char name[MAX_TASK_NAME + 1];
  enum task_status status;
};

struct time_record {
  char name[MAX_TASK_NAME + 1];
  time_t time;
};

/* Get the name of the task status. */
const char* get_task_status(enum task_status);

/* Format a time_record as a string. */
struct error format_time_record(const struct time_record*, char* buffer, size_t
    max_buffer);

/* Read a time record. */
struct error read_time_record(FILE* fp, struct time_record*);

/* Append an entry with the current time and the task name to the time sheet.
 * */
struct error append_time_sheet_entry(const char* filename, const char*
    task_name);

/* Parse a time sheet entry. */
struct error parse_time_record(const char* str, struct time_record*);

/* Read the time sheet. Add to the actual_secods in tasks.  */
struct error read_time_sheet(const char* filename, struct task* tasks, size_t
    max_task);

/* Parse a task. The format is <task_name> TAB <status> TAB <estimate> TAB
 * <actual>. Return ERROR_TASK_MISSING_FIELDS if some fields are missing.
 * Return ERROR_UNKNOWN_STATUS if the status field is invalid. */
struct error parse_task(const char* str, struct task* result);

/* Format a task and put up to max_buffer bytes into result. The terminating
 * null is included in max_buffer. Return ERROR_BUFFER_LIMIT if the result does
 * not fit the buffer. */
struct error format_task(const struct task*, char* buffer, size_t max_buffer);

/* Get a task from the given stream. */
struct error read_task(FILE* fp, struct task*);

/* Append a task to the stream. */
struct error write_task(const struct task*, FILE* fp);

/* Find task by name. Return null if the task is not found. */
struct task* find_task(const char* name, struct task* tasks, size_t
    max_task);

/* Predict completion date for the filtered, active tasks. Completed tasks are
 * not filtered. Possible errors are ERROR_TIME_UNAVAILABLE and
 * ERROR_INCOMPLETE_TASK if the tasks cannot be completed with the (currently
 * hard-coded) calendar. */
struct error predict_completion_date(const struct task*, const size_t, const
    char* filter); 

#endif
