#ifndef _ebs_hash_h_
#define _ebs_hash_h_

#include <stddef.h>

enum {
  MAX_HASH_ENTRY = 4096,
  MAX_HASH_KEY = 127,
};

/* Hash that maps strings to an index from 0 to MAX_HASH_ENTRY - 1. Empty
 * strings are considered empty entries. */
struct ebs_hash {
  char entries[MAX_HASH_ENTRY * (MAX_HASH_KEY + 1)];
};

/* Find the index of the given key. If the key doesn't exist,
 * ERROR_HASH_NOT_FOUND is returned and index is the last one checked. */
struct error ebs_hash_find(const struct ebs_hash*, const char*, size_t* index);

/* Add a key to the hash. If the hash is full, ERROR_HASH_FULL is returned. */
struct error ebs_hash_add(struct ebs_hash*, const char*);

#endif
