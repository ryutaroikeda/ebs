#ifndef _ebs_error_h_
#define _ebs_error_h_

enum error_code {
	ERROR_NONE,
	ERROR_FILE,
	ERROR_TASK_LIMIT,
	ERROR_BAD_TIME_STRING,
	ERROR_INCOMPLETE_TASK,
  ERROR_INVALID_TIME
};

struct error {
	enum error_code code;
};

#endif
