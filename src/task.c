#include "task.h"
#include <assert.h>
#include <stdio.h>

/* Read a line from a stream. */
struct error
read_line(FILE* stream, struct string* line) {
	assert(NULL != stream);
	assert(NULL != line);

	size_t char_count;
	for (char_count = 0; char_count < MAX_STRING_LENGTH; char_count++) {
		int next_char = fgetc(stream);
		if (EOF == next_char) {
			break;
		}
		line->bytes[char_count] = next_char;
	}
	line->length = char_count;
	line->bytes[line->length] = '\0';

	struct error error;
	error.code = ferror(stream);
	return error;
}

/* Read tasks from a stream. */
struct error
read_tasks(FILE* stream, struct task_array* tasks) {
	assert(NULL != stream);
	assert(NULL != tasks);

	const int field_length = 4;

	struct error error;
	error.code = ERROR_NONE;

	size_t task_index;
	for (task_index = 0; task_index < MAX_TASK_LENGTH; task_index++) {
		tasks->tasks[task_index].task_id = task_index;
		int match_count = fscanf(stream, STRING_FORMAT "\t%d\t%d\t%d\n",
				tasks->tasks[task_index].name.bytes,
				&tasks->tasks[task_index].estimated_time,
				&tasks->tasks[task_index].predicted_time,
				&tasks->tasks[task_index].observed_time);

		if (EOF == match_count) {
			error.code = ferror(stream);
			break;
		}
		if (field_length != match_count) {
			error.code = ERROR_FILE;
			break;
		}
	}
	tasks->length = task_index;

	return error;
}

/* Write tasks to a stream. */
struct error
write_tasks(FILE* stream, const struct task_array* tasks) {
	assert(NULL != stream);
	assert(NULL != tasks);

	size_t task_index;
	for (task_index = 0; task_index < tasks->length; task_index++) {
		fprintf(stream, "%s\t%d\t%d\t%d\n",
				tasks->tasks[task_index].name.bytes,
				tasks->tasks[task_index].estimated_time,
				tasks->tasks[task_index].predicted_time,
				tasks->tasks[task_index].observed_time);
	}
	struct error error;
	error.code = ferror(stream);
	return error;
}

/* Print tasks to stdout. */
void
print_tasks(const struct task_array* tasks) {
	printf("name\testimated\tpredicted\tactual\n");
	write_tasks(stdout, tasks);
}

/* Add a task. */
struct error
push_task(struct task_array* tasks, const struct task* task) {
	assert(NULL != tasks);
	assert(NULL != task);

	struct error error;

	if (MAX_TASK_LENGTH <= tasks->length) {
		error.code = ERROR_TASK_LIMIT;
		return error;
	}

	tasks->tasks[tasks->length] = *task;
	tasks->length++;
	return error;
}

/* Read sessions from a stream. */
struct error
read_sessions(FILE* stream, struct session_array* sessions) {
	assert(NULL != stream);
	assert(NULL != sessions);

	const int field_length = 3;

	struct error error;
	size_t session_index;

	for (session_index = 0; session_index < MAX_SESSION_LENGTH;
			session_index++) {
		int match_count = fscanf(stream, "%d\t%ld\t%ld\n",
				&sessions->sessions[session_index].task_id,
				&sessions->sessions[session_index].begin,
				&sessions->sessions[session_index].end);
		if (EOF == match_count) {
			error.code = ferror(stream);
			return error;
		}
		if (field_length != match_count) {
			error.code = ERROR_FILE;
			return error;
		}
	}

	sessions->length = session_index;
	return error;
}

/* Write sessions to a stream. */
struct error
write_sessions(FILE* stream, const struct session_array* sessions) {
	assert(NULL != stream);
	assert(NULL != sessions);
	assert(sessions->length <= MAX_SESSION_LENGTH);

	size_t session_index;
	for (session_index = 0; session_index < sessions->length; session_index++) {
		fprintf(stream, "%d\t%ld\t%ld\n",
				sessions->sessions[session_index].task_id,
				sessions->sessions[session_index].begin,
				sessions->sessions[session_index].end);
	}

	struct error error;
	error.code = ferror(stream);
	return error;
}
