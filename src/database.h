#ifndef _ebs_database_h_
#define _ebs_database_h_

struct database {
	struct task_array tasks;
	struct session_array sessions;
};
