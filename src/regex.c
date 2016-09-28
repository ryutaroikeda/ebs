#include "regex.h"

bool
string_contains(const struct string haystack, const struct string needle) {
  if (haystack.length < needle.length) {
    return false;
  }

  size_t haystack_index;
  for (haystack_index = 0; haystack_index <= haystack.length - needle.length;
      haystack_index++) {
    size_t needle_index;
    bool is_match = true;
    for (needle_index = 0; needle_index < needle.length; needle_index++) {
      if (haystack.s[haystack_index + needle_index] !=
          needle.s[needle_index]) {
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
