#include <string.h>
#include "util.h"
#include "mem.h"
#include "buf.h"

buf_t *
buf_alloc(mem_pool_t *pool)
{
	buf_t *b;

	b = mem_alloc(pool, sizeof(*b));
	memset(b, 0, sizeof(*b));
	b->pool = pool;

	return b;
}

int
buf_write(buf_t *b, const void *data, int sz)
{
	int nsz, npow2;

	nsz = b->sz + sz;
	npow2 = next_pow2(nsz);
	if (b->next_pow2 <= npow2) {
		b->next_pow2 = npow2;
		b->data = mem_realloc(b->pool, b->data, npow2);
	}
	memmove((char*)b->data + b->sz, data, sz);
	b->sz = nsz;

	return nsz;
}

int
buf_puts(buf_t *b, const char *s)
{
	return buf_write(b, s, strlen(s));
}

void
buf_clear(buf_t *b)
{
	b->sz = 0;
}

void
buf_free(buf_t *b)
{
	mem_free(b->data);
	mem_free(b);
}
