#include "error.h"
#include "expression.h"
#include "string.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

static int test_string_contains(void);
static int test_parse_expression(void);
static int test_string_matches(void);
static int do_test_string_matches(const char*, const char*, bool);

int test_string_contains(void) {
  char s1[] = "hello world";
  char s2[] = "hello";
  char s3[] = "mundo";

  assert(true == string_contains(s1, s2));
  assert(false == string_contains(s2, s1));
  assert(false == string_contains(s1, s3));
  return 0;
}

int test_parse_expression(void) {
  char s[] = "hello|!world;!goodbye|world;";
  struct expression e;
  struct error error = parse_expression(s, &e);
  assert(ERROR_NONE == error.code);
  return 0;
}

int do_test_string_matches(const char* const subject, const char* const
    expression, const bool expect_match) {
  struct expression e;
  struct error error = parse_expression(expression, &e);
  assert(ERROR_NONE == error.code);
  if (expect_match) {
    printf("testing %s matches ", subject);
  } else {
    printf("testing %s does not match ", subject);
  }
  print_expression(&e);
  assert(expect_match == string_matches(subject, &e));
  return 0;
}

int test_string_matches(void) {
  char* expressions[] = {
    "hello",
    "!hello",
    "hello,world",
    "hello,world",
    "!hello,!world",
    "!hello,!world",
    "!hello/!world",
    "!hello/!world"
  };
  char* subjects[] = {
    "hello world",
    "hello world",
    "hello",
    "world",
    "hello",
    "hello world",
    "hello",
    "world"
  };
  bool expected[] = {
    true,
    false,
    true,
    true,
    true,
    false,
    false,
    false
  };
  size_t max_test = sizeof(expressions) / sizeof(expressions[0]);
  for (size_t test_num = 0; test_num < max_test; test_num++) {
    do_test_string_matches(subjects[test_num], expressions[test_num],
        expected[test_num]);
  }
  return 0;
}

int
main(void) {
  test_string_contains();
  test_parse_expression();
  test_string_matches();
  return 0;
}
