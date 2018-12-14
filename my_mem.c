#include "my_mem.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/*
 * You CAN'T allocate a global static array in global memory, for example,
 * you CAN'T do node[50], you HAVE to use pointers, as many as you want. You may
 * need more than one linked list, I (your TA) use only one list but keep/maintain information about the nodes
 * in a specific way.
 *
 * Feel free to use a different data structure, single/doubly linked lists, trees, w/e, all up to you.
 *
 * Also, to make sure you do it correctly; all structures that you use for bookkeeping, you will use
 * the system's malloc, not yours. Only user's data will reside in the heap you are managing.
 */

//example: head of a list of nodes
Node* head;

// this global variable is used to count the currently allocated spaces.
size_t total_used_space;

//This is your "heap". you will reserve/allocate and manage memory from this block.
//if you run out of memory or can't allocate any more, you should "throw" a
//segmentation fault (your turn to get back to those pesky segfaults that always
//bother you) and exit the program with an error (there's an example below)
//You won't need to extend/increase this heap.
//Note: In real OS, what happens when you do allocate more than what you have?
//      well, run "man sbrk" on your terminal, you'll learn how to use it in the OS class

// points to the beginning of the heap, this is the starting position of the first block
byte_ptr heap;

//just a toggle flag to initialize only once
static char mm_inited = 0;

// to initialize the heap (The Linked list)
void my_mminit()
{
    heap = (byte_ptr) malloc (HEAP_SIZE);
    if (heap == 0) {
        printf("ERROR during mm init malloc\n");
        exit(1);
    }
    mm_inited = 1;

    // initialze the first block of the heap.
    // this first block is initially a 32MB block, unused.
    Node *node =(Node *)malloc(sizeof(Node));     /*node is now a ptr to the beginning of a struct*/
    head = node;
    head->prev = NULL;
    head->next = NULL;
    head->bsize = HEAP_SIZE;
    head->start_pos = heap;
    head->is_used = 0;

    // initially, nothing is allocated, so the total used space is zero.
    total_used_space = 0;
}

//you shouldn't need to change this, but feel free to do so if you want
//it's just a switch to choose allocation policy during compile time
//and an init that only runs once
void* mm_malloc(size_t size)
{
    if (!mm_inited)
        my_mminit();

    #ifdef BESTFIT
        return mm_best_fit_malloc(size);
    #elif FIRSTFIT
        return mm_first_fit_malloc(size);
    #else
        printf("memory system policy undefined\n");
        exit(1);
    #endif
}


void mm_free(void* ptr)
{
    //TODO: do your work here

    //if ptr is invalid (does not point to the start
    //of a memory block you allocated, emulate a SEGFAULT:
    if (ptr == 0 || ptr < (void*)heap || ptr > (void*)(heap+HEAP_SIZE)) {
        printf("Segmentation fault.\n");
        exit(1);
    }

    // go through the entire LL, find the corresponding block
    Node* node = head;
    // flag to indicate whether we have freed or not.
    size_t freed = 0;

    // go through the heap until we have freed or we never found the pointer
    while(node != NULL && freed == 0){
        // if the pointer address matches the block's starting position
        if(node->start_pos == ptr){

            // if the node is used, then we can free it
            // if the node is not used, then it's a segmentation fault
            if(node->is_used == 1){
                node->is_used = 0;
                total_used_space -= node->bsize;

                //coalesce the blocks if possible
                //coalesce the current node and the previous node.
                if (node->prev != NULL && node->prev->is_used == 0){
                    Node* temp = node;
                    (node->prev)->bsize += node->bsize;
                    node->prev->next = node->next;
                    if(node->next != NULL){node->next->prev = node->prev;}
                    node = node->prev;
                    // after we coalesce, we have to free the pointer.
                    free(temp);
                }

                // coalesce the current node and the next node.
                if (node->next != NULL && node->next->is_used == 0){
                    Node* temp = node->next;
                    node->bsize += node->next->bsize;
                    node->next = node->next->next;
                    if(node->next != NULL){node->next->prev = node;}
                    // after coalescing, free this pointer that no longer points to an availabile node.
                    free(temp);
                }

                freed = 1;
            }else{    // if the pointer is a pointer that points an unused block - segfault
                printf("Segmentation fault. \n");
                exit(1);
            }

         }
        node = node->next;
    }
}

