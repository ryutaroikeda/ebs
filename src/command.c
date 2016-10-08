#include "command.h"
#include "error.h"
#include <assert.h>
#include <string.h>

static const char* const command_names[] = {
  "help",
  "add",
  "config",
  "do",
  "tick",
  "untick",
  "list",
  "guess",
};

struct error parse_command_type(const char* const str, enum command_type* const
    result) {
  assert(MAX_COMMAND == sizeof(command_names) / sizeof(command_names[0]));
  assert(NULL != str);
  assert(NULL != result);

  struct error error;
  for (enum command_type type = 0; type < MAX_COMMAND; type++) {
    if (0 != strcmp(str, command_names[type])) {
      continue;
    }
    *result = type;
    error.code = ERROR_NONE;
    return error;
  }
  error.code = ERROR_UNKNOWN_COMMAND;
  return error;
}

const char* get_command_name(const enum command_type command_type) {
  return command_names[command_type];
}
