#include "error.h"
#include "task.h"
#include <math.h>
#include <stdio.h>

enum {
  max_task_length = 1024
};

int
main(int argc, char** argv) {
  if (argc < 2) {
    printf("usage: %s filename..\n", argv[0]);
    return 1;
  }

  const char* const filename = argv[1];

  struct task tasks[max_task_length];
  size_t task_count = 0;
  struct error error;

  error = read_time_sheet(filename, tasks, max_task_length, &task_count);
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
