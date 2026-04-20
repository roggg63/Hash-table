#include "hash_table.h"

const static int table_size = 503;

uint hash_const_1(const char* word) {
    return 1;
}

uint hash_first_ascii(const char* word) {
    uint hash = (uint)word[0];
    return hash;
}

uint hash_len(const char* word) {
    uint hash = (uint)(strlen(word));
    return hash;
}

uint hash_ascii_sum(const char* word) {
    uint hash = 0;
    int len = (int) strlen(word);

    for (int i = 0; i < len; i++) {
        hash += (uint) word[i];
    }

    return hash;
}

uint rol(uint value) {
    return (value << 1) | (value >> 31);
}

uint ror(uint value) {
    return (value >> 1) | (value << 31);
}

uint hash_rol(const char* word) {
    uint hash = 0;
    int len = strlen(word);
    for (int i = 0; i < len; i++) {
        hash = rol(hash) ^ (unsigned char)word[i];
    }
    return hash;
}

uint hash_ror(const char* word) {
    uint hash = 0;
    int len = strlen(word);
    for (int i = 0; i < len; i++) {
        hash = ror(hash) ^ (unsigned char)word[i];
    }
    return hash;
}

uint gnu_hash(const char* word) {
    uint hash = 5381;
    int c;
    while ((c = *word++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}



long check_size(FILE* file){
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);
    return file_size;
}

char** save_in_buffer(char** buffer) {
    FILE *file = fopen("big_data.txt", "r");
    if(file == NULL) {
        printf("ошибка открытия файла\n");

    }
    long file_size = check_size(file);

    buffer = (char**) calloc((file_size+2), sizeof(char*));

    char* word_buffer = (char*) calloc(file_size+2, sizeof(char));
    size_t word_buffer_size = fread(word_buffer, 1, file_size, file);
    word_buffer[word_buffer_size] = '\0';

    int index = 0;

    for (long i = 0; i < file_size; i++) {
        if (word_buffer[i] == ' ') {
            word_buffer[i] = '\0';
            buffer[index] = word_buffer + i + 1;
            index++;
        }
    }

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
                if (current_list->data[current_idx] != NULL && strcmp(current_list->data[current_idx], buffer[i]) == 0) {
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

    if (list == NULL) {
        printf("no in table\n");
        return 0;
    }
    else {
        for (int i = 0; i < list->capacity; i ++) {
            if (list->data[i] != NULL && strcmp(list->data[i], word) == 0) {
                printf("in table, i = %d, s = %s\n", i, list->data[i]);
                return 1;
            }
        }
    }
    return -1;
}

void get_info(my_list** table) {
    FILE* file = fopen("info.txt", "w");

    for (int i = 1; i < table_size; i++) {
        int size = (table[i] == NULL) ? 0 : list_size(table[i]);
        fprintf(file, "%d %d\n", i, size);
    }

    fclose(file);
}



int main() {
    //const char word[] = "jopa";
    //uint hash_1 = hash_const_1(word);
    //printf("%d\n", hash_1);
    //uint hash_2 = hash_first_ascii(word);
    //printf("%d\n", hash_2);
    //uint hash_3 = hash_len(word);
    //printf("%d\n", hash_3);
    //uint hash_4 = hash_ascii_sum(word);
    //printf("%d\n", hash_4);

    char** buffer = NULL;
    buffer = save_in_buffer(buffer);
    //printf("%p %s\n", buffer, buffer[10]);

    my_list** table = save_in_table(buffer, hash_ascii_sum);

    get_info(table);

    //for (int i = 0; i < table_size; i++) {
    //    if (table[i] != NULL) {
    //        list_dump(table[i], __func__, "bucket %d", i);
    //    }
    //}
    //find_in_table("coal", hash_ascii_sum, table);

    return 0;
}