// best fit function - go through the heap to find a block,to find the block that has the
// smallest fitting block.
void* mm_best_fit_malloc (size_t size)
{
    if(size == 0){
        return 0;
    }
    Node* curr_node = head;

    int can_allocate = 0;
    size_t curr_size;

    // compare to the find the best fitting block
    Node* best_fit_block = head;
    size_t size_diff = head->bsize;

    while(curr_node != NULL){
        curr_size = curr_node->bsize;
        // we allocate it right away
        if(curr_size>=size && curr_node->is_used == 0){
            can_allocate = 1;
            if (curr_size == size){
                curr_node->is_used = 1;
                total_used_space += size;
                return curr_node->start_pos;
            }
            // if the size is less than curr block size
            // compare and decide what to allocate
            int temp = curr_size - size;
            if(temp < size_diff){
                best_fit_block = curr_node;
            }
        }
        curr_node = curr_node->next;
    }
    if(can_allocate == 1){
        Node *new_node = malloc(sizeof(Node));
        place_node(best_fit_block, new_node, size);
        total_used_space += size;
        return new_node->start_pos;
    }
    return 0;
}

// go through the heap to find the first fitting block with enough size
void* mm_first_fit_malloc (size_t size)
{
    if(size == 0){
        return 0;
    }
    Node* curr_node = head;
    int allocated = 0;
    size_t curr_size;

    // find the first avaiable block
    while(curr_node != NULL && !allocated){
        curr_size = curr_node->bsize;
        if(curr_size >= size && curr_node->is_used == 0){
            // allocate size block and make new node, if size == current block size, if size < current block size
            allocated = 1;

            if(curr_size == size){
                curr_node->is_used = 1;
                total_used_space += size;
                return curr_node->start_pos;
            }
            // if size is less than curr block. request a memory space to place a new node
            Node *new_node = malloc(sizeof(Node));
            // add the node to the end of the block
            place_node(curr_node, new_node, size);
            total_used_space += size;

            return new_node->start_pos;

        }
        curr_node = curr_node->next;
    }
    return 0;
}

// add a new node to the linked list
// add the node to the end of the curr block, change the block size and its other
// attributes, such as next/prev nodes, and availability
void place_node(Node* curr, Node* new, size_t size){
    new->prev = curr;
    new->next = curr->next;
    if(curr->next != NULL){
        curr->next->prev = new;
    }
    curr->next = new;
    new->bsize = size;
    int curr_size = curr->bsize;
    new->start_pos = curr->start_pos + curr_size - size;
    new->is_used = 1;
    curr->bsize = curr_size - size;
}

//this is the function that will be used to grade you, make sure to print in the
//correct format according to the pdf.
void mm_print_heap_status(FILE* fout)
{
    /*
     *   TODO: Your code here
     */

    /*
     *   Your code here. Because you are writing a file or maybe stdout (your terminal), instead of using printf, use fprintf
     *   It has the exact same syntax, but the first parameter is fout. This function assumes the file already exists.
     *   Check test.c if you want to see how this works. You shouldn't worry about it, just use fprintf as the example below.
     *   eg: fprintf(fout, "some string %d  %s\n", parameter1, parameter2)
     */

    Node* node = head;
    char availability;
    if (head != NULL){
        availability = node->is_used ? 'A':'F';
        fprintf(fout, "%lu%c", head->bsize, availability);
        node = node->next;
    }
    while(node != NULL){
        size_t size = node->bsize;
        availability = node->is_used ? 'A':'F';
        fprintf(fout, " %lu%c", size, availability);
        node = node->next;
    }
    fprintf(fout, "\n");
}

// coalesce all of the free blocks into a big empty block
// push all of the used block to the rightmost end of the heap
void  mm_defragment()
{
     size_t total_free_space = HEAP_SIZE - total_used_space;
     byte_ptr new_start_pos = HEAP_SIZE + total_free_space;

    // create the firts free block, the block size is already calculated with all the
    // malloc calls and free calls.
    // set the prev and next nodes and its availability
     Node *free_head = malloc(sizeof(Node));
    free_head->prev = NULL;
    free_head->next = head;
    free_head->bsize = total_free_space;
    free_head ->start_pos = heap;
    free_head->is_used = 0;
    head->prev = free_head;

    // nodes for going through the heap to update the used blocks
    Node* cur = head;
    Node* prev = free_head;

    // go through the heap, if the node is used, then update this node's starting position
    // and update its prev and next nodes, update the new starting position for the nodes
    // coming after it.
    while(cur != NULL){
        if(cur->is_used){
            cur->start_pos = new_start_pos;
            new_start_pos += cur->bsize;
            cur->prev = prev;
            prev->next = cur;
            prev = cur;
            cur = cur->next;
        }else{
            // if the node is not used, we remove the node from the heap aka the Linked List
            // Also, free the pointer by calling system free()
            // Set the prev and next nodes accordingly.
            Node* temp = cur;
            cur->prev->next = cur->next;
            if(cur->next != NULL){cur->next->prev = cur->prev;}
            cur = cur->next;
            free(temp);
        }
    }
    // update the head nodes to point to the free block at the beginning of the heap.
    head = free_head;
}

