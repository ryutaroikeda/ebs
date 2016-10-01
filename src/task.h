#ifndef _ebs_task_h_
#define _ebs_task_h_

#include <stddef.h>
#include <stdint.h>

enum {
  MAX_NAME_LENGTH = 255,
  MAX_TASK_LENGTH = 1024,
  MAX_SIMULATION_LENGTH = 100,
  SIGMA_LEVEL = 2,
  NOT_SET = -1,
  SECONDS_OF_WORK_PER_DAY = 8 * 60 * 60
};

struct task {
  int64_t estimated_seconds;
  int64_t actual_seconds;
  char name[MAX_NAME_LENGTH + 1];
  /* struct string name; */
};

struct error
read_time_sheet(const char*, struct task*, const size_t, size_t*);

struct error
predict_completion_date(const struct task*, const size_t);
#endif
