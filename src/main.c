#include "command.h"
#include "config.h"
#include "error.h"
#include "expression.h"
#include "task.h"
#include "utility.h"
#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

/* These files live under the ebs path. */
const char* TASK_SHEET = "task.tsv";
const char* TIME_SHEET = "time.tsv";

enum {
  MAX_TASK = 1024,
  MAX_RECORD = 1024,
  MAX_BUFFER = 4995,
  MAX_LOOP = 1000000,
};

/* Print help. */
void print_help(void);

/* List tasks matching the given expression. */
int list_tasks(const char* filter, const struct config* config);

/* Append task to the time sheet. */
int do_task(const char* task_name, const struct config* config);

/* Add task to the task sheet. */
int add_task(const char* task_name, intmax_t estimated_seconds, const struct
    config* config);

/* Set the task status to done. */
int tick_task(const char* task_name, const struct config* config);

/* Set the task status to incomplete. */
int untick_task(const char* task_name, const struct config* config);

/* Predict task completion times. */
int guess(const char* filter, const struct config* config);

/* Load tasks into a buffer. */
struct error load_tasks(const char* filter, const struct config* , struct task*
    tasks, size_t max_task, size_t* task_count);

/* Search for a task with the given name. */
struct error scan_task(const char* task_name, const struct config* config,
    bool* task_exists);

/* Set the status of the task. */
struct error set_task_status(const char* task_name, const enum task_status,
    const struct config*);

void print_help(void) {
  puts("ebs");
  puts("config:");
  puts("--path <path>          - path to the ebs directory");
  puts("commands:");
  puts("help                   - print this message");
  puts("add <task> <estimate>  - add a task"); 
  puts("config                 - print the configuration");
  puts("do <task>              - start recording time for task");
  puts("guess [filter]         - predict completion time of tasks");
  puts("list [filter]          - list tasks");
  puts("tick <task>            - mark task as completed");
}

int list_tasks(const char* const filter, const struct config* const config) {
  assert(NULL != filter);
  assert(NULL != config);
  assert(NULL != config->base_path);

  struct error error;
  struct task tasks[MAX_TASK];
  size_t task_count;

  error = load_tasks(filter, config, tasks, MAX_TASK, &task_count);
  if (ERROR_NONE != error.code) {
    print_error(&error);
    return 1;
  }

  for (size_t task_num = 0; task_num < task_count; task_num++) {
    char buffer[MAX_BUFFER];
    format_task(&tasks[task_num], buffer, MAX_BUFFER);
    printf("%s\n", buffer);
  }
  return 0;
}

struct error load_tasks(const char* const filter, const struct config* const
    config, struct task* tasks, const size_t max_task, size_t* task_count) {
  assert(NULL != filter);
  assert(NULL != tasks);
  assert(NULL != config);
  assert(NULL != config->base_path);

  struct error error;
  char task_sheet[MAX_BUFFER];
  char time_sheet[MAX_BUFFER];

  struct expression pattern;
  error = parse_expression(filter, &pattern);
  if (ERROR_NONE != error.code) {
    return error;
  }

  snprintf(task_sheet, MAX_BUFFER, "%s/%s", config->base_path, TASK_SHEET);

  FILE* fp = fopen(task_sheet, "r");
  if (NULL == fp) {
    printf("no such file %s\n", task_sheet);
    error.code = ERROR_FILE;
    return error;
  }

  *task_count = 0;
  for (size_t loop_num = 0; loop_num < MAX_LOOP; loop_num++) {
    if (max_task <= *task_count) {
      break;
    }
    error = read_task(fp, &tasks[*task_count]);
    if (ERROR_END_OF_FILE== error.code) {
      break;
    }
    if (ERROR_NONE != error.code) {
      print_error(&error);
      continue;
    }
    if (string_matches(tasks[*task_count].name, &pattern)) {
      *task_count += 1;
    }
  }

  snprintf(time_sheet, MAX_BUFFER, "%s/%s", config->base_path, TIME_SHEET);
  error = read_time_sheet(time_sheet, tasks, *task_count);
  if (ERROR_NONE != error.code) {
    fclose(fp);
    return error;
  }

  fclose(fp);
  error.code = ERROR_NONE;
  return error;
}

