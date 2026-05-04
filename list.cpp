#include "list.h"

int list_resize(my_list* list, int new_capacity) {
    if (new_capacity <= list->capacity) {
        return 0;
    }

    char** new_data = (char**)calloc(new_capacity, sizeof(char*));
    int*   new_next = (int*)  calloc(new_capacity, sizeof(int));
    int*   new_prev = (int*)  calloc(new_capacity, sizeof(int));

    if (!new_data || !new_next || !new_prev) {
        free(new_data);
        free(new_next);
        free(new_prev);
        return -1;
    }

    for (int i = 0; i < list->capacity; i++) {
        new_data[i] = list->data[i];
        new_next[i] = list->next[i];
        new_prev[i] = list->prev[i];
    }

    for (int i = list->capacity; i < new_capacity - 1; i++) {
        new_data[i] = NULL;
        new_prev[i] = POIZON;
        new_next[i] = i + 1;
    }

    new_data[new_capacity - 1] = NULL;
    new_prev[new_capacity - 1] = POIZON;
    new_next[new_capacity - 1] = POIZON;

    if (list->free == POIZON) {
        list->free = list->capacity;
    }
    else {
        int last_free = list->free;
        while (new_next[last_free] != POIZON) {
            last_free = new_next[last_free];
        }
        new_next[last_free] = list->capacity;
    }

    free(list->data);
    free(list->next);
    free(list->prev);

    list->data = new_data;
    list->next = new_next;
    list->prev = new_prev;
    list->capacity = new_capacity;

    return 0;
}

int list_ctor(my_list* list, int size) {
    list->data = (char**)(calloc(size, sizeof(char*)));
    list->next = (int*)(calloc(size, sizeof(int)));
    list->prev = (int*)(calloc(size, sizeof(int)));

    //assert(list->data != NULL);
    //assert(list->next != NULL);
    //assert(list->prev != NULL);

    list->free = 1;
    list->data[0] = strdup(PTICHKA_STR);
    list->capacity = size;

    for(int i = 1; i < size; i++) {
        list->next[i] = i+1;
        list->prev[i] = POIZON;
        list->data[i] = NULL;
    }

    list->next[size-1] = POIZON;

    list->next[0] = 0;
    list->prev[0] = 0;
    return 0;
}

int decoder_of_error(int err) {
    if ((err & bad_data_addres) != 0) {
        printf("bad data addres\n");
    }
    if ((err & bad_next_addres) != 0) {
        printf("bad next addres\n");
    }
    if ((err & bad_prev_addres) != 0) {
        printf("bad prev addres\n");
    }
    if ((err & bad_next_equiv_prev) != 0) {
        printf("next not equiv prev\n");
    }
    if ((err & bad_ptichka) != 0) {
        printf("bad ptichka\n");
    }
    if ((err & bad_cycle_next) != 0) {
        printf("next not cycled\n");
    }
    if ((err & bad_cycle_prev) != 0) {
        printf("prev not cycled\n");
    }
    if ((err & bad_insert_index) != 0) {
        printf("bad insert index\n");
    }
    return 0;
}


int list_insert_after(my_list* list, int index, const char* value) {
    //int err_count = list_verify(list);
    //printf("next[0] = %d, prev[0] = %d\n", list->next[0], list->prev[0]);

    if (list->free == POIZON) {
        int new_cap = list->capacity * 2;
        if (list_resize(list, new_cap) != 0) {
            return POIZON;
        }
    }

    char new_word[32] = {};
    int index_new_elem = list->free;
    list->free = list->next[index_new_elem];
    memcpy(new_word, value, strlen(value));
    list->data[index_new_elem] = new_word;//strdup(value);

    list->next[index_new_elem] = list->next[index];
    list->prev[index_new_elem] = index;

    list->prev[list->next[index]] = index_new_elem;
    list->next[index] = index_new_elem;


    //printf("next[0] = %d, prev[0] = %d\n\n", list->next[0], list->prev[0]);
    /*
    if (index != 0) {
        //printf("%d\n", list->next[index+1]);
        if (list->next[index] == 0 && index != 1) {
            //printf("^^^^^^^^^^^\n");
            list->next[list->free] = list->next[index];
            list->next[index] = list->free;
            list->next[list->tail+1] = 0;
        }
        else if (index == 1) {
            list->next[list->free] = list->next[index];
            list->next[index] = list->free;
            list->next[list->tail+1] = 0;
        }
        else {
            //printf("456789\n");
            list->next[list->free] = list->next[index];
            list->next[index] = list->free;
        }
    }
    if (index == 0) {
        list->head = list->free;
    }
    if (list->free > list->tail) {
        list->tail = list->free;
    }
    if (list->free < list->head) {
        list->head = list->free;
    }

    //printf("%d, %d", list->head, list->tail);
    */
    //err_count += list_verify(list);
    //decoder_of_error(err_count);

#ifdef DEBUG_MODE
    list_dump(list, __func__, "ERROR");
#endif
    return index_new_elem;
}

