#ifndef _ebs_error_h_
#define _ebs_error_h_

enum error_code {
	ERROR_NONE,
	ERROR_FILE,
  ERROR_END_OF_FILE,
  ERROR_BUFFER_LIMIT,
	ERROR_TASK_LIMIT,
  ERROR_TASK_MISSING_FIELDS,
  ERROR_TIME_RECORD_MISSING_FIELDS,
	ERROR_BAD_TIME_STRING,
	ERROR_INCOMPLETE_TASK,
  ERROR_INVALID_TIME,
  ERROR_TIME_UNAVAILABLE,
  ERROR_LITERAL_TOO_LONG,
  ERROR_DISJUNCTION_TOO_LONG,
  ERROR_CONJUNCTION_TOO_LONG,
  ERROR_HASH_NOT_FOUND,
  ERROR_HASH_FULL,
  ERROR_UNKNOWN_STATUS,
  ERROR_UNKNOWN_COMMAND,
  ERROR_INVALID_ADD_PARAMETERS,
  ERROR_TIME_RECORD_LIMIT,
  ERROR_UNKNOWN_CONFIG,
  ERROR_NO_SUCH_TASK,
  ERROR_STRING_TO_INT,
  MAX_ERROR
};

struct error {
	enum error_code code;
};

void print_error(const struct error*);

#endif
