/*
* fpool.c (fixed memory pool)
* author: xinghun_4
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fpool.h"

#define MEM_POOL_DEBUG
#define DEFAULT_BASE_SIZE 4
#define DEFAULT_LINKS 4
#define DEFAULT_BLOCKS 4
#define DEFAULT_INCREMENT_LINKS 4
#define DEFAULT_INCREMENT_BLOCKS 4
#define ALIGN(ADD, SIZE) (((ADD) + ((SIZE) - 1)) & ~((SIZE) - 1))

/* 声明内存块, 前向引用 */
typedef struct _mem_block mem_block_t;

/* 内存节点指针 */
typedef union _mem_node
{
	mem_block_t *b_next; /* 指向下一块内存节点 */
	char data[1]; /* 内存节点首地址 */
}mem_node_t;

/* 内存块 */
struct _mem_block
{
	short index; /* 内存链索引, mem_free 的时候有用 */
	mem_node_t n_node; /* 内存节点 */
	char pad[2];
};

/* 内存链 */
typedef struct _mem_link
{
	struct _mem_link *l_next; /* 指向下一条内存链 */
	short index; /* 内存链索引 */
	short blocks; /* 内存链内存块数 */
	short free_blocks; /* 内存链空闲内存块数 */
	char pad[2];
	mem_block_t *b_head; /* 内存链首内存块 */
}mem_link_t;

/* 内存池 */
typedef struct _mem_pool
{
	short links; /* 内存链数 */
	short base_size; /* 内存节点为base_size的倍数 */
	mem_link_t *l_head; /* 首内存链 */
	mem_link_t *l_tail; /* 尾内存链 */
}mem_pool_t;
static mem_pool_t mem_pool;

/* add new memory links to our memory pool */
static int add_mem_link(int links);

/* add new memory blocks to our memory link */
static int add_mem_block(mem_link_t *link, int blocks);

/*
* @base_size: 节点大小单位
* @links: 内存链条数
*/
int mem_pool_init(int base_size, int links)
{
	if(base_size <= 0)
	{
		base_size = DEFAULT_BASE_SIZE;
	}
	else
	{
		/* base_size 为 4 的倍数 */
		base_size = ALIGN(base_size, DEFAULT_BASE_SIZE);
	}
	if(links <= 0)
	{
		links = DEFAULT_LINKS;
	}
	/* initiate mem_pool */
	memset(&mem_pool, 0, sizeof(mem_pool));
	mem_pool.base_size = base_size;
	/* add links into the memory pool */
	if(!add_mem_link(links))
	{
		fprintf(stderr, "mem_pool_init::add_mem_link error\n");
		return 0;
	}
	return 1;
}

void mem_pool_destroy(void)
{
	mem_link_t *cur_l;
	mem_block_t *cur_b;

	cur_l = mem_pool.l_head;
	while(cur_l)
	{/* 释放内存链 */
		mem_pool.l_head = mem_pool.l_head->l_next; /* 指向下一条内存链 */
		cur_b = cur_l->b_head;
		while(cur_b)
		{/* 释放所有内存块 */
			cur_l->b_head = cur_l->b_head->n_node.b_next; /* 指向下个内存块 */
			free(cur_b);
			cur_b = cur_l->b_head;
		}
		cur_l = mem_pool.l_head;
	}
	memset(&mem_pool, 0, sizeof(mem_pool_t));
}

void print_mem_pool_info(void)
{
	int i;
	mem_link_t *cur_l = NULL;
	mem_block_t *cur_b = NULL;

	cur_l = mem_pool.l_head;
	if(NULL == cur_l)
	{
		printf("/nmemory pool is not exist!\n");
		return;
	}
	/* memory pool info */
	printf("/n/************** Memory pool information **************/\n");
	#ifdef MEM_POOL_DEBUG
	printf("sizeof(mem_node_t)        : %d\n", sizeof(mem_node_t));
	printf("sizeof(mem_block_t)       : %d\n", sizeof(mem_block_t));
	printf("sizeof(mem_link_t)        : %d\n", sizeof(mem_link_t));
	printf("sizeof(mem_pool_t)        : %d\n", sizeof(mem_pool_t));
	#endif
	printf("links                     : %d\n", mem_pool.links);
	printf("base_size                 : %d\n", mem_pool.base_size);
	printf("link head pointer         : %x(%d)\n", (int)mem_pool.l_head, (int)mem_pool.l_head);
	printf("link tail pointer         : %x(%d)\n", (int)mem_pool.l_tail, (int)mem_pool.l_tail);
	while(cur_l)
	{/* 打印内存链 */
		printf("/n#### memory link %d ####\n", cur_l->index);
		printf("blocks                    : %d\n", cur_l->blocks);
		printf("free blocks               : %d\n", cur_l->free_blocks);
		printf("block size                : %d\n", cur_l->index * mem_pool.base_size);
		printf("block head pointer        : %x(%d)\n", (int)cur_l->b_head, (int)cur_l->b_head);
		cur_b = cur_l->b_head;
		i = 0;
		while(cur_b)
		{
			printf("memory block %d            : %x(%d)\n", i++, (int)cur_b, (int)cur_b);
			cur_b = cur_b->n_node.b_next;
		}
		cur_l = cur_l->l_next;
	}
	printf("/*****************************************************/\n");
}

