//#include "task.h"
#include "linear_algebra.h"
#include <stdio.h>

int
main(int argc, char** argv) {
	(void) argc;
	(void) argv;

	/*
	const char src_path[] = "resources/foo.tsv";
	const char dst_path[] = "resources/bar.tsv";
	const char ses_path[] = "resources/baz.tsv";
	const char ses_dst_path[] = "resources/alpha.tsv";

	FILE* src = fopen(src_path, "rb");
	FILE* dst = fopen(dst_path, "wb");
	FILE* ses = fopen(ses_path, "rb");
	FILE* ses_dst = fopen(ses_dst_path, "wb");

	struct string buffer;
	struct error error = read_line(file, &buffer);

	printf("%s\n", buffer.bytes);

	struct task_array tasks;
	struct session_array sessions;

	printf("reading %s\n", src_path);
	struct error read_error = read_tasks(src, &tasks);
	printf("error: %d\n", read_error.code);

	printf("writing %s\n", dst_path);
	struct error write_error = write_tasks(dst, &tasks);
	printf("error: %d\n", write_error.code);

	printf("reading %s\n", ses_path);
	struct error ses_read_error = read_sessions(ses, &sessions);
	printf("error: %d\n", ses_read_error.code);

	printf("writing %s\n", ses_dst_path);
	struct error ses_write_error = write_sessions(ses_dst, &sessions);
	printf("error: %d\n", ses_write_error.code);

	fclose(src);
	fclose(dst);
	fclose(ses);
	fclose(ses_dst);
	*/

	return 0;
}
