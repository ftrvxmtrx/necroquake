typedef struct mem_alloc_t mem_alloc_t;

typedef struct {
	mem_alloc_t *allocs;
}mem_pool_t;

void mem_pool_free(mem_pool_t *pool) gnuc_attr(nonnull(1));

void *mem_alloc(mem_pool_t *pool, int sz) gnuc_attr(nonnull(1), malloc);
void *mem_realloc(mem_pool_t *pool, void *m, int sz) gnuc_attr(nonnull(1));
void *mem_realloc_pow2(mem_pool_t *pool, void *m, int elsz, int currnum) gnuc_attr(nonnull(1));
char *mem_strdup(mem_pool_t *pool, const char *s) gnuc_attr(nonnull(1, 2), malloc);
void mem_free(void *m);
