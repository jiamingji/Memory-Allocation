#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include "my_mem.h"
#include "sol_mem.h"

#define MAX_LINE 255

uint32_t next_multiple_of_8 (uint32_t n) {
    return ((n + 7) / 8) * 8;
}

int main (int argc, char** argv)
{
    if (argc != 3) {
        printf("Two parameters required: <traces/filename> <normal or defrag>\n");
        return 1;
    }

    uint64_t ids, ops;
    char line[MAX_LINE+1];

    FILE* trace = fopen(argv[1], "r");
    if (trace == 0) {
        printf("error opening file %s, did you type the correct name?\n", argv[1]);
        return 1;
    }

    //suggested heap
    fgets(line, MAX_LINE, trace);
    //number of ids
    fgets(line, MAX_LINE, trace);
    ids = atoll(line);
    //number of ops
    fgets(line, MAX_LINE, trace);
    ops = atoll(line);
    //weight
    fgets(line, MAX_LINE, trace);

    char** ptrs = malloc(ids * sizeof(char*));
    char** sol_ptrs = malloc(ids * sizeof(char*));
    char op;
    uint32_t id, size;
    uint32_t sum = 0;
    
    for (int i = 0 ; i < ops ; i++) {
        //read ops:  <a|r|f id size>
        fgets(line, MAX_LINE, trace);
        sscanf(line, "%c %u %u", &op, &id, &size);

        //printf("got:  %c  %u  %u\n", op, id, size);
        if (op == 'a') {
            sum += next_multiple_of_8(size);
            ptrs[id] = mm_malloc(size);
            sol_ptrs[id] = sol_malloc(size);
        }
        else if (op == 'f') {
            mm_free(ptrs[id]);
            sol_free(sol_ptrs[id]);
        }
    }
    
    sum = (sum/(1024*1024))+1;
    //printf("upper bound on memory allocation:  %u MB\n", sum);

    FILE* solution_file = fopen("temp_solution_file.txt", "w+");
    FILE* my_file = fopen("temp_my_file.txt", "w+");

    if (strcmp(argv[2], "defrag") == 0) {
        sol_defragment();
        mm_defragment();
    }

    sol_print_heap_status(solution_file);
    mm_print_heap_status(my_file);

    fseek(solution_file, 0L, SEEK_END);
    uint32_t sol_size = ftell(solution_file);
    fseek(solution_file, 0L, SEEK_SET);

    fseek(my_file, 0L, SEEK_END);
    uint32_t my_size = ftell(my_file);
    fseek(my_file, 0L, SEEK_SET);

    char* sol_txt = malloc(sol_size+1);
    char* my_txt = malloc(my_size+1);

    fread(sol_txt, 1, sol_size, solution_file);
    fread(my_txt, 1, my_size, my_file);
    sol_txt[sol_size] = 0;
    my_txt[my_size] = 0;

    if (sol_size != my_size) {
        printf("WRONG!!!!!!!!!!!!!!!!!\n");
        printf("\nsolution: %d   mine: %d\n", sol_size, my_size);
        printf("\nsolution: %s\n mine: %s\n", sol_txt, my_txt);
    }
    else {

        if( strcmp(sol_txt, my_txt) == 0 )
            printf("CORRECT\n");
        else {
            printf("WRONG!!!!!!!!!!!!!!!\n");
            printf("\nsolution: %s\n mine: %s\n", sol_txt, my_txt);
        }
        free(sol_txt);
        free(my_txt);
    }

    fclose(solution_file);
    fclose(my_file);

    remove("temp_solution_file.txt");
    remove("temp_my_file.txt");

    fclose(trace);
    return 0;
}