#include "my_mem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    //write the output of mm_print_heap_status to STDOUT (your terminal), instead of a file
    FILE* fout = stdout;

    printf("Let's check if this thing works..\n");

    char* buf = mm_malloc(16);
    if (buf == 0) {
        printf("looks like we cant allocate memory :(\n");
        return 1;
    }

    memset(buf, 'Q', 15);
    buf[15] = 0;

    printf("should print 15 Q's:  %s\n", buf);

    printf("You should see 33554416F 16A next:\n");
    mm_print_heap_status(fout);

    char* not_used = mm_malloc(256);

    printf("You should see 33554160F 256A 16A next:\n");
    mm_print_heap_status(fout);

    mm_free(buf);

    printf("You should see 33554160F 256A 16F next:\n");
    mm_print_heap_status(fout);

    printf("Running defrag. You should see 33554176F 256A next:\n");
    mm_defragment();
    mm_print_heap_status(fout);

    printf("That's it for now, just a VERY simple test, you can extend this or use the trace reader to do more complicated ones\n");

    return 0;
}
