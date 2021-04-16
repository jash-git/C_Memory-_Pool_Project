#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "memorypool.h"

void print_info_line(char *data)
{
	printf("%s---%d\t%s\n",__FILE__, __LINE__,data);
}
int main()
{
    memorypool* mp = NULL;

    int memory_init_quantity = 5;
    _Bool memory_extend = 1;
    int block_request_size =1024;

    time_t start = 0;
    time_t end = 0;
    int count = 0;

    mp = memorypool_create(block_request_size, memory_init_quantity, memory_extend);//1024,5,1
    print_info_line("memorypool_create");
    memorypool_info_get(mp);

    struct test
    {
        int a;
        int b;
    };
    struct test* p_data;

    /*
    start = time(NULL);
    int i=10;

    while (i>0)
    {
        p_data = (struct test*)memory_malloc(mp, sizeof(struct test));
        //p_data = (struct test*)malloc(sizeof(struct test));
        memorypool_info_get(mp);
        p_data->a = 100;

        memory_free((char**)&p_data);
        memorypool_info_get(mp);
        //free(p_data);

        end = time(NULL);
        count += 1;
        if (end == (start + 300))
        {
            printf("%d", count);
            break;
        }
        i--;
    }
    */

    p_data = (struct test*)memory_malloc(mp, sizeof(struct test));
    p_data->a = 100;
    p_data->b = 200;
    int Sum = p_data->a + p_data->b;
    printf("Sum=%d\n",Sum);
    print_info_line("memory_malloc");
    memorypool_info_get(mp);

    memory_free((char**)&p_data);
    print_info_line("memory_free");
    memorypool_info_get(mp);

    memorypool_destroy(mp);
    print_info_line("memorypool_destroy");

    return 0;
}
