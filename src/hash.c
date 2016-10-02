#include "hash.h"
#include "error.h"
#include <assert.h>
#include <string.h>
#include <stdint.h>

enum {
  HASH_MURMUR_SEED = 12345
};

static uint32_t ebs_hash_murmur3(const char* str, size_t len, uint32_t seed);

static const char* ebs_hash_get_const_entry(const char*, size_t);

static char* ebs_hash_get_entry(char*, size_t);

uint32_t ebs_hash_murmur3(const char* const str, const size_t len, const
    uint32_t seed) {
  uint32_t c1 = 0xcc9e2d51;
  uint32_t c2 = 0x1b873593;
  uint32_t r1 = 15;
  uint32_t r2 = 13;
  uint32_t m = 5;
  uint32_t n = 0xe6546b64;
  uint32_t h = seed;
  size_t block_size = len / 4;
  const uint32_t* blocks = (const uint32_t*) str;
  size_t i;
  for (i = 0; i < block_size; i++) {
    uint32_t k = blocks[i];
    k *= c1;
    // rotate left by r1
    k = (k << r1) | (k >> (32 - r1));
    k *= c2;
    h ^= k;
    h = (h << r2) | (h >> (32 - r2));
    h = h * m + n;
  }
  const uint8_t* tail = (const uint8_t*) &blocks[block_size];
  uint32_t k1 = 0;
  switch (len & 3) {
  case 3:
    k1 ^= (uint32_t) tail[2] << 16;
  case 2:
    k1 ^= (uint32_t) tail[1] << 8;
  case 1:
    k1 ^= tail[0];
    k1 += c1;
    k1 = (k1 << r1) | (k1 >> (32 - r1));
    k1 += c2;
    h ^= k1;
  }
  h ^= len;
  h ^= (h >> 16);
  h *= 0x85ebca6b;
  h ^= (h >> 13);
  h *= 0xc2b2ae35;
  h ^= (h >> 16);
  return h;
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

void ebs_hash_init(struct ebs_hash* hash) {
  assert(NULL != hash);
  memset(hash, 0, sizeof(struct ebs_hash));
}

struct error ebs_hash_find(const struct ebs_hash* hash, const char* const key,
    size_t* const index) {
  assert(NULL != hash);
  assert(NULL != key);
  assert(NULL != index);

  struct error error;
  size_t candidate_num = ebs_hash_murmur3(key, strlen(key), HASH_MURMUR_SEED) %
    MAX_HASH_ENTRY;

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
