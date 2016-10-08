#ifndef _ebs_config_h_
#define _ebs_config_h_

enum config_type {
  CONFIG_PATH,
  MAX_CONFIG
};

struct config {
  char* base_path;
};

/* Parse config. Return ERROR_UNKNOWN_CONFIG if there is no
 * match. */
struct error parse_config_type(const char* str, enum
    config_type* result);

/* Get the config type as a string. */
const char* get_config_name(const enum config_type);

/* Print the config. */
void print_config(const struct config*);

#endif