struct error scan_task(const char* const task_name, const struct config* const
    config, bool* task_exists) {
  assert(NULL != task_name);
  assert(NULL != config);
  assert(NULL != config->base_path);

  struct error error;
  char task_sheet[MAX_BUFFER];

  snprintf(task_sheet, MAX_BUFFER, "%s/%s", config->base_path, TASK_SHEET);
  FILE* fp = fopen(task_sheet, "r");
  if (NULL == fp) {
    error.code = ERROR_FILE;
    return error;
  }

  *task_exists = false;
  for (size_t loop_num = 0; loop_num < MAX_LOOP; loop_num++) {
    struct task task;
    error = read_task(fp, &task);
    if (ERROR_END_OF_FILE == error.code) {
      break;
    }
    if (ERROR_NONE != error.code) {
      print_error(&error);
      continue;
    }
    if (0 == strcmp(task_name, task.name)) {
      *task_exists = true;
      break;
    }
  }

  fclose(fp);
  error.code = ERROR_NONE;
  return error;
}

int do_task(const char* const task_name, const struct config* const config) {
  assert(NULL != task_name);
  assert(NULL != config);
  assert(NULL != config->base_path);

  struct error error;
  char time_sheet[MAX_BUFFER];

  bool task_exists = false;
  error = scan_task(task_name, config, &task_exists);
  if (ERROR_NONE != error.code) {
    print_error(&error);
    return 1;
  }
  if (!task_exists) {
    printf("no such task %s\n", task_name);
    return 1;
  }

  snprintf(time_sheet, MAX_BUFFER, "%s/%s", config->base_path, TIME_SHEET);
  error = append_time_sheet_entry(time_sheet, task_name);
  if (ERROR_NONE != error.code) {
    print_error(&error);
    return 1;
  }
  return 0;
}

int add_task(const char* const task_name, const intmax_t estimated_seconds,
    const struct config* const config) {
  assert(NULL != task_name);
  assert(NULL != config);
  assert(NULL != config->base_path);

  struct error error;
  char task_sheet[MAX_BUFFER];

  bool task_exists = false;
  error = scan_task(task_name, config, &task_exists);
  if (ERROR_NONE != error.code) {
    print_error(&error);
    return 1;
  }
  if (task_exists) {
    printf("task %s already exists\n", task_name);
    return 1;
  }

  struct task task;
  task.estimated_seconds = estimated_seconds;
  task.actual_seconds = 0;
  strncpy(task.name, task_name, MAX_TASK_NAME);
  task.name[MAX_TASK_NAME] = '\0';
  task.status = STATUS_ACTIVE;
  snprintf(task_sheet, MAX_BUFFER, "%s/%s", config->base_path, TASK_SHEET);

  FILE* fp = fopen(task_sheet, "a");
  error = write_task(&task, fp);
  if (ERROR_NONE != error.code) {
    fclose(fp);
    print_error(&error);
    return 1;
  }

  fclose(fp);
  return 0;
}

int tick_task(const char* const task_name, const struct config* config) {
  assert(NULL != task_name);
  assert(NULL != config);

  struct error error = set_task_status(task_name, STATUS_DONE, config);
  if (ERROR_NONE != error.code) {
    print_error(&error);
    return 1;
  }
  return 0;
}

int untick_task(const char* const task_name, const struct config* config) {
  assert(NULL != task_name);
  assert(NULL != config);

  struct error error = set_task_status(task_name, STATUS_ACTIVE, config);
  if (ERROR_NONE != error.code) {
    print_error(&error);
    return 1;
  }
  return 0;
}

struct error set_task_status(const char* const task_name, const enum
    task_status status, const struct config* const config) {
  assert(NULL != task_name);
  assert(NULL != config);

  struct error error;
  char task_sheet[MAX_BUFFER];
  char task_backup[MAX_BUFFER];
  snprintf(task_sheet, MAX_BUFFER, "%s/%s", config->base_path, TASK_SHEET);
  snprintf(task_backup, MAX_BUFFER, "%s/%s.bak", config->base_path,
      TASK_SHEET);

  FILE* const fin = fopen(task_sheet, "r");
  if (NULL == fin) {
    error.code = ERROR_FILE;
    return error;
  }
  FILE* const fout = fopen(task_backup, "w");
  if (NULL == fout) {
    error.code = ERROR_FILE;
    return error;
  }

  bool task_exists = false;
  for (size_t loop_num = 0; loop_num < MAX_LOOP; loop_num++) {
    struct task task;
    error = read_task(fin, &task);
    if (ERROR_END_OF_FILE == error.code) {
      break;
    }
    if (ERROR_NONE != error.code) {
      fclose(fin);
      fclose(fout);
      return error;
    }
    if (0 == strcmp(task_name, task.name)) {
      task.status = status;
      task_exists = true;
    }
    error = write_task(&task, fout);
    if (ERROR_NONE != error.code) {
      fclose(fin);
      fclose(fout);
      return error;
    }
  }

  fclose(fin);
  fclose(fout);
  // Commit changes and delete the backup.
  error = copy(task_backup, task_sheet);
  if (ERROR_NONE != error.code) {
    return error;
  }
  if (0 != remove(task_backup)) {
    // Report and carry on.
    printf("remove(): %s\n", strerror(errno));
  }
  if (!task_exists) {
    error.code = ERROR_NO_SUCH_TASK;
    return error;
  }
  error.code = ERROR_NONE;
  return error;
}

