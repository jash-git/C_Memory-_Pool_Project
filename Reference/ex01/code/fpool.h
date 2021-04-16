/*
* fpool.h (fixed memory pool)
* author: xinghun_4
*/
#ifndef _FPOOL_H
#define _FPOOL_H
	int mem_pool_init(int base_size, int links);
	void mem_pool_destroy(void);
	void print_mem_pool_info(void);
	void *mem_malloc(int size);
	void mem_free(void *ptr);
#endif
