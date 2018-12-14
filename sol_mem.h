#include <stdlib.h>
#include <stdio.h>
#include "defines.h"

/*
 *  Hi! :)  nothing to see here...
 */ 

void* sol_best_fit_malloc (size_t size);
void* sol_first_fit_malloc (size_t size);
void* sol_malloc(size_t size);
void  sol_free(void* ptr);
void  sol_print_heap_status(FILE*);
void  sol_defragment();
