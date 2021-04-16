/***********************mem_pool.h************************
 *
 * * author:bripengandre
 * * modified by: LaoLiulaoliu
 * * TODO: thread_safe, different model
 *
 * *********************************************************/

#ifndef _MEM_POOL_H_
#define _MEM_POOL_H_

#define BUF_SIZE 100
#define BASE_COUNT 10000
#define STEP_COUNT 1000


typedef union _mem_node
{
    char buf[BUF_SIZE];
    union _mem_node *next;
} mem_node_t, *pmem_node_t;

/* consider of the efficiency, node_cnt can be annotated */
/* used to store block information */
typedef struct _mem_block
{
    mem_node_t *node_head;
    mem_node_t *node_tail;
    int node_cnt; /* node count */
    struct _mem_block *next;
} mem_block_t, *pmem_block_t;

/* consider of the efficiency, block_cnt can be annotated */
/* used to store the pool information */
typedef struct _mem_pool
{
    mem_block_t *block_head;
    // mem_block_t *block_tail;

    mem_node_t *free_head;
    int block_cnt; /* block count */
    int free_cnt; /* free node count; */
    int base;
    int step;
} mem_pool_t, *pmem_pool_t;


/* mem_pool will have at least base blocks, and will increase steps a time if needed */
int mem_pool_init(int base, int step);
void mem_pool_destroy(void);
void print_mem_pool_info(void);

/* since the block size is constant, this function need no input parameter */
void *mem_alloc(void);
void mem_free(void *ptr);


#endif /* _MEM_POOL_H */