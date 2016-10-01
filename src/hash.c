#include "hash.h"
#include "error.h"
#include <assert.h>
#include <string.h>

size_t ebs_hash_djb2(const char*);

const char* ebs_hash_get_const_entry(const char*, size_t);

char* ebs_hash_get_entry(char*, size_t);

size_t ebs_hash_djb2(const char* string) {
  assert(NULL != string);
  size_t hash = 5381;
  int c;
  while ((c = *string++))
    hash = ((hash << 5) + hash) + (size_t) c; /* hash * 33 + c */
  return hash;
}

const char* ebs_hash_get_const_entry(const char* const entries, const size_t
    entry_num) {
  assert(NULL != entries);
  return &entries[entry_num * (MAX_HASH_KEY + 1)];
}

char* ebs_hash_get_entry(char* const entries, const size_t entry_num) {
  assert(NULL != entries);
  return &entries[entry_num * (MAX_HASH_KEY + 1)];
}

struct error ebs_hash_find(const struct ebs_hash* hash, const char* const key,
    size_t* const index) {
  assert(NULL != hash);
  assert(NULL != key);
  assert(NULL != index);

  struct error error;
  size_t candidate_num = ebs_hash_djb2(key) % MAX_HASH_ENTRY;

  for (size_t entry_num = 0; entry_num < MAX_HASH_ENTRY; entry_num++) {
    *index = candidate_num + entry_num;
    const char* entry = ebs_hash_get_const_entry(hash->entries, candidate_num +
        entry_num);
    if (0 == strlen(entry)) {
      error.code = ERROR_HASH_NOT_FOUND;
      return error;
    }
    if (0 == strcmp(key, entry)) {
      error.code = ERROR_NONE;
      return error;
    }
  }

  error.code = ERROR_HASH_NOT_FOUND;
  return error;
}

struct error ebs_hash_add(struct ebs_hash* const hash, const char* const key) {
  assert(NULL != hash);
  assert(NULL != key);

  struct error error;
  size_t entry_num;
  // Check if the key already exists.
  error = ebs_hash_find(hash, key, &entry_num);
  // The key already exists.
  if (ERROR_NONE == error.code) {
    error.code = ERROR_NONE;
    return error;
  }
  char* entry = ebs_hash_get_entry(hash->entries, entry_num);
  // If the entry is not empty, the hash is full.
  if (0 != strlen(entry)) {
    error.code = ERROR_HASH_FULL;
    return error;
  }
  // Insert the key.
  strncpy(entry, key, MAX_HASH_KEY);
  entry[MAX_HASH_KEY] = '\0';
  error.code = ERROR_NONE;
  return error;
}
