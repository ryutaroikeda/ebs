#include "regex.h"
#include <assert.h>
#include <stdbool.h>

int
test_string_contains(void);

int
test_string_contains(void) {
  struct string s1;
  s1.s = "hello world";
  s1.length = 11;

  struct string s2;
  s2.s = "hello";
  s2.length = 5;

  struct string s3;
  s3.s = "mundo";
  s3.length = 5;

  assert(true == string_contains(s1, s2));
  assert(false == string_contains(s2, s1));
  assert(false == string_contains(s1, s3));
  return 0;
}

int
main(void) {
  test_string_contains();
  return 0;
}
