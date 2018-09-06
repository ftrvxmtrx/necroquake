typedef struct {
	mem_pool_t *pool;
	void *data;
	int sz;
	int next_pow2;
}buf_t;

buf_t *buf_alloc(mem_pool_t *pool) gnuc_attr(nonnull(1), malloc);
int buf_write(buf_t *b, const void *data, int sz) gnuc_attr(nonnull(1, 2));
int buf_puts(buf_t *b, const char *s) gnuc_attr(nonnull(1, 2));
void buf_clear(buf_t *b) gnuc_attr(nonnull(1));
void buf_free(buf_t *b) gnuc_attr(nonnull(1));
