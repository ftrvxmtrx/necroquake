#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "mem.h"

struct mem_alloc_t {
	mem_pool_t *pool;
	mem_alloc_t *next;
};

void
mem_pool_free(mem_pool_t *pool)
{
	mem_alloc_t *a, *next;

	for (a = pool->allocs; a != NULL; a = next) {
		next = a->next;
		free(a);
	}

	pool->allocs = NULL;
}

void *
mem_alloc(mem_pool_t *pool, int sz)
{
	mem_alloc_t *a;

	a = malloc(sizeof(*a) + sz);
	a->pool = pool;
	a->next = pool->allocs;
	pool->allocs = a;

	return a+1;
}

void *
mem_realloc(mem_pool_t *pool, void *m, int sz)
{
	mem_alloc_t *a, *n, *pa, *prev;

	if (pool != NULL && m == NULL)
		return mem_alloc(pool, sz);

	a = (mem_alloc_t*)m - 1;
	if ((n = realloc(a, sizeof(*a) + sz)) == NULL)
		return NULL;
	for (prev = NULL, pa = a->pool->allocs; pa != NULL && pa != a; prev = pa, pa = pa->next);
	if (prev != NULL)
		prev->next = n;
	else
		n->pool->allocs = n;

	return n+1;
}

void *
mem_realloc_pow2(mem_pool_t *pool, void *m, int elsz, int currnum)
{
	mem_alloc_t *a, *n, *pa, *prev;

	if (((currnum - 1) & currnum) != 0)
		return m;

	if (pool != NULL && m == NULL)
		return mem_alloc(pool, elsz*currnum);

	a = (mem_alloc_t*)m - 1;
	if ((n = realloc(a, elsz*currnum*2)) == NULL)
		return NULL;
	for (prev = NULL, pa = a->pool->allocs; pa != NULL && pa != a; prev = pa, pa = pa->next);
	if (prev != NULL)
		prev->next = n;
	else
		n->pool->allocs = n;

	return n+1;
}

char *
mem_strdup(mem_pool_t *pool, const char *s)
{
	char *copy;
	int len;

	len = strlen(s);
	copy = mem_alloc(pool, len);
	memmove(copy, s, len+1);

	return copy;
}

void
mem_free(void *m)
{
	mem_alloc_t *a, *pa, *prev;

	if (m == NULL)
		return;

	a = (mem_alloc_t*)m - 1;
	for (prev = NULL, pa = a->pool->allocs; pa != NULL && pa != a; prev = pa, pa = pa->next);
	if (prev != NULL)
		prev->next = a->next;
	else
		a->pool->allocs = a->next;
	free(a);
}
