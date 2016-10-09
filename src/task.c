#include "task.h"
#include "calendar.h"
#include "error.h"
#include "expression.h"
#include "utility.h"
#include "monte_carlo.h"

#include <assert.h>
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

enum {
  MAX_STATUS_NAME = 31,
  MAX_TASK_LENGTH = 1024,
  MAX_BUFFER = 4095,
  // @cleanup These can be configuration.
  MAX_SIMULATION_LENGTH = 100,
  SIGMA_LEVEL = 2,
  // @cleanup This should come from the calendar in the future.
  SECONDS_OF_WORK_PER_DAY = 8 * 60 * 60,
  MAX_LOOP = 1000000
};

static const char* const status_names[] = {
  "ACTIVE",
  "DONE"
};

static struct error parse_status(const char* const str, enum task_status*
    status);

const char* get_task_status(const enum task_status status) {
  return status_names[status];
}

struct error parse_status(const char* const str, enum task_status* const
    status) {
  assert(NULL != str);
  assert(NULL != status);

  struct error error;
  for (enum task_status status_num = 0; status_num < MAX_STATUS; status_num++)
  {
    if (0 == strcmp(status_names[status_num], str)) {
      *status = status_num;
      error.code = ERROR_NONE;
      return error;
    }
  }
  error.code = ERROR_UNKNOWN_STATUS;
  return error;
}

struct error format_time_record(const struct time_record* const record, char*
    const buffer, const size_t max_buffer) {
  assert(NULL != record);
  assert(NULL != buffer);

  struct error error;

  // This is not thread-safe.
  const struct tm* time = localtime(&record->time);
  if (NULL == time) {
    error.code = ERROR_TIME_UNAVAILABLE;
    return error;
  }
  char time_buffer[MAX_BUFFER];
  format_iso_8601_time(time, time_buffer, MAX_BUFFER);
  snprintf(buffer, max_buffer, "%s\t%s", time_buffer, record->name);
  error.code = ERROR_NONE;
  return error;
}

struct error read_time_record(FILE* const fp, struct time_record* const record)
{
  assert(NULL != fp);
  assert(NULL != record);

  struct error error;
  char buffer[MAX_BUFFER];
  size_t bytes_read;

  error = get_line(fp, buffer, MAX_BUFFER, &bytes_read);
  if (ERROR_NONE != error.code) {
    return error;
  }
  error = parse_time_record(buffer, record);
  if (ERROR_NONE != error.code) {
    print_error(&error);
  }
  error.code = ERROR_NONE;
  return error;
}

struct error append_time_sheet_entry(const char* const filename, const char*
    const task_name) {
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
    fprintf(fp, "%s\t", time_buffer);
  }
  fprintf(fp, "%s\n", task_name);
  fclose(fp);
  error.code = ERROR_NONE;
  return error;
}

struct error parse_time_record(const char* const str, struct time_record* const
    record) {
  assert(NULL != str);
  assert(NULL != record);

  const int expected_matches = 2;
  char time_buffer[MAX_BUFFER + 1];
  struct error error;

  int matches = sscanf(str, "%255s\t%255s", time_buffer, record->name);
  if (expected_matches != matches) {
    error.code = ERROR_TIME_RECORD_MISSING_FIELDS;
    return error;
  }

  struct tm time;
  error = parse_iso_8601_time(time_buffer, &time);
  if (ERROR_NONE != error.code) {
    return error;
  }
  time_t t = mktime(&time);
  if ((time_t)(-1) == t) {
    error.code = ERROR_INVALID_TIME;
    return error;
  }
  record->time = t;

  error.code = ERROR_NONE;
  return error;
}

struct error read_time_sheet(const char* const filename, struct task* const
    tasks, const size_t max_task) {
  assert(NULL != filename);
  assert(NULL != tasks);

  struct error error;
  FILE * fp = fopen(filename, "r");
  if (NULL == fp) {
    error.code = ERROR_FILE;
    return error;
  }

  struct time_record last_record;
  error = read_time_record(fp, &last_record);
  if (ERROR_END_OF_FILE == error.code) {
    error.code = ERROR_NONE;
    return error;
  }
  if (ERROR_NONE != error.code) {
    return error;
  }

  for (size_t loop_num = 0; loop_num < MAX_LOOP; loop_num++) {
    struct time_record record;
    error = read_time_record(fp, &record);
    if (ERROR_END_OF_FILE == error.code) {
      break;
    }
    if (ERROR_NONE != error.code) {
      print_error(&error);
      continue;
    }
    intmax_t elapsed_time = (intmax_t) difftime(record.time, last_record.time);
    struct task* const task = find_task(last_record.name, tasks, max_task);
    if (NULL != task) {
      task->actual_seconds += elapsed_time;
    }
    last_record = record;
  }

  fclose(fp);
  error.code = ERROR_NONE;
  return error;
}

struct error parse_task(const char* const str, struct task* const result) {
  assert(NULL != str);
  assert(NULL != result);

  char status_buffer[MAX_STATUS_NAME + 1];
  struct error error;
  const int expected_matches = 4;
  int matches = sscanf(str, "%127s\t%31s\t%jd\t%jd",
      result->name, status_buffer, &result->estimated_seconds,
      &result->actual_seconds);
  if (expected_matches != matches) {
    error.code = ERROR_TASK_MISSING_FIELDS;
    return error;
  }
  error = parse_status(status_buffer, &result->status);
  if (ERROR_NONE != error.code) {
    return error;
  }
  // Convert minutes to seconds.
  result->estimated_seconds *= 60;
  result->actual_seconds *= 60;
  error.code = ERROR_NONE;
  return error;
}

