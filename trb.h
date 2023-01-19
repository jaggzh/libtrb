#ifndef _RINGBUFFER_H
#define _RINGBUFFER_H
#ifndef _IN_RINGBUFFER_C
#endif
#include <stdint.h>

/* Includer options */
// #define RB_NOMUNGE  // define to not create shortened macros (below)

#ifndef RB_DTYPE
	#define RB_DTYPE   uint16_t
#endif

#define RB_IDXTYPE uint8_t

struct ringbuffer_st {
    RB_DTYPE *d;          // data
    RB_IDXTYPE hd, tl;    // head tail indices
    RB_IDXTYPE sz;        // *total* buffer element count
	RB_DTYPE mn, mx;      // min max
};
typedef struct ringbuffer_st rb_st;

void ringbuffer_init(rb_st *rb, RB_IDXTYPE len);
void ringbuffer_add(rb_st *rb, RB_DTYPE v);
void ringbuffer_setall(rb_st *rb, RB_DTYPE v);
void ringbuffer_minmax(rb_st *rb); // stores within struct
void ringbuffer_median_filter(rb_st *rb, RB_IDXTYPE window_size);
void ringbuffer_median_filter2(rb_st *rb, rb_st *rb_med, RB_IDXTYPE window_size);
void ringbuffer_print(rb_st *rb);
int rbutil_cmpfunc(const void *a, const void *b);

#ifndef RB_NOMUNGE
# define rb_init(rb, len) ringbuffer_init(rb, len)
# define rb_setall(rb, v) ringbuffer_setall(rb, v)
# define rb_minmax(rb, v) ringbuffer_minmax(rb)
# define rb_median2(rb, rbdest, winsize) ringbuffer_median_filter2(rb, rbdest, winsize)
#endif // RB_NOMUNGE

#endif
