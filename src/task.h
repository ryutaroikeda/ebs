#ifndef _ebs_task_h_
#define _ebs_task_h_

#define MAX_TASK_NAME_LENGTH 255

struct task {
	int id;
	char name[MAX_TASK_NAME_LENGTH + 1];
	int estimated_time;
	int predicted_time;
	int observed_time;
};


#endif

