#include "expression.h"
#include "error.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

enum {
  SYMBOL_NOT = '!',
  SYMBOL_OR = ',',
  SYMBOL_AND = '/'
};

/* Get a pointer to a literal by indices. */
static struct literal* get_literal(struct literal* literals, size_t
    disjunction_num, size_t literal_num);

/* Get a const pointer to a literal by indices. */
static const struct literal* get_const_literal(const struct literal* literals,
    size_t disjuction_num, size_t literal_num);

/* Return true if the literal matches nothing. This is the empty, negative
 * literal. */
static bool is_null_literal(const struct literal* lit);

/* Return true if the literal matches everything. */
static bool is_full_literal(const struct literal* lit);

/* Return true if the disjunction matches everything. */
static bool is_full_disjunction(const struct expression* expression, size_t
    disjunction_num);

/* Print a literal. */
static void print_literal(const struct literal* lit);

/* Print a disjunction. */
static void print_disjunction(const struct expression* expression, size_t
    disjunction_num);

bool string_contains(const char* const haystack, const char* const needle) {
  assert(NULL != haystack);
  assert(NULL != needle);

  size_t haystack_length = strlen(haystack);
  size_t needle_length = strlen(needle);
  if (haystack_length < needle_length) {
    return false;
  }

  size_t haystack_index;
  for (haystack_index = 0; haystack_index <= haystack_length - needle_length;
      haystack_index++) {
    size_t needle_index;
    bool is_match = true;
    for (needle_index = 0; needle_index < needle_length; needle_index++) {
      if (haystack[haystack_index + needle_index] !=
          needle[needle_index]) {
        is_match = false;
        break;
      }
    }
    if (is_match) {
      return true;
    }
  }
  return false;
}

struct literal* get_literal(struct literal* const literals, const size_t
    disjunction_num, const size_t literal_num) {
  assert(NULL != literals);
  assert(disjunction_num <= MAX_DISJUNCTIONS);
  assert(literal_num <= MAX_LITERALS);

  return &literals[disjunction_num * MAX_LITERALS + literal_num];
}

const struct literal* get_const_literal(const struct literal* const literals,
    const size_t disjunction_num, const size_t literal_num) {
  assert(NULL != literals);
  assert(disjunction_num <= MAX_DISJUNCTIONS);
  assert(literal_num <= MAX_LITERALS);

  return &literals[disjunction_num * MAX_LITERALS + literal_num];
}

bool is_null_literal(const struct literal* const lit) {
  assert(NULL != lit);
  return ((lit->is_negative) && (0 == strlen(lit->name)));
}

bool is_full_literal(const struct literal* const lit) {
  assert(NULL != lit);
  return ((!lit->is_negative) && (0 == strlen(lit->name)));
}

bool is_full_disjunction(const struct expression* const expression, const
    size_t disjunction_num) {
  assert(NULL != expression);
  assert(disjunction_num < MAX_DISJUNCTIONS);

  for (size_t literal_num = 0; literal_num < MAX_LITERALS; literal_num++) {
    const struct literal* lit = get_const_literal(expression->literals,
        disjunction_num, literal_num);
    if (!is_full_literal(lit)) {
      return false;
    }
  }
  return true;
}

struct error parse_expression(const char* const string, struct expression*
    const expression) {
  assert(NULL != string);
  assert(NULL != expression);
  memset(expression, 0, sizeof(struct expression));
  const size_t string_len = strlen(string);
  size_t name_length = 0;
  size_t literal_num = 0;
  size_t disjunction_num = 0;
  struct error error;

  for (size_t string_pos = 0; string_pos < string_len; string_pos++) {
    if (SYMBOL_NOT == string[string_pos]) {
      struct literal* lit = get_literal(expression->literals, disjunction_num,
          literal_num);
      lit->is_negative = true;
    } else if (SYMBOL_OR == string[string_pos]) {
      if (MAX_LITERALS <= literal_num) {
        error.code = ERROR_DISJUNCTION_TOO_LONG;
        return error;
      }
      literal_num++;
      name_length = 0;
    } else if (SYMBOL_AND == string[string_pos]) {
      if (MAX_DISJUNCTIONS <= disjunction_num) {
        error.code = ERROR_CONJUNCTION_TOO_LONG;
        return error;
      }
      literal_num++;
      // Negate unused literals.
      for (size_t unused_literal_num = literal_num; unused_literal_num <
          MAX_LITERALS; unused_literal_num++) {
        struct literal* const unused_lit = get_literal(expression->literals,
            disjunction_num, unused_literal_num);
        unused_lit->is_negative = true;
      }
      disjunction_num++;
      literal_num = 0;
      name_length = 0;
    } else {
      if (MAX_NAME < name_length) {
        error.code = ERROR_LITERAL_TOO_LONG;
        return error;
      }
      struct literal* const lit = get_literal(expression->literals,
          disjunction_num, literal_num);
      lit->name[name_length] = string[string_pos];
      name_length++;
      // Handle end of string.
      if (string_pos == string_len - 1) {
        literal_num++;
        // Negate unused literals.
        for (size_t unused_literal_num = literal_num; unused_literal_num <
            MAX_LITERALS; unused_literal_num++) {
          struct literal* const unused_lit = get_literal(expression->literals,
              disjunction_num, unused_literal_num);
          unused_lit->is_negative = true;
        }
      }
    }
  }
  error.code = ERROR_NONE;
  return error;
}

void print_literal(const struct literal* const lit) {
  assert(NULL != lit);
  if (lit->is_negative) {
    printf("%s", "¬");
  }
  printf("%s", lit->name);
}

void print_disjunction(const struct expression* const expression, const size_t
    disjunction_num) {
  assert(NULL != expression);

  const struct literal* const first_lit =
    get_const_literal(expression->literals, disjunction_num, 0);
  if (is_null_literal(first_lit)) {
    return;
  }
  print_literal(first_lit);

  for (size_t literal_num = 1; literal_num < MAX_LITERALS; literal_num++) {
    const struct literal* const lit = get_const_literal(expression->literals,
        disjunction_num, literal_num);
    // Check if this is the last literal in the disjunction.
    if (is_null_literal(lit)) {
      break;
    }
    printf("%s", " | ");
    print_literal(lit);
  }
}

void print_expression(const struct expression* const expression) {
  assert(NULL != expression);

  if (is_full_disjunction(expression, 0)) {
    return;
  }
  print_disjunction(expression, 0);

  for (size_t disjunction_num = 1; disjunction_num < MAX_DISJUNCTIONS;
      disjunction_num++) {
    if (is_full_disjunction(expression, disjunction_num)) {
      break;
    }
    printf("%s", " & ");
    print_disjunction(expression, disjunction_num);
  }
  printf("%s", "\n");
}

bool string_matches(const char* const subject, const struct expression* const
    expression) {
  assert(NULL != subject);
  assert(NULL != expression);
  for (size_t disjunction_num = 0; disjunction_num < MAX_DISJUNCTIONS;
      disjunction_num++) {
    bool matched_disjunction = false;
    for (size_t literal_num = 0; literal_num < MAX_LITERALS; literal_num++) {
      const struct literal* const lit = get_const_literal(expression->literals,
          disjunction_num, literal_num);
      bool match = string_contains(subject, lit->name);
      if (match && !lit->is_negative) {
        matched_disjunction = true;
        break;
      }
      if (!match && lit->is_negative) {
        matched_disjunction = true;
        break;
      }
    }
    if (!matched_disjunction) {
      return false;
    }
  }
  return true;
}
