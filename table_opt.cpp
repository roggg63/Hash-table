#include "table_opt.h"

uint32_t crc32_hash(const char* word) {
    uint32_t hash = 0xFFFFFFFF;
    /*
    int len = strlen(word);
    for (int i = 0; i < len; i++) {
        hash ^= word[i];
        for (int bit = 0; bit < 8; bit++) {
            if (hash & 1)
                hash = (hash >> 1) ^ 0x04C11DB7;
            else
                hash >>= 1;
        }
    }
    */
    while (*word) {
        hash = _mm_crc32_u8(hash, *word);
        word++;
    }

    return hash;
}

int strcmp_fast_32(const char* string_1, const char* string_2) {
    const uint64_t* block_1 = (const uint64_t*) string_1;
    const uint64_t* block_2 = (const uint64_t*) string_2;

    for (int i = 0; i < 4; i++) {
        if (block_1[i] != block_2[i])
            return (block_1[i] < block_2[i]) ? -1 : 1;
    }
    return 0;
}

void make_32byte_word(char* dest, const char* string) {
    size_t len = strlen(string);

    memcpy(dest, string, len);
    memset(dest + len, 0, 32 - len);
}


long check_size(FILE* file){
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);
    return file_size;
}

char** save_in_buffer(char** buffer) {
    FILE *file = fopen("normalized_data.bin", "rb");
    if(file == NULL) {
        printf("ошибка открытия файла\n");
    }
    long file_size = check_size(file);

    char* raw_data = (char*)aligned_alloc(32, file_size);
    fread(raw_data, 1, file_size, file);
    fclose(file);

    long word_count = file_size / 32;

    buffer = (char**)calloc(word_count + 1, sizeof(char*));

    for (long i = 0; i < word_count; i++) {
        buffer[i] = raw_data + i * 32;
    }
    buffer[word_count] = NULL;
    free(raw_data);

    return buffer;
}

my_list** save_in_table(char** buffer, uint(*hash_func)(const char* word)) {
    my_list** table = (my_list**) calloc(table_size, sizeof(my_list*));

    int i = 0;

    while (buffer[i] != NULL) {
        //printf("111\n");
        uint hash_res = (uint) (hash_func(buffer[i]) % table_size);
        if (table[hash_res] == NULL) {
            my_list* new_list = (my_list*)calloc(1, sizeof(my_list));
            list_ctor(new_list, 5);
            table[hash_res] = new_list;
            list_insert_after(table[hash_res], 0, buffer[i]);
        }
        else {
            my_list* current_list = table[hash_res];
            int current_idx = current_list->next[0];
            int word_in_table = 0;

            while (current_idx != 0) {
                if (current_list->data[current_idx] != NULL && current_list->data[current_idx][0] == buffer[i][0] && strcmp_fast_32(current_list->data[current_idx], buffer[i]) == 0) {
                    word_in_table = 1;
                    break;
                }
                current_idx = current_list->next[current_idx];
            }

            if (word_in_table == 0) {
                int tail = get_list_tail(table[hash_res]);
                list_insert_after(table[hash_res], tail, buffer[i]);
            }
        }
        i++;
    }
    return table;
}

int find_in_table(const char* word, uint(*hash_func)(const char* word), my_list** table) {
    uint hash = (uint) (hash_func(word) % table_size);

    my_list* list = table[hash];

    alignas(8) char word32[32] = {};
    make_32byte_word(word32, word);

    if (list == NULL) {
        //printf("no in table\n");
        return 2;
    }
    else {
        for (int i = 0; i < list->capacity; i ++) {
            if (list->data[i] != NULL && word32[0] == list->data[i][0] && strcmp_fast_32(list->data[i], word32) == 0) {
                //printf("in table, i = %d, s = %s\n", i, list->data[i]);
                return 1;
            }
        }
    }
    //printf("%s not in table\n", word);

    return -1;
}

int test_for_finding(my_list** table, char** buffer) {
    int total = 0;
    int i = 0;
    while (buffer[i] != NULL) {
        total += find_in_table(buffer[i], crc32_hash, table);
        i++;
    }
    return total;
}

void get_info(my_list** table, const char* name) {
    char file_name[256] = "";
    snprintf(file_name, sizeof(file_name), "info_%s.txt", name);

    FILE* file = fopen(file_name, "w");

    for (int i = 1; i < table_size; i++) {
        int size = (table[i] == NULL) ? 0 : list_size(table[i]);
        fprintf(file, "%d %d\n", i, size);
    }

    fclose(file);
}

double get_load_factor(my_list** table, int table_size) {
    int total_elements = 0;
    for (int i = 0; i < table_size; i++) {
        if (table[i] != NULL) {
            total_elements += list_size(table[i]);
        }
    }
    return (double)total_elements / table_size;
}



int main() {

    char** buffer = NULL;
    buffer = save_in_buffer(buffer);

    my_list** table = save_in_table(buffer, crc32_hash);

    //printf("lf = %lf\n", get_load_factor(table, table_size));

    int total = test_for_finding(table, buffer);

    total += find_in_table("coal", crc32_hash, table);
    total += find_in_table("jopa", crc32_hash, table);
    total += find_in_table("a", crc32_hash, table);
    total += find_in_table("b", crc32_hash, table);
    total += find_in_table("for", crc32_hash, table);
    total += find_in_table("hopa", crc32_hash, table);
    total += find_in_table("hippo", crc32_hash, table);
    total += find_in_table("denis", crc32_hash, table);
    total += find_in_table("pork", crc32_hash, table);
    total += find_in_table("return", crc32_hash, table);
    total += find_in_table("not", crc32_hash, table);
    total += find_in_table("wiht", crc32_hash, table);
    total += find_in_table("of", crc32_hash, table);
    total += find_in_table("at", crc32_hash, table);
    total += find_in_table("loh", crc32_hash, table);
    total += find_in_table("goga", crc32_hash, table);
    total += find_in_table("gym", crc32_hash, table);
    total += find_in_table("west", crc32_hash, table);
    total += find_in_table("macan", crc32_hash, table);
    total += find_in_table("cayenne", crc32_hash, table);
    total += find_in_table("beha", crc32_hash, table);
    total += find_in_table("merc", crc32_hash, table);
    total += find_in_table("sueta", crc32_hash, table);
    total += find_in_table("job", crc32_hash, table);
    total += find_in_table("labour", crc32_hash, table);
    total += find_in_table("gelik", crc32_hash, table);
    total += find_in_table("hookah", crc32_hash, table);
    total += find_in_table("hop", crc32_hash, table);
    total += find_in_table("loop", crc32_hash, table);
    total += find_in_table("joke", crc32_hash, table);
    total += find_in_table("qwerty", crc32_hash, table);
    total += find_in_table("chat", crc32_hash, table);
    total += find_in_table("gpt", crc32_hash, table);
    total += find_in_table("table", crc32_hash, table);
    total += find_in_table("fond", crc32_hash, table);
    total += find_in_table("here", crc32_hash, table);
    total += find_in_table("door", crc32_hash, table);
    total += find_in_table("shark", crc32_hash, table);

    printf("%d\n", total);

    return 0;
}
