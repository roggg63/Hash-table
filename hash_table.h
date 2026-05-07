#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

typedef unsigned int uint;

const static int table_size = 4001;

uint hash_const_1(const char* word);
uint hash_first_ascii(const char* word);
uint hash_len(const char* word);
uint hash_ascii_sum(const char* word);
uint hash_rol(const char* word);
uint hash_ror(const char* word);
uint gnu_hash(const char* word);
uint crc32_hash(const char* word);
uint ror(uint value);
uint rol(uint value);

//uint (*funcs[8])(const char* word) = { hash_ascii_sum, hash_const_1, hash_first_ascii, hash_len, hash_rol, hash_ror, gnu_hash, crc32_hash};

#define HASH_FUNC(func) {.function = func, .name = #func}

struct functions {
    uint (*function)(const char* word);
    const char* name;
};

functions funcs[] = {HASH_FUNC(hash_ascii_sum),
                     HASH_FUNC(hash_const_1),
                     HASH_FUNC(hash_first_ascii),
                     HASH_FUNC(hash_len),
                     HASH_FUNC(hash_rol),
                     HASH_FUNC(hash_ror),
                     HASH_FUNC(gnu_hash),
                     HASH_FUNC(crc32_hash)};

//functions funcs[] = {{.function = hash_ascii_sum,   .name = "hash_ascii_sum"},
//                     {.function = hash_const_1,     .name = "hash_const_1"},
//                     {.function = hash_first_ascii, .name = "hash_first_ascii"},
//                     {.function = hash_len,         .name = "hash_len"},
//                     {.function = hash_rol,         .name = "hash_rol"},
//                     {.function = hash_ror,         .name = "hash_ror"},
//                     {.function = gnu_hash,         .name = "gnu_hash"},
//                     {.function = crc32_hash,       .name = "crc32_hash"}};