void *mem_malloc(int size)
{
	int align_size = ALIGN(size, mem_pool.base_size);
	int index = align_size / mem_pool.base_size - 1;
	mem_link_t *cur_l = mem_pool.l_head;
	mem_block_t *cur_b = NULL;
	mem_node_t *cur_n = NULL;
	int i;
	short *p_index;

	if(index >= mem_pool.links)
	{
		printf("no such size(%d) memory block!\n", align_size);
		return NULL;
	}
	/* go to the right link */
	for(i = 0; i < index; ++i)
	{
		cur_l = cur_l->l_next;
	}
	if(0 >= cur_l->free_blocks)
	{/* no free blocks, expand memory link */
		if(!add_mem_block(cur_l, DEFAULT_INCREMENT_BLOCKS))
		{
			fprintf(stderr, "mem_malloc::add_mem_block error\n");
			return NULL;
		}
	}
	/* get free block from block head pointer */
	cur_b = cur_l->b_head;
	cur_l->free_blocks--;
	/* modify memory link node head pointer */
	cur_l->b_head = cur_l->b_head->n_node.b_next;
	p_index = (short *)cur_b;
	/* 记录索引, 便于 mem_free 找到正确的内存链 */
	*p_index = cur_l->index;
	++p_index;
	cur_n = (mem_node_t *)p_index;

	return (void *)&(cur_n->data[0]);
}

void mem_free(void *ptr)
{
	short *p_index = (short *)ptr;
	mem_link_t *cur_l = mem_pool.l_head;
	mem_block_t *cur_b = NULL;
	int i;

	--p_index;
	/* go to the right link */
	for(i = 0; i < *p_index; ++i)
	{
		cur_l = cur_l->l_next;
	}
	cur_b = (mem_block_t *)p_index;
	/* 内存链头部插入空闲内存块 */
	cur_b->n_node.b_next = cur_l->b_head;
	cur_l->b_head = cur_b;
	cur_l->free_blocks++;
}

/* 返回成功添加的内存链条数 */
static int add_mem_link(int links)
{
	mem_link_t *new_l = NULL;
	int index = 0;
	int i = links;
	int cnt = 0;

	/* 求得新添加链的索引 */
	if(NULL == mem_pool.l_head)
	{
		index = 0;
	}
	else
	{
		index = mem_pool.l_tail->index + 1;
	}
	while(i--)
	{
		if(NULL == (new_l = (mem_link_t *)malloc(sizeof(mem_link_t))))
		{
			fprintf(stderr, "add_mem_link::malloc mem_link_t error\n");
			continue;
		}
		/* 初始化新内存链 */
		memset(new_l, 0, sizeof(mem_link_t));
		new_l->index = index++;
		/* 新内存链加入内存池 */
		if(NULL == mem_pool.l_head)
		{
			mem_pool.l_head = new_l;
			mem_pool.l_tail = new_l;
		}
		else
		{
			mem_pool.l_tail->l_next = new_l;
			mem_pool.l_tail = new_l;
		}
		mem_pool.links++;
		++cnt;
		if(!add_mem_block(new_l, DEFAULT_INCREMENT_BLOCKS))
		{
			fprintf(stderr, "add_mem_link::add_mem_block error\n");
		}
	}

	return cnt;
}

static int add_mem_block(mem_link_t *link, int blocks)
{
	mem_block_t *new_b = NULL;
	int i = blocks;
	int cnt = 0;

	if(NULL == link)
	{
		fprintf(stderr, "memory link is NULL!\n");
		return 0;
	}
	#ifdef MEM_POOL_DEBUG
	printf("add_mem_block::block_size: %d\n", sizeof(short) + mem_pool.base_size * (link->index + 1));
	#endif
	while(i--)
	{
		if(NULL == (new_b = (mem_block_t *)malloc(sizeof(short) + mem_pool.base_size * (link->index + 1))))
		{
			fprintf(stderr, "add_mem_block::malloc mem_block_t error\n");
			continue;
		}
		/* 初始化新内存块 */
		memset(new_b, 0, sizeof(mem_block_t));
		new_b->index = link->index;
		/* 新内存块加入内存链 */
		if(NULL == link->b_head)
		{
			link->b_head = new_b;
		}
		else
		{
			new_b->n_node.b_next = link->b_head;
			link->b_head = new_b;
		}
		link->blocks++;
		link->free_blocks++;
		++cnt;
	}
	return cnt;
}
