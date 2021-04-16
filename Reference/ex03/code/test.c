#include "memorypool.h"
#include <time.h>
 
int main() 
{
    memorypool* mp = NULL;

    int memory_init_quantity = 5;
    bool memory_extend = 1;
    int block_request_size =1024;
    
	time_t start = 0;
    time_t end = 0;
    int count = 0;
    
	mp = memorypool_create(block_request_size, memory_init_quantity, memory_extend);//1024,5,1
    
	struct test 
    {
        int a;
    };
    struct test* p_data;

    start = time(NULL);
    while (1)
    {
        p_data = (struct test*)memory_malloc(mp, sizeof(struct test));
        //p_data = (struct test*)malloc(sizeof(struct test));
        p_data->a = 100;
        memory_free((char**)&p_data);
        //free(p_data);
        end = time(NULL);
        count += 1;
        if (end == (start + 300))
        {
            printf("%d", count);
            break;
        }
    }

    /*
    p_data = (struct test*)memory_malloc(mp, sizeof(struct test));
    memory_free((char**)&p_data[0]);
    memorypool_info_get(mp);
    memorypool_destroy(mp);
    */
 
    system("pause");
    return 0; 
}
