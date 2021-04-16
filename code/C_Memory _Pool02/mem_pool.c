/***********************mem_pool.c************************
 *
 * * author:bripengandre
 * * modified by: LaoLiulaoliu
 *
 * *********************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mem_pool.h"


/* static functions are the mainly expense of cpu in memory pool */
/* add new memory block to our memory pool */
static int add_mem_block(int cnt);
/* init the new block */
static int mem_block_init(int cnt, mem_block_t *block);
/* init free_list of the new block */
static int free_list_init(const mem_block_t *block);

static mem_pool_t mem_pool;

int mem_pool_init(int base, int step)
{
    if(base <= 0)
    {
        base = BASE_COUNT;
    }
    if(step <= 0)
    {
        step = STEP_COUNT;
    }

    /* initiate mem_pool */
    memset( &mem_pool, 0, sizeof(mem_pool) );
    mem_pool.base = base;
    mem_pool.step = step;

    /* add the base block(node of base count) into the memory pool */
    if( !add_mem_block(base) )
    {
        fprintf(stderr, "mem_pool_init::add_mem_block error\n");
        return 0;
    }

    return 1;
}


void mem_pool_destroy(void)
{
    mem_block_t *prev_block, *cur_block;

    prev_block = NULL;
    cur_block = mem_pool.block_head;
    while(cur_block != NULL)
    {
        prev_block = cur_block;
        cur_block = cur_block->next;
        /* mem_block_init() malloc once,so just free once of the head pointer */
        free(prev_block->node_head);
        free(prev_block);
    }

    memset( &mem_pool, 0, sizeof(mem_pool_t) );
}


void print_mem_pool_info(void)
{
    int i;
    mem_block_t *p;

    if(mem_pool.block_head == NULL)
    {
        fprintf(stderr, "memory pool has not been created!\n");
        return;
    }
    printf("***************memory pool information start*******************\n");
    printf("base block size: %d\n", mem_pool.base);
    printf("increasing block size: %d\n", mem_pool.step);
    printf("block count: %d\n", mem_pool.block_cnt);
    printf("current free node count: %d\n", mem_pool.free_cnt);
    printf("the first block: %#x\n", mem_pool.block_head);
    //printf("the last block: %#x\n", mem_pool.block_tail);

    printf("the first free node: %#x\n\n", mem_pool.free_head);
    for(p = mem_pool.block_head, i = 0; p != NULL; p = p->next, i++)
    {
        printf("-----------------block %d-----------------------\n", i+1);
        printf("node count: %d\n", p->node_cnt);
        printf("the first node: %#x\n", p->node_head);
        printf("-------------------------------------------------\n");
    }
    printf("***************memory pool information end*********************\n\n");
}


void *mem_alloc(void)
{
    mem_node_t *p;

    /* no free node ready, attempt to allocate new free node */
    if(mem_pool.free_head == NULL)
    {
        if( !add_mem_block(mem_pool.step) )
        {
            fprintf(stderr, "mem_alloc::add_mem_block error\n");
            return NULL;
        }
    }

    /* get free node from free_list */
    p = mem_pool.free_head;
    mem_pool.free_head = p->next;

    /* decrease the free node count */
    mem_pool.free_cnt--;

    return p;
}


void mem_free(void *ptr)
{
    if(ptr == NULL)
    {
        return;
    }

    /* return the node to free_list */
    ((mem_node_t *)ptr)->next = mem_pool.free_head;
    mem_pool.free_head = ptr;

    /* increase the free node count */
    mem_pool.free_cnt++;
}


static int add_mem_block(int cnt)
{
    mem_block_t *block;

    if( (block = malloc(sizeof(mem_block_t))) == NULL )
    {
        fprintf(stderr, "mem_pool_init::malloc block error\n");
        return 0;
    }

    if( !mem_block_init(cnt, block) )
    {
        fprintf(stderr, "mem_pool_init::mem_block_init error\n");
        return 0;
    }

    /* insert the new block in the head */
    /* for the first time, block->next == NULL */
    block->next = mem_pool.block_head;
    mem_pool.block_head = block;
    // if(mem_pool.block_tail == NULL)
    // {
    //     mem_pool.block_tail = block;
    // }

    /* insert the new block into the free list */
    /* block->node_tail->next == NULL in these two situations of add_mem_block() */
    block->node_tail->next = mem_pool.free_head;
    mem_pool.free_head = block->node_head;
    mem_pool.free_cnt += cnt;

    /* increase the block count */
    mem_pool.block_cnt++;

    return 1;
}


static int mem_block_init(int cnt, mem_block_t *block)
{
    size_t size;
    mem_node_t *p;

    if(block == NULL)
    {
        return 0;
    }

    size = cnt * sizeof(mem_node_t);
    if( (p = malloc(size)) == NULL )
    {
        fprintf(stderr, "mem_pool_init::malloc node error\n");
        return 0;
    }
    memset(p, 0, size);
    memset(block, 0, sizeof(mem_block_t));
    block->node_cnt = cnt;
    block->node_head = p;
    block->node_tail = p+cnt-1;
    free_list_init(block);

    return 1;
}


static int free_list_init(const mem_block_t *block)
{
    mem_node_t *p, *end;

    if(block == NULL)
    {
        return 0;
    }

    /* start initiating free list */
    end = block->node_tail; /* block_cnt > 0 */
    for(p = block->node_head; p < end; p++)
    {
        p->next = (p+1);
    }
    p->next = NULL; /* end->next = NULL */

    return 1;
}

