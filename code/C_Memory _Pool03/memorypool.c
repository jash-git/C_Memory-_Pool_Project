#include "memorypool.h"

memorypool* memorypool_create(int block_request_size, int memory_init_quantity, _Bool memory_extend)
{//請求最大極限,記憶池數量=>記憶池大小(基底*數量),

    memorypool* mp = NULL;
    mp = (memorypool*)malloc(sizeof(memorypool));
    memset(mp, 0, sizeof(memorypool));
    mp->block_size = block_request_size;
    mp->memory_size = block_request_size + sizeof(memory);
    mp->auto_extend = memory_extend;

    for (int i = memory_init_quantity; i > 0; i--)
    {
        memory* mm = NULL;
        mm = memory_creat(mp->memory_size , mp);
    }
    return mp;
}


char* memory_malloc(memorypool* mp, int data_size)
{
    memory* p_memory = NULL;
    if (mp->block_size < data_size)
    {
        return NULL;
    }

    if (NULL == mp->free_list)
    {
        if (mp->auto_extend == 1)
            memory_creat(mp->memory_size, mp);
        else
            return NULL;
    }

    p_memory = mp->free_list;
    mp->free_list = mp->free_list->next;
    p_memory->next = NULL;
    if (mp->used_list)
    {
        mp->used_list->prev->next = p_memory;
        p_memory->prev = mp->used_list->prev;
    }
    else
    {
        mp->used_list = p_memory;
    }

    p_memory->next = mp->used_list;
    mp->used_list->prev = p_memory;
    mp->used_list = p_memory;
    mp->used_memory_cnt += 1;
    return p_memory->block;
}

int memorypool_info_get(memorypool* mp)
{
    printf("current mempool_size:%d\n\
           current memory_size:%d\n\
           current block_size:%d\n\
           total_memory_cnt:%d\nused_memory_cnt:%d\n",mp->mempool_size,mp->memory_size,mp->block_size, \
    mp->total_memory_cnt,mp->used_memory_cnt);
    return 0;
}

int memory_free(char** p_bk)
{
    memory* p_memory = (memory* )(*p_bk - sizeof(memory)/(sizeof(char)));//正确
    p_memory->next->prev = p_memory->prev;
    p_memory->prev->next = p_memory->next;
    p_memory->next = NULL;
    p_memory->prev = NULL;
    if (p_memory->mp->used_memory_cnt == 1)
    {
        p_memory->mp->used_list = NULL;
    }
    p_memory->next = p_memory->mp->free_list;
    p_memory->mp->free_list = p_memory;
    p_memory->mp->used_memory_cnt -= 1;
    *p_bk = NULL;
    return 0;
}

int memorypool_destroy(memorypool* mp)
{
    memory* mm = NULL;
    if (mp->free_list)
    {
        mm = mp->free_list;
        mp->free_list = mp->free_list->next;
        free(mm);
        mm = NULL;
    }
    if (mp->used_list)
    {
        mp->used_list->prev->next = NULL;
        mm = mp->used_list;
        mp->used_list = mp->used_list->next;
        free(mm);
        mm = NULL;
    }
    free(mp);

return 0;

}

memory* memory_creat(int memory_size, memorypool* mp)
{
    memory* mm = NULL;
    char* bk = NULL;
    char* memory_start = (char*)malloc(memory_size);
    memset(memory_start, 0, memory_size);
    mm = (memory*)memory_start;
    memset(mm, 0, sizeof(memory));
    bk = memory_start - sizeof(memory);//错误
    bk =memory_start +sizeof(memory)/(sizeof(char));//正确
    mm->memory_size = memory_size;
    mm->block_size = memory_size - sizeof(memory);
    mm->mp = mp;
    mm->block = bk;
    mm->next = mp->free_list;
    mp->free_list = mm;
    mp->total_memory_cnt += 1;
    mp->mempool_size += mp->memory_size;

    return mm;
}
