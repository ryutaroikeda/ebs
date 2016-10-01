#include "error.h"
#include "time_sheet.h"
#include <assert.h>

int test_add_time_sheet_entry(void);

int test_add_time_sheet_entry(void) {
  char filename[] = "test.txt";
  char task_name[] = "greet world";
  struct error error = add_time_sheet_entry(filename, task_name);
  assert(ERROR_NONE == error.code);
  return 0;
}

int main(void) {
  test_add_time_sheet_entry();
  return 0;
}
