#include "error.h"
#include "task.h"
#include <math.h>
#include <stdio.h>

enum {
  MAX_TASK = 1024,
  MAX_RECORD = 1024
};

int
main(int argc, char** argv) {
  if (argc < 3) {
    printf("usage: %s <time_sheet> <task_sheet>\n", argv[0]);
    return 1;
  }

  const char* const time_sheet = argv[1];
  const char* const task_sheet = argv[2];
  const char* const stdout_filename = "/dev/stdout";

  struct task tasks[MAX_TASK];
  struct time_record records[MAX_RECORD];
  size_t task_count;
  size_t record_count;
  struct error error;

  error = read_task_sheet(task_sheet, tasks, MAX_TASK, &task_count);
  if (ERROR_NONE != error.code) {
    print_error(&error);
    return 1;
  }

  error = read_time_sheet(time_sheet, records, MAX_RECORD, &record_count);
  if (ERROR_NONE != error.code) {
    print_error(&error);
    return 1;
  }

  add_time_record_to_tasks(records, record_count, tasks, task_count);
  error = write_task_sheet(stdout_filename, tasks, task_count);
  if (ERROR_NONE != error.code) {
    print_error(&error);
    return 1;
  }

  error = predict_completion_date(tasks, task_count);
  if (ERROR_NONE != error.code) {
    print_error(&error);
    return 1;
  }

  return 0;
}
