#include "config.h"
#include "error.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static const char* config_names[] = {
  "--path"
};

struct error parse_config_type(const char* const str, enum config_type* const
    result) {
  assert(MAX_CONFIG == sizeof(config_names) / sizeof(config_names[0]));
  assert(NULL != str);
  assert(NULL != result);

  struct error error;
  for (enum config_type type = 0; type < MAX_CONFIG; type++) {
    if (0 != strcmp(str, config_names[type])) {
      continue;
    }
    *result = type;
    error.code = ERROR_NONE;
    return error;
  }
  error.code = ERROR_UNKNOWN_CONFIG;
  return error;
}

const char* get_config_name(const enum config_type config_type) {
  return config_names[config_type];
}

void print_config(const struct config* const config) {
  printf("path = %s\n", config->base_path);
}
