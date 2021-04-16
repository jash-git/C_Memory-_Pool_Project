***********************mem_pool_debug.c********************/

#include <stdio.h>
#include <stdlib.h>
#include "mem_pool.h"

#define ALLOC_COUNT 10

void alloc_test(char *ptr[])
{
    int i, j;

    for(i = 0; i < ALLOC_COUNT; i++)
    {
        if( (ptr[i] = mem_alloc()) == NULL )
        {
            fprintf(stderr, "mem_alloc error\n");
            return;
        }
        for(j = 0; j < ALLOC_COUNT; j++)
        {
            ptr[i][j] = 'a' + j;
        }
    }
    for(i = 0; i < ALLOC_COUNT; i++)
    {
        for(j = 0; j < ALLOC_COUNT; j++)
        {
            printf("ptr[%d][%d]=%c ", i, j, ptr[i][j]);
        }
        fputc('\n', stdout);
    }
}


int main(int argc, char *argv[])
{
    int base, step;
    char *ptr1[ALLOC_COUNT], *ptr2[ALLOC_COUNT];

    switch(argc)
    {
        case 1:
            base = 0; /* default count */
            step = 0; /* default count */
            break;
        case 2:
            base = atoi(argv[1]);
            step = 0;
            break;
        case 3:
            base = atoi(argv[1]);
            step = atoi(argv[2]);
            break;
        default:
            fprintf(stderr, "Usage: %s [ [step]]\n", argv[0]);
            break;
    }

    if( !mem_pool_init(base, step) )
    {
        fprintf(stderr, "mem_pool_init error\n");
        return 1;
    }
    print_mem_pool_info();
    alloc_test(ptr1);
    print_mem_pool_info();

    //mem_free(ptr1[5]);

    print_mem_pool_info();

    alloc_test(ptr2);
    print_mem_pool_info();

    mem_pool_destroy();


    /* once again */
    if( !mem_pool_init(base, step) )
    {
        fprintf(stderr, "mem_pool_init error\n");
        return 1;
    }
    print_mem_pool_info();
    alloc_test(ptr1);
    print_mem_pool_info();

    mem_free(ptr1[5]);
    print_mem_pool_info();

    alloc_test(ptr2);
    print_mem_pool_info();

    mem_pool_destroy();

    return 1;
}