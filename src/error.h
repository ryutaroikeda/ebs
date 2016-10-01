#ifndef _ebs_error_h_
#define _ebs_error_h_

enum error_code {
	ERROR_NONE,
	ERROR_FILE,
	ERROR_TASK_LIMIT,
	ERROR_BAD_TIME_STRING,
	ERROR_INCOMPLETE_TASK,
  ERROR_INVALID_TIME,
  ERROR_TIME_UNAVAILABLE,
  ERROR_LITERAL_TOO_LONG,
  ERROR_DISJUNCTION_TOO_LONG,
  ERROR_CONJUNCTION_TOO_LONG,
  ERROR_HASH_NOT_FOUND,
  ERROR_HASH_FULL
};

struct error {
	enum error_code code;
};

void print_error(const struct error*);

#endif