int list_insert_before(my_list* list, int index, const char* value) {
    //int err_count = list_verify(list);
    int index_new_elem = list_insert_after(list, list->prev[index], value);

    //err_count |= list_verify(list);
    //decoder_of_error(err_count);
    return index_new_elem;
}


int list_del(my_list* list, int index) {
    //int err_count = list_verify(list);
    list->next[list->prev[index]] = list->next[index];
    list->prev[list->next[index]] = list->prev[index];

    free(list->data[index]);
    list->data[index] = NULL;
    list->prev[index] = POIZON;

    list->next[index] = list->free;
    list->free = index;
    //err_count += list_verify(list);
    //decoder_of_error(err_count);
#ifdef DEBUG_MODE
    list_dump(list, __func__, "ERROR");
#endif
    return 0;
}

int get_list_head(my_list* list) {
    //int err_count = list_verify(list);
    //decoder_of_error(err_count);
    return list->next[0];
}

int get_list_tail(my_list* list) {
    //int err_count = list_verify(list);
    //decoder_of_error(err_count);
    return list->prev[0];
}

int list_size(my_list* list) {
    int free_count = 0;
    int idx = list->free;
    while (idx != POIZON) {
        free_count++;
        idx = list->next[idx];
    }
    return list->capacity - free_count - 1;
}


int list_verify(my_list* list) {
    int error_count = 0;
    if (list->data[0] == NULL || strcmp(list->data[0], PTICHKA_STR) != 0) {
        error_count = error_count | bad_ptichka;
    }
    if (list->data == NULL) {
        error_count = error_count | bad_data_addres;
    }
    if (list->next == NULL) {
        error_count = error_count | bad_next_addres;
    }

    for (int i = 1; i < list->capacity; i++) {
        if (list->data[i] != NULL && list->next[i] != POIZON) {
            if (list->prev[list->next[i]] != i) {
                error_count = error_count | bad_next_equiv_prev;
            }
        }
    }

    return error_count;
}

void initialize_dump_file() {
    FILE *html_file = fopen("list_dump.html", "w");
    assert(html_file != NULL);
    fclose(html_file);
}

void create_html_dump(my_list* list, const char* func_inf, int number_of_files, const char* done_command, va_list arg_elements) {
    FILE *html_file = fopen("list_dump.html", "a");
    assert(html_file != NULL);


    fprintf(html_file, "<pre>");
    fprintf(html_file, "<h1>List Dump Visualization</h1>\n");
    fprintf(html_file, "<h3>After %s</h3>\n", func_inf);
//
    //fprintf(html_file, "<h3>");
    //vfprintf(html_file, done_command, arg_elements);
    //fprintf(html_file, "</h3>\n");
//
    //fprintf(html_file, "LIST FREE = %d\n", list->free);
    //fprintf(html_file, "LIST CAPACITY = %d\n", list->capacity);
//
    //fprintf(html_file, "DATA\n");
    //int i = 0;
    //while (i != list->capacity) {
    //    fprintf(html_file, "%s ", list->data[i]);
    //    i++;
    //}
    //fprintf(html_file, "\n");
//
    //fprintf(html_file, "NEXT\n");
    //i = get_list_head(list);
    //while (i != 0) {
    //    fprintf(html_file, "%d ", list->next[i]);
    //    i = list->next[i];
    //}
    //fprintf(html_file, "\n");
//
    //fprintf(html_file, "PREV\n");
    //i = get_list_tail(list);
    //while (i != 0) {
    //    fprintf(html_file, "%d ", list->prev[i]);
    //    i = list->prev[i];
    //}
    //fprintf(html_file, "\n");
//
    //int tail = get_list_tail(list);
    //fprintf(html_file, "TAIL = %d\n", tail);
    //int head = get_list_head(list);
    //fprintf(html_file, "HEAD = %d\n", head);

    for (int i = 0; i < number_of_files; i++) {
        fprintf(html_file, "<img src=\"output_%03d.png\" alt=\"List Graph\" style=\"max-width: 100%%; height: auto;\">\n", i);
    }
    fclose(html_file);
    //system("open list_dump.html");
}

void list_dtor(my_list* list) {
    //assert(list->data != NULL);
    //assert(list->next != NULL);
    //assert(list->prev != NULL);

    for (int i = 0; i < list->capacity; i++)
        free(list->data[i]);

    free(list->data);
    free(list->next);
    free(list->prev);
}

void list_dump(my_list* list, const char* func_inf, const char* done_command, ...) {
    //printf("jopa\n");
    FILE *file = fopen("list_dump.txt", "w");
    assert (file != NULL);

    fprintf(file, "LIST FREE = %d\n", list->free);
    fprintf(file, "LIST CAPACITY = %d\n", list->capacity);

    fprintf(file, "DATA\n");
    int i = 0;
    while (i != list->capacity) {
        fprintf(file, "%s ", list->data[i]);
        i++;
    }
    fprintf(file, "\n");

    fprintf(file, "NEXT\n");
    i = get_list_head(list);
    while (i != 0) {
        fprintf(file, "%d ", list->next[i]);
        i = list->next[i];
    }
    fprintf(file, "\n");

    fprintf(file, "PREV\n");
    i = get_list_tail(list);
    while (i != 0) {
        fprintf(file, "%d ", list->prev[i]);
        i = list->prev[i];
    }

    fprintf(file, "\n");

    int tail = get_list_tail(list);
    fprintf(file, "TAIL = %d\n", tail);
    int head = get_list_head(list);
    fprintf(file, "HEAD = %d\n", head);

    fclose(file);

    int file_count = graphic_dump(list);

    va_list arg_elements;
    va_start(arg_elements, done_command);
    create_html_dump(list, func_inf, file_count, done_command, arg_elements);
    va_end(arg_elements);
    //getchar();
}

