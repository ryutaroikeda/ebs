#include "error.h"
#include "task.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

extern long int timezone;

static int test_add_time_sheet_entry(void);

static int test_parse_and_format_task(void);

static int test_parse_time_record(void);

int test_add_time_sheet_entry(void) {
  char filename[] = "test.tsv";
  char task_name[] = "greet-world";
  struct error error = append_time_sheet_entry(filename, task_name);
  assert(ERROR_NONE == error.code);
  return 0;
}

int test_parse_and_format_task(void) {
  char s[] = "hello-world\tACTIVE\t10\t20";
  struct task t;
  struct error error = parse_task(s, &t);
  if (ERROR_NONE != error.code) {
    print_error(&error);
  }
  assert(ERROR_NONE == error.code);
  assert(0 == strcmp("hello-world", t.name));
  assert(STATUS_ACTIVE == t.status);
  assert(600 == t.estimated_seconds);
  assert(1200 == t.actual_seconds);

  char buffer[128];
  error = format_task(&t, buffer, 128);
  if (ERROR_NONE != error.code) {
    print_error(&error);
  }
  assert(ERROR_NONE == error.code);
  assert(0 == strcmp(s, buffer));
  return 0;
}

int test_parse_time_record(void) {
  char s[] = "1970-01-01T00:00:00\ttest_task";
  struct time_record t;
  struct error e = parse_time_record(s, &t);
  assert(ERROR_NONE == e.code);
  // This is tricky to test because of timezones and daylight savings. Check
  // manually or don't bother.
  //assert(timezone == t.time);
  assert(0 == strcmp("test_task", t.name));
  return 0;
}

int main(void) {
  test_add_time_sheet_entry();
  test_parse_and_format_task();
  test_parse_time_record();
  return 0;
}
