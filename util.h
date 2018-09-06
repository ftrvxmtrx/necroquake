#ifdef __GNUC__
#define gnuc_attr(a...) __attribute__((a))
#else
#define gnuc_attr(a...)
#endif

#define nelem(x) (int)(sizeof(x)/sizeof((x)[0]))
#define USED(x) (void)x
#define MAX(a, b) ((a) < (b) ? (b) : (a))
#define next_pow2(x) (1 + ((x) | (x)>>1 | (x)>>2 | (x)>>4 | (x)>>8 | (x)>>16))