int graphic_dump(my_list* list) {
    static int file_number = 1;
    FILE *file = fopen("graphic_list_dump.txt", "w");
    assert (file != NULL);
    fprintf(file, "digraph 1 {\n");
    fprintf(file, "rankdir = \"LR\";\n");
    //fprintf(file, "splines = ortho;\n");
    fprintf(file, "node [shape=octagon, style=filled, fillcolor=\"#ff0000\", color=\"black\", fontcolor=\"black\"];\n");
    fprintf(file, "\tnode0 [shape = Mrecord; style = filled; fillcolor = \"#de0808f4\"; color = \"#000000d0\"; label = \"PTICHKA | ptichka_value = %s | ptichka_next = 1\"]\n", PTICHKA_STR);
    int i = 1;
    while (i != list->capacity) {
        if (list->data[i] != 0 && strcmp(list->data[0], PTICHKA_STR) != 0){
            fprintf(file, "\tnode%d [shape = Mrecord; style = filled; fillcolor = \"#42ca11d0\"; color = \"#000000d0\"; label = \"data[%d] | value = %s | index_next = %d | index_prev = %d\"]", i, i, list->data[i], list->next[i], list->prev[i]);
        }
        else {
            fprintf(file, "\tnode%d [shape = Mrecord; style = filled; fillcolor = \"#0077ffd0\"; color = \"#000000d0\"; label = \"data[%d] | value = %s | index_next = %d | index_prev = %d\"]", i, i, list->data[i], list->next[i], list->prev[i]);
        }
        fprintf(file, "\n");
        i++;
    }
    i = 0;
    while (i != list->capacity) {
            fprintf(file, "\tnode%d->node%d[color = \"#ffffffd0\"; weight = %d];", i, i+1, 5000000);
            fprintf(file, "\n");
            i++;
    }

    //i = get_list_head(list);
    for(i = 0; i < list->capacity; i++) {
        if (list->prev[i] != POIZON) {
            fprintf(file, "\tnode%d->node%d[color = \"#09104ed0\"; weight = 10, tailport=\"n\", headport=\"n\"];", i, list->next[i]);
            fprintf(file, "\n");
        }
        //i = list->next[i];
    }

    //i = get_list_tail(list);
    for(i = 0; i < list->capacity; i++) {
        if (list->prev[i] != POIZON) {
            fprintf(file, "\tnode%d->node%d[color = \"#ff0055d0\"; tailport=\"s\", headport=\"s\", weight = 1, constraint=false, dir=back];", list->prev[i], i);
            fprintf(file, "\n");
        }
        //i = list->prev[i];
    }

    fprintf(file, "}\n");
    fclose(file);

    char file_name[100] = "";
    snprintf(file_name, sizeof(file_name), "dot graphic_list_dump.txt -T png -o output_%03d.png",file_number);
    file_number++;
    system(file_name);

    //system("dot graphic_list_dump.txt -T png -o output.png");
    return file_number;
}

//int main() {
    //initialize_dump_file();
    //my_list list = {0};
    //list_ctor(&list, 25);
    /*
    list_insert_after(&list, 0, 10);
    list_insert_after(&list, 1, 20);
    list_insert_after(&list, 2, 30);
    list_insert_after(&list, 3, 40);
    list_insert_after(&list, 4, 50);
    list_insert_after(&list, 5, 60);
    list_insert_after(&list, 3, 35);
    list_insert_after(&list, 7, 38);
    list_del(&list, 4);
    */

    //int index_new_elem = list_insert_after(&list, 0, 1);
    //list_dump(&list, __func__, "inserted %d to %d", 1, index_new_elem);
//
    //list_insert_after(&list, 1, 2);
    //list_dump(&list, __func__, "inserted 2");
//
    //list_insert_after(&list, 2, 3);
    //list_dump(&list, __func__, "inserted 3");
//
    //list_insert_after(&list, 3, 4);
    //list_dump(&list, __func__, "inserted 4");
//
    //list_insert_after(&list, 4, 500);
    //list_dump(&list, __func__, "inserted 500");
//
    //list_insert_after(&list, 5, 600);
    //list_dump(&list, __func__, "inserted 600");
//
    //list_insert_after(&list, 6, 700);
    //list_dump(&list, __func__, "inserted 700");
//
    //list_insert_after(&list, 7, 800);
    //list_dump(&list, __func__, "inserted 800");
//
    //list.next[6] = 300;
    //list.prev[7] = 5;
    //list_dump(&list, __func__, "KACCA BCTALA");

    //return 0;
//}
