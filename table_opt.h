#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

typedef unsigned int uint;

const static int table_size = 19997;

uint crc32_hash(const char* word);
