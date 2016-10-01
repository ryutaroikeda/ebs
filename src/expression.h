#ifndef _ebs_expression_h_
#define _ebs_expression_h_

#include <stdbool.h>

enum {
  MAX_NAME = 254,
  MAX_LITERALS = 16,
  MAX_DISJUNCTIONS = 16
};

/* Represent a literal in an expression. */
struct literal {
  char name[MAX_NAME + 1];
  bool is_negative;
};

/* Represent a pattern in conjunctive normal form. */
struct expression {
  struct literal literals[MAX_LITERALS * MAX_DISJUNCTIONS];
};

/* Check if the second string is contained in the first. */
bool string_contains(const char* haystack, const char* needle);

/* Parse a conjunctive expression. The characters '!', '|', and '&' are
 * reserved. The expression must end with &. */
struct error parse_expression(const char* string, struct expression*
    expression);

/* Pretty-print the expression. */
void print_expression(const struct expression* expression);

/* Check if the string matches the expression. */
bool string_matches(const char* subject, const struct expression* expression);

#endif