int guess(const char* const filter, const struct config* const config) {
  assert(NULL != filter);
  assert(NULL != config);
  assert(NULL != config->base_path);

  struct error error;
  struct task tasks[MAX_TASK];
  size_t task_count;

  error = load_tasks(filter, config, tasks, MAX_TASK, &task_count);
  if (ERROR_NONE != error.code) {
    print_error(&error);
    return 1;
  }

  error = predict_completion_date(tasks, task_count, filter);
  if (ERROR_NONE != error.code) {
    print_error(&error);
    return 1;
  }

  return 0;
}

int main(int argc, char** argv) {
  struct config config;
  config.base_path = NULL;

  for (int arg_num = 1; arg_num < argc; arg_num++) {
    enum config_type config_type;
    struct error error = parse_config_type(argv[arg_num], &config_type);

    // Parse configuration.
    if (ERROR_NONE == error.code) {
      if (CONFIG_PATH == config_type) {
        if (argc <= arg_num + 1) {
          printf("%s <path>\n", get_config_name(CONFIG_PATH));
          return 1;
        }
        arg_num += 1;
        config.base_path = argv[arg_num];
        continue;
      } else {
        printf("%s is not supported\n", get_config_name(config_type));
        return 1;
      }
    }

    enum command_type command_type;
    error = parse_command_type(argv[arg_num], &command_type);
    if (ERROR_NONE != error.code) {
      printf("unknown command or configuration %s\n", argv[arg_num]);
      return 1;
    }
    if (COMMAND_HELP == command_type) {
      print_help();
      return 0;
    }
    if (COMMAND_CONFIG == command_type) {
      print_config(&config);
      return 0;
    }

    if (NULL == config.base_path) {
      puts("specify path with --path <path>");
      return 1;
    }

    if (COMMAND_LIST == command_type) {
      const char* filter = "";
      if (arg_num + 1 < argc) {
        arg_num += 1;
        filter = argv[arg_num];
      }
      return list_tasks(filter, &config);
    }

    if (COMMAND_DO == command_type) {
      if (argc <= arg_num + 1) {
        puts("usage: do <task>");
        return 1;
      }
      const char* task_name = NULL;
      arg_num += 1;
      task_name = argv[arg_num];
      return do_task(task_name, &config);
    } 

    if (COMMAND_ADD == command_type) {
      if (argc <= arg_num + 2) {
        puts("usage: add <task> <estimate>");
        return 1;
      }
      arg_num += 1;
      const char* const task_name = argv[arg_num];
      arg_num += 1;
      intmax_t estimated_seconds;
      error = parse_int(argv[arg_num], 10, &estimated_seconds);
      if (ERROR_NONE != error.code) {
        print_error(&error);
        return 1;
      }
      return add_task(task_name, estimated_seconds, &config);
    }

    if (COMMAND_TICK == command_type) {
      if (argc <= arg_num + 1) {
        puts("usage: tick <task>");
        return 1;
      }
      arg_num += 1;
      const char* const task_name = argv[arg_num];
      return tick_task(task_name, &config);
    }

    if (COMMAND_UNTICK == command_type) {
      if (argc <= arg_num + 1) {
        puts("usage: untick <task>");
        return 1;
      }
      arg_num += 1;
      const char* const task_name = argv[arg_num];
      return untick_task(task_name, &config);
    }

    if (COMMAND_GUESS == command_type) {
      const char* filter = "";
      if (arg_num + 1 < argc) {
        arg_num += 1;
        filter = argv[arg_num];
      }
      return guess(filter, &config);
    }
    printf("unsupported command %s\n", get_command_name(command_type));
    return 1;
  }

  return 0;
}
