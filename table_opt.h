#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <nmmintrin.h>
#include <stdint.h>
#include <stddef.h>

#include "list.h"

typedef unsigned int uint;

const static int table_size = 19997;

uint32_t crc32_hash(const char* word);

extern "C" int my_strcmp(const char* string_1, const char* string_2);
