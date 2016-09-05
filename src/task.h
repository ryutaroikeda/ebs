#ifndef _ebs_task_h_
#define _ebs_task_h_

#include <stddef.h>
#include <stdio.h>
#include <time.h>

#define MAX_STRING_LENGTH 1023
#define STRING_FORMAT "%1023s"

#define MAX_TASK_LENGTH 1024
#define MAX_SESSION_LENGTH 2048

enum error_code {
	ERROR_NONE = 0,
	ERROR_FILE = 1,
	ERROR_TASK_LIMIT = 2
};

struct error {
	enum error_code code;
};

struct string {
	char bytes[MAX_STRING_LENGTH + 1];
	size_t length;
};

struct task {
	size_t task_id;
	struct string name;
	int estimated_time;
	int predicted_time;
	int observed_time;
};

struct task_array {
	struct task tasks[MAX_TASK_LENGTH];
	size_t length;
};

struct session {
	int task_id;
	time_t begin;
	time_t end;
};

struct session_array {
	struct session sessions[MAX_SESSION_LENGTH];
	size_t length;
};

struct error
read_line(FILE*, struct string*);

struct error
read_tasks(FILE*, struct task_array*);

struct error
write_tasks(FILE*, const struct task_array*);

void
print_tasks(const struct task_array*);

struct error
push_task(struct task_array*, const struct task*);

struct error
read_sessions(FILE*, struct session_array*);

struct error
write_sessions(FILE*, const struct session_array*);

#endif
