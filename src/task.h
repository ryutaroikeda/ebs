#ifndef _ebs_task_h_
#define _ebs_task_h_

#include <stddef.h>
#include <stdint.h>
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
  int64_t estimated_seconds;
  int64_t actual_seconds;
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

/* Append an entry with the current time and the task name to the time sheet.
 * */
struct error append_time_sheet_entry(const char* filename, const char*
    task_name);

/* Parse a time sheet entry. */
struct error parse_time_record(const char* str, struct time_record*);

/* Read the time sheet.  */
struct error read_time_sheet(const char* filename, struct time_record*, size_t
    max_record, size_t* records_read);

/* Write the time sheet. Return ERROR_FILE if there's a problem with the file.
 * */
struct error write_time_sheet(const char* filename, const struct time_record*,
    size_t max_record);

/* Parse a task. The format is <task_name> TAB <status> TAB <estimate> TAB
 * <actual>. Return ERROR_TASK_MISSING_FIELDS if some fields are missing.
 * Return ERROR_UNKNOWN_STATUS if the status field is invalid. */
struct error parse_task(const char* str, struct task* result);

/* Format a task and put up to max_buffer bytes into result. The terminating
 * null is included in max_buffer. Return ERROR_BUFFER_LIMIT if the result does
 * not fit the buffer. */
struct error format_task(const struct task*, char* buffer, size_t max_buffer);

/* Read a TSV file of tasks. Read the first max_task_length rows. Return
 * ERROR_FILE if there's a problem opening the file. */
struct error read_task_sheet(const char* filename, struct task*, size_t
    max_task, size_t* tasks_read);

/* Write a TSV file of tasks. */
struct error write_task_sheet(const char* filename, const struct task*, size_t
    max_task);

/* Add up the actual time used for tasks and update the tasks. */
void add_time_record_to_tasks(const struct time_record* records, size_t
    max_record, struct task* tasks, size_t max_task);

/* Predict completion date. */
struct error
predict_completion_date(const struct task*, const size_t);

#endif
