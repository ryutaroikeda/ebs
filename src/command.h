#ifndef _ebs_command_h_
#define _ebs_command_h_

enum command_type {
  COMMAND_HELP,
  COMMAND_ADD,
  COMMAND_CONFIG,
  COMMAND_DO,
  COMMAND_TICK,
  COMMAND_UNTICK,
  COMMAND_LIST,
  COMMAND_PREDICT,
  COMMAND_TOP,
  MAX_COMMAND
};

/* Parse a command. Return ERROR_UNKNOWN_COMMAND if there is no match. */
struct error parse_command_type(const char* str, enum command_type*
    result);

/* Get the command type as a string. */
const char* get_command_name(enum command_type);
#endif
