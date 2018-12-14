#include <stdlib.h>
#include <stdio.h>
#include "defines.h"

//You should declare the structures in the header file
//this is a start of a linked list. You're free to ignore this and build your own
//or extend this
typedef struct Node Node;

struct Node {
    Node* next;
    Node* prev;
    size_t bsize;    /*block size*/
    int is_used;    /*whether the block is used or not*/
    byte_ptr start_pos;    /* the starting position of the block */
};

void* mm_best_fit_malloc (size_t size);
void* mm_first_fit_malloc (size_t size);
void* mm_malloc (size_t size);
void  mm_free (void* ptr);
void  mm_print_heap_status (FILE*);
void  mm_defragment();
void place_node(Node* curr, Node* new, size_t size);
