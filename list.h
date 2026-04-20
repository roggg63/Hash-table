#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <string.h>
//#define DEBUG_MODE

int const static POIZON = -1;
//int const static PTICHKA = 77777777;
const static char* PTICHKA_STR = "PTICHKA";

struct my_list {
    char** data;
    int* next;
    int* prev;
    int free;
    int capacity;
};

int list_ctor(my_list* list, int size);
int list_insert_after(my_list* list, int index, const char* value);
int graphic_dump(my_list* list);
void list_dump(my_list* list, const char* func_inf, const char* done_command, ...) __attribute__((format(printf, 3, 4)));
int list_verify(my_list* list);
int list_del(my_list* list, int index);
int get_list_head(my_list* list);
int get_list_tail(my_list* list);
int list_verify(my_list* list);
void list_dtor(my_list* list);
void create_html_dump(my_list* list, const char* func_inf, int number_of_files, const char* done_command, va_list arg_elements);
int list_insert_before(my_list* list, int index, const char* value);
int decoder_of_error(int err);
int list_resize(my_list* list, int new_capacity);
int list_size(my_list* list);

enum list_errors {
    bad_data_addres     = 0b00000001,
    bad_next_addres     = 0b00000010,
    bad_prev_addres     = 0b00000100,
    bad_next_equiv_prev = 0b00001000,
    bad_ptichka         = 0b00010000,
    bad_cycle_next      = 0b00100000,
    bad_cycle_prev      = 0b01000000,
    bad_insert_index    = 0b10000000,
};
