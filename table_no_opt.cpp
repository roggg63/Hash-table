#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>

#include "list.h"

typedef unsigned int uint;

const static int table_size = 19997;

uint32_t crc32_hash(const char* word);

uint32_t crc32_hash(const char* word) {
    uint32_t hash = 0xFFFFFFFF;
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


    return hash;
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

    char* raw_data = (char*)calloc(file_size, sizeof(char));
    fread(raw_data, 1, file_size, file);
    fclose(file);

    long word_count = file_size / 32;

    buffer = (char**)calloc(word_count + 1, sizeof(char*));

    for (long i = 0; i < word_count; i++) {
        buffer[i] = raw_data + i * 32;
    }
    buffer[word_count] = NULL;
    //free(raw_data);

    return buffer;
}

my_list** save_in_table(char** buffer, uint32_t(*hash_func)(const char* word)) {
    my_list** table = (my_list**) calloc(table_size, sizeof(my_list*));

    int i = 0;

    while (buffer[i] != NULL) {
        //printf("111\n");
        uint32_t hash_res = (uint32_t) (hash_func(buffer[i]) % table_size);
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
                if (current_list->data[current_idx] != NULL && current_list->data[current_idx][0] == buffer[i][0] && strcmp(current_list->data[current_idx], buffer[i]) == 0) {
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

void table_dtor(my_list** table,  char** buffer) {
    free(table);
    free(buffer);
}

extern "C" int find_in_table(const char* word, uint32_t(*hash_func)(const char* word), my_list** table) {
    uint hash = (uint) (hash_func(word) % table_size);

    my_list* list = table[hash];

    //alignas(8) char word32[32] = {};
    //make_32byte_word(word32, word);

    if (list == NULL) {
        //printf("no in table\n");
        return 2;
    }
    int current_idx = list->next[0];

    while (current_idx != POIZON) {
        if (list->data[current_idx] != NULL && word[0] == list->data[current_idx][0] && strcmp(list->data[current_idx], word) == 0) {
            return 1;
        }
        current_idx = list->next[current_idx];
    }
    return 0;
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

    printf("%d\n", total);

    table_dtor(table, buffer);
    //1244881
    return 0;
}