struct error format_task(const struct task* const task, char* const buffer,
    size_t max_buffer) {
  assert(NULL != task);
  assert(NULL != buffer);

  struct error error;

  int bytes_num = snprintf(buffer, max_buffer, "%s\t%s\t%jd\t%jd",
      task->name, get_task_status(task->status), task->estimated_seconds / 60,
      task->actual_seconds / 60);

  if (max_buffer <= (size_t) bytes_num) {
    error.code = ERROR_BUFFER_LIMIT;
    return error;
  }
  error.code = ERROR_NONE;
  return error;
}

struct error read_task(FILE* const fp, struct task* const task) {
  assert(NULL != fp);
  assert(NULL != task);

  struct error error;
  error.code = ERROR_NONE;
  char buffer[MAX_BUFFER];
  size_t bytes_read;

  error = get_line(fp, buffer, MAX_BUFFER, &bytes_read);
  if (ERROR_NONE != error.code) {
    return error;
  }
  
  error = parse_task(buffer, task);
  return error;
}

struct error write_task(const struct task* const task, FILE* const fp) {
  assert(NULL != task);
  assert(NULL != fp);

  struct error error;
  error.code = ERROR_NONE;
  char buffer[MAX_BUFFER];

  error = format_task(task, buffer, MAX_BUFFER);
  if (ERROR_NONE != error.code) {
    return error;
  }
  fprintf(fp, "%s\n", buffer);

  return error;
}

struct error predict_completion_date(const struct task* const tasks, const
    size_t task_length, const char* const filter) {
  assert(NULL != tasks);
  assert(NULL != filter);

  struct expression expression;
  struct error error = parse_expression(filter, &expression);
  if (ERROR_NONE != error.code) {
    return error;
  }

  /* Compute the velocities and sum up the estimated work time in seconds. */
  double velocities[MAX_TASK_LENGTH];
  double estimated_times[MAX_TASK_LENGTH];
  double simulated_times[MAX_SIMULATION_LENGTH];

  intmax_t seconds_to_work = 0;
  size_t task_index;
  size_t velocity_index = 0;
  size_t estimated_times_index = 0;

  for (task_index = 0; task_index < task_length; task_index++) {
    if (STATUS_ACTIVE == tasks[task_index].status) {
      if (!string_matches(tasks[task_index].name, &expression)) {
        continue;
      }
      seconds_to_work += tasks[task_index].estimated_seconds;
      estimated_times[estimated_times_index] = (double)
        tasks[task_index].estimated_seconds;
      estimated_times_index++;
      continue;
    }
    velocities[velocity_index] =
      ((double) tasks[task_index].estimated_seconds) / (double)
      tasks[task_index].actual_seconds;
    if (isnan(velocities[velocity_index])) {
      continue;
    }
    velocity_index++;
  }

  const size_t velocities_length = velocity_index;
  const size_t estimated_times_length = estimated_times_index;

  /* Run simulations. */
  simulate(velocities, velocities_length, estimated_times,
      estimated_times_length, simulated_times, MAX_SIMULATION_LENGTH);

  const double mean = compute_mean(simulated_times, MAX_SIMULATION_LENGTH);
  const double variance = compute_variance(simulated_times,
      MAX_SIMULATION_LENGTH);
  const double standard_deviation = sqrt(variance);

  const intmax_t mean_seconds_to_work = (intmax_t) mean;
  const intmax_t five_percent_seconds_to_work = (intmax_t) (mean - 
      (SIGMA_LEVEL * standard_deviation));
  const intmax_t ninety_five_percent_seconds_to_work = (intmax_t) (mean +
      (SIGMA_LEVEL * standard_deviation));

  /* Try to get the current time. */
  time_t current_time = time(NULL);
  if ((time_t) (-1) == current_time) {
    error.code = ERROR_TIME_UNAVAILABLE;
    return error;
  }
  struct tm today = *localtime(&current_time);

  /* Hard-code a 9 to 5 weekday. */
  struct calendar calendar;
  init_calendar(&calendar);

  struct event work;
  work.start = today;
  work.period = DAY;
  work.repetition = MAX_CALENDAR_DAYS;

  add_inclusion(&work, &calendar);

  struct event saturday;
  error = get_next_week(&today, SATURDAY, &saturday.start);
  if (ERROR_NONE != error.code) {
    return error;
  }
  saturday.period = WEEK;
  saturday.repetition = MAX_CALENDAR_DAYS;
  add_exclusion(&saturday, &calendar);

  struct event sunday;
  error = get_next_week(&today, SUNDAY, &sunday.start);
  if (ERROR_NONE != error.code) {
    return error;
  }
  sunday.period = WEEK;
  sunday.repetition = MAX_CALENDAR_DAYS;
  add_exclusion(&sunday, &calendar);

  struct tm mean_completion_date;
  error = compute_completion_date(&today, &calendar, SECONDS_OF_WORK_PER_DAY,
      mean_seconds_to_work, &mean_completion_date);
  if (ERROR_NONE != error.code) {
    return error;
  }

  struct tm five_percent_completion_date;
  error = compute_completion_date(&today, &calendar, SECONDS_OF_WORK_PER_DAY,
      five_percent_seconds_to_work, &five_percent_completion_date);
  if (ERROR_NONE != error.code) {
    return error;
  }

  struct tm ninety_five_percent_completion_date;
  error = compute_completion_date(&today, &calendar, SECONDS_OF_WORK_PER_DAY,
      ninety_five_percent_seconds_to_work,
      &ninety_five_percent_completion_date);
  if (ERROR_NONE != error.code) {
    return error;
  }

  printf("%s", "mean completion time: ");
  print_time(&mean_completion_date);
  printf("%s", "5% time: ");
  print_time(&five_percent_completion_date);
  printf("%s", "95% time: ");
  print_time(&ninety_five_percent_completion_date);

  error.code = ERROR_NONE;
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
