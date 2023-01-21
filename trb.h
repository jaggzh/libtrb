#ifndef _RINGBUFFER_H
#define _RINGBUFFER_H
#ifndef _IN_RINGBUFFER_C
#endif
#include <stdint.h>

/* Includer options */
// #define RB_NO_SHORT_DEFS  // define to not create shortened macros (below)

#ifndef RB_DTYPE
	#define RB_DTYPE   uint16_t
#endif

#define RB_IDXTYPE uint8_t

struct ringbuffer_st {
    RB_DTYPE *d;          // data
    RB_IDXTYPE nx;        // Pointer to next storage location
    //RB_IDXTYPE hd, tl;    // head tail indices
    RB_IDXTYPE sz;        // *total* buffer element count
	RB_DTYPE mn, mx;      // min max
};
typedef struct ringbuffer_st rb_st;

void ringbuffer_init(rb_st *rb, RB_IDXTYPE len);
void ringbuffer_free(rb_st *rb); // frees only rb->d
void ringbuffer_add(rb_st *rb, RB_DTYPE v);
void ringbuffer_setall(rb_st *rb, RB_DTYPE v);
RB_DTYPE ringbuffer_get_last(rb_st *rb, RB_IDXTYPE noff); // 0=last, 1 is prior, ...
void ringbuffer_minmax(rb_st *rb); // stores mn,mx within struct
// Filter entire buffer
void ringbuffer_median_filter_inplace(rb_st *rb, RB_IDXTYPE window_size);
void ringbuffer_median_filter(rb_st *rb, rb_st *rb_med, RB_IDXTYPE window_size);
// Return single value median of entire buffer. (Even ->sz is 1 higher than center)
RB_DTYPE ringbuffer_median_filter1(rb_st *rb);
void ringbuffer_print(rb_st *rb);

int rbutil_cmpfunc(const void *a, const void *b);

#ifndef RB_NO_SHORT_DEFS
# define rb_init(rb, len) ringbuffer_init(rb, len)
# define rb_free(rb) ringbuffer_free(rb)
# define rb_setall(rb, v) ringbuffer_setall(rb, v)
# define rb_add(rb, v) ringbuffer_add(rb, v)
# define rb_get_last(rb, noff) ringbuffer_get_last(rb, noff)
# define rb_minmax(rb) ringbuffer_minmax(rb)
# define rb_median_inplace(rb, winsize) \
             ringbuffer_median_filter_inplace(rb, rbdest, winsize)
# define rb_median(rb, rbdest, winsize) \
             ringbuffer_median_filter(rb, rbdest, winsize)
# define rb_median1(rb) ringbuffer_median_filter1(rb)
# define rb_print(rb) ringbuffer_print(rb)
#endif // RB_NO_SHORT_DEFS

#endif
