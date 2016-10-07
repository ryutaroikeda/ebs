#include "error.h"
#include "expression.h"
#include "task.h"
#include "utility.h"
#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

enum {
  MAX_TASK = 1024,
  MAX_RECORD = 1024,
  MAX_BUFFER = 1024
};

enum command_type {
  COMMAND_HELP,
  COMMAND_ADD,
  COMMAND_DO,
  COMMAND_TICK,
  COMMAND_UNTICK,
  COMMAND_LIST,
  COMMAND_GUESS,
  MAX_COMMAND
};

const char* command_names[] = {
  "help",
  "add",
  "do",
  "tick",
  "untick",
  "list",
  "guess"
};

/* Parse a command. Return ERROR_UNKNOWN_COMMAND if there is no match. */
struct error parse_command_type(const char* str, enum command_type* result);

/* Find task by name. Return null if the task is not found. */
struct task* find_task(const char* name, struct task* tasks, size_t max_task);

/* Print help. */
void print_help(void);

/* List tasks matching the given expression. */
int list_tasks(const struct task* tasks, size_t max_task, const char* filter);

struct error parse_command_type(const char* const str, enum command_type* const
    result) {
  assert(NULL != str);
  assert(NULL != result);

  struct error error;
  for (enum command_type type = 0; type < MAX_COMMAND; type++) {
    if (0 != strcmp(str, command_names[type])) {
      continue;
    }
    *result = type;
    error.code = ERROR_NONE;
    return error;
  }
  error.code = ERROR_UNKNOWN_COMMAND;
  return error;
}

struct task* find_task(const char* const name, struct task* tasks, size_t
    max_task) {
  assert(NULL != name);
  assert(NULL != tasks);

  for (size_t task_num = 0; task_num < max_task; task_num++) {
    if (0 != strcmp(name, tasks[task_num].name)) {
      continue;
    }
    return &tasks[task_num];
  }
  return NULL;
}

void print_help(void) {
  puts("ebs");
  puts("help                   - print this message");
  puts("list [filter]          - list tasks");
  puts("add <task> <estimate>  - add a task"); 
  puts("do <task>              - start recording time for task");
  puts("tick <task>            - mark task as completed");
  puts("guess [filter]         - predict completion time of tasks");
}

int list_tasks(const struct task* tasks, size_t max_task, const char* const
    filter) {
  assert(NULL != tasks);
  assert(NULL != filter);

  struct expression pattern;
  struct error error = parse_expression(filter, &pattern);
  if (ERROR_NONE != error.code) {
    print_error(&error);
    return 1;
  }

  for (size_t task_num = 0; task_num < max_task; task_num++) {
    if (!string_matches(tasks[task_num].name, &pattern)) {
      continue;
    }
    char buffer[MAX_BUFFER];
    format_task(&tasks[task_num], buffer, MAX_BUFFER);
    printf("%s\n", buffer);
  }
  return 0;
}

int main(int argc, char** argv) {
  enum command_type type;
  const char* filter = "";
  const char* task_name = "";
  int64_t estimated_seconds;

  for (int arg_num = 1; arg_num < argc; arg_num++) {
    struct error error = parse_command_type(argv[arg_num], &type);
    if (ERROR_NONE != error.code) {
      print_error(&error);
      return 1;
    }
    switch (type) {
      case COMMAND_HELP:
        break;
      case COMMAND_ADD:
        if (argc <= arg_num + 2) {
          puts("usage: ebs <task> <estimate>");
          return 1;
        }
        task_name = argv[arg_num + 1];
        if (1 != sscanf(argv[arg_num + 2], "%" SCNd64, &estimated_seconds)) {
          puts("enter an integer for the estimated minutes");
        }
        estimated_seconds *= 60;
        arg_num += 2;
        break;
      case COMMAND_LIST:
        if (arg_num + 1 < argc) {
          filter = argv[arg_num + 1];
          arg_num += 1;
        }
        break;
      case COMMAND_DO:
        if (argc <= arg_num + 1) {
          puts("usage: ebs do <task>");
          return 1;
        }
        task_name = argv[arg_num + 1];
        arg_num += 1;
        break;
      case COMMAND_TICK:
        if (argc <= arg_num + 1) {
          puts("usage: ebs tick <task>");
          return 1;
        }
        task_name = argv[arg_num + 1];
        arg_num += 1;
        break;
      case COMMAND_GUESS:
        if (arg_num + 1 < argc) {
          filter = argv[arg_num + 1];
          arg_num += 1;
        }
        break;
      default:
        break;
    }
  }

  if (COMMAND_HELP == type) {
    print_help();
    return 0;
  }

  struct error error;
  const char* const time_sheet = "/Users/ryutaroikeda/git/ebs/test.tsv";
  const char* const task_sheet =
    "/Users/ryutaroikeda/git/ebs/resources/tasks.tsv";

  struct task tasks[MAX_TASK];
  struct time_record records[MAX_RECORD];

  size_t task_count;
  size_t record_count;

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

  if (COMMAND_LIST == type) {
    return list_tasks(tasks, task_count, filter);
  }

  if (COMMAND_ADD == type) {
    struct task* existing_task = find_task(task_name, tasks, task_count);
    if (NULL != existing_task) {
      printf("task %s already exists\n", task_name);
      return 1;
    }
    if (MAX_TASK <= task_count) {
      puts("too many tasks");
      return 1;
    }
    struct task task;
    task.estimated_seconds = estimated_seconds;
    task.actual_seconds = 0;
    strncpy(task.name, task_name, MAX_TASK_NAME);
    task.name[MAX_TASK_NAME] = '\0';
    task.status = STATUS_ACTIVE;
    tasks[task_count] = task;
    task_count++;
  }

  if (COMMAND_DO == type) {
    struct task* existing_task = find_task(task_name, tasks, task_count);
    if (NULL == existing_task) {
      puts("no such task");
      return 1;
    }
    if (MAX_RECORD <= record_count) {
      puts("too many records");
      return 1;
    }
    struct time_record record;
    record.time = time(NULL);
    if (-1 == record.time) {
      puts("time unavailable");
      return 1;
    }
    strncpy(record.name, task_name, MAX_TASK_NAME);
    record.name[MAX_TASK_NAME] = '\0';
    records[record_count] = record;
    record_count++;
    existing_task->status = STATUS_ACTIVE;
  }

  if (COMMAND_TICK == type) {
    struct task* existing_task = find_task(task_name, tasks, task_count);
    if (NULL == existing_task) {
      puts("no such task");
      return 1;
    }
    existing_task->status = STATUS_DONE;
  }

  if (COMMAND_GUESS == type) {
    error = predict_completion_date(tasks, task_count, filter);
    if (ERROR_NONE != error.code) {
      print_error(&error);
      return 1;
    }
  }

  error = write_time_sheet(time_sheet, records, record_count);
  if (ERROR_NONE != error.code) {
    print_error(&error);
    return 1;
  }

  error = write_task_sheet(task_sheet, tasks, task_count);
  if (ERROR_NONE != error.code) {
    print_error(&error);
    return 1;
  }

  return 0;
}
