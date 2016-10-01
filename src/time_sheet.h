#ifndef _ebs_time_sheet_h_
#define _ebs_time_sheet_h_

/* Append an entry with the current time and the task name to the time sheet.
 * */
struct error add_time_sheet_entry(const char* filename, const char* task_name);

#endif
