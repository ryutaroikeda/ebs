#include "task.h"
#include "calendar.h"
#include "error.h"
#include "monte_carlo.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <inttypes.h>

/* Read a tsv file of tasks. Each row has the task name, estimated time of
 * completion, and an optinal actual time of completion. Input time is in
 * minutes. */
struct error
read_time_sheet(const char* const filename, struct task* const tasks,
    const size_t max_task_length, size_t* const task_count) {
  assert(NULL != filename);
  assert(NULL != tasks);

  struct error error;
  FILE* fp = fopen(filename, "r");
  if (NULL == fp) {
    error.code = ERROR_FILE;
    return error;
  }

  size_t task_index = 0;
  size_t row_count;
  for (row_count = 0; row_count < max_task_length; row_count++) {
    int matches_count = fscanf(fp, "%256s\t%" SCNd64 "\t%" SCNd64 "\n",
        tasks[task_index].name, &tasks[task_index].estimated_seconds,
        &tasks[task_index].actual_seconds);
    if (matches_count < 2) {
      continue;
    }
    /* Convert minutes to seconds. */
    tasks[task_index].estimated_seconds *= 60;
    tasks[task_index].actual_seconds *= 60;
    if (matches_count == 2) {
      tasks[task_index].actual_seconds = NOT_SET;
    }
    task_index++;
  }
  *task_count = task_index;

  error.code = ERROR_NONE;
  return error;
}

/* Predict completion date. */
struct error
predict_completion_date(const struct task* const tasks, const size_t
    task_length) {
  assert(NULL != tasks);

  /* Compute the velocities and sum up the estimated work time in seconds. */
  double velocities[MAX_TASK_LENGTH];
  double estimated_times[MAX_TASK_LENGTH];
  double simulated_times[MAX_SIMULATION_LENGTH];

  int64_t seconds_to_work = 0;
  size_t task_index;
  size_t velocity_index = 0;
  size_t estimated_times_index = 0;

  for (task_index = 0; task_index < task_length; task_index++) {
    if (NOT_SET == tasks[task_index].actual_seconds) {
      seconds_to_work += tasks[task_index].estimated_seconds;
      estimated_times[estimated_times_index] =
        (double) tasks[task_index].estimated_seconds;
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

  const int64_t mean_seconds_to_work = (int64_t) mean;
  const int64_t five_percent_seconds_to_work = (int64_t) (mean - 
      (SIGMA_LEVEL * standard_deviation));
  const int64_t ninety_five_percent_seconds_to_work = (int64_t) (mean +
      (SIGMA_LEVEL * standard_deviation));

  printf("std. dev: %f\n", standard_deviation);
  printf("mean: %" PRId64 "\n5%%: %" PRId64 "\n95%%: %" PRId64 "\n",
      mean_seconds_to_work, five_percent_seconds_to_work,
      ninety_five_percent_seconds_to_work);

  struct error error;

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
