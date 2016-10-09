#include "error.h"
#include "utility.h"
#include <assert.h>
#include <inttypes.h>

static int test_oarse_int(void);
static int test_parse_int_overflow(void);

int test_oarse_int(void) {
  char s[] = "1023";
  struct error error;
  intmax_t expected = 1023;
  intmax_t actual;
  error = parse_int(s, 10, &actual);
  assert(ERROR_NONE == error.code);
  assert(expected == actual);
  return 0;
}

int test_parse_int_overflow(void) {
  char s[] = "9999999999999999999999";
  struct error error;
  intmax_t result;
  error = parse_int(s, 10, &result);
  assert(ERROR_STRING_TO_INT == error.code);
  return 0;
}

int main(void) {
  test_oarse_int();
  test_parse_int_overflow();
  return 0;
}
