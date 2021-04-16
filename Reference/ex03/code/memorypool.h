#ifndef _MEMORYPOOL_H_
#define _MEMORYPOOL_H_
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
typedef struct Memorypool
{
    int mempool_size;
    int memory_size;//block_size + sizeof(manager information) = mem_size
    int block_size;
    int total_memory_cnt;
    int used_memory_cnt;
    int auto_extend;
    struct Memory* free_list;
    struct Memory* used_list;
}memorypool;
 
typedef struct Memory
{
    int memory_size;
    int block_size;
    struct Memory* prev;
    struct Memory* next;
    struct Memorypool* mp;
    char* block;
}memory;
memorypool* memorypool_create(int block_request_size, int memory_init_quantity, _Bool memory_extend);
char* memory_malloc(memorypool* mp,int data_size);
int memorypool_info_get(memorypool* mp);
int memory_free(char** p_bk);
int memorypool_destroy(memorypool* mp);
memory* memory_creat(int memory_size, memorypool* mp); 
#endif
