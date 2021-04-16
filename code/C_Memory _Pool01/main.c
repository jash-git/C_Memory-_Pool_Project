#include <stdio.h>
#include <stdlib.h>
#include "fpool.h"

void mempool_test_index0(void)
{
     int *t1 = (int *)mem_malloc(sizeof(int));
     int *t2 = NULL;
     int value = 3;

     if(NULL == t1)
     {
		printf("mempool_test_index0:: mem_malloc t1 NULL\n");
        return;
     }
     print_mem_pool_info();

     if(NULL == (t2 = (int *)mem_malloc(sizeof(int))))
     {
		printf("mempool_test_index0:: mem_malloc t2 NULL\n");
		return;
     }

     print_mem_pool_info();

     *t1 = value;
     printf("mempool_test01:: t1: %d\n", *t1);
     *t1 *= *t1;
     *t2 = *t1;
     (*t2)++;
     printf("mempool_test01:: t1: %d\n", *t1);
     printf("mempool_test01:: t2: %d\n", *t2);

     mem_free(t1);
     t1 = NULL; /* 不要忘了, 否则若两指针同时指向一块内存会出问题 */
     print_mem_pool_info();
     mem_free(t2);
     t2 = NULL;
     print_mem_pool_info();
}

int main()
{
	if(!mem_pool_init(0, 0))
	{
		return -1;
	}
	print_mem_pool_info();

	mempool_test_index0();

	mem_pool_destroy();

	print_mem_pool_info();
    return 0;
}
