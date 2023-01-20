/* ringbuffer.c */
#define _IN_RINGBUFFER_C
#include "trb.h"
#include <stdlib.h>
#include <stdio.h>

void ringbuffer_init(rb_st *rb, RB_IDXTYPE len) {
	//struct *rb = malloc(sizeof struct ringbuffer_st);
	if (len<1) len=1; // minimum. sorry.
	if (!(rb->d = malloc(sizeof(*rb->d) * len))) abort();
	rb->hd=rb->tl=0;
	rb->sz = len;
}

void ringbuffer_setall(rb_st *rb, RB_DTYPE v) {
	for (int i=0; i<rb->sz; i++) rb->d[i] = v;
}

void ringbuffer_add(rb_st *rb, RB_DTYPE v) {
	rb->d[rb->hd] = v;
	rb->hd++;
	if (rb->hd >= rb->sz) rb->hd = 0;
}

void ringbuffer_minmax(rb_st *rb) {
	/* rb->mn=rb->d[0]; */
	/* rb->mx=rb->d[0]; */
	for (int i=1; i<rb->sz; i++) {
		if (rb->mn > rb->d[i]) rb->mn = rb->d[i];
		if (rb->mx < rb->d[i]) rb->mx = rb->d[i];
	}
}

/* This call writes to a temporary buffer, then updates *rb
 * window_size must be >= 2, and is divided by 2,
 *  and really is best understood as needing to be an odd number
 *  (start = location - windowsize/2)
 */
void ringbuffer_median_filter(rb_st *rb, RB_IDXTYPE window_size) {
	if (window_size < 2) abort(); // better than nothing
    RB_DTYPE temp_data[sizeof(RB_DTYPE) * rb->sz];
    for (int i = 0; i < rb->sz; i++) {
        temp_data[i] = rb->d[i];
    }
    for (int i = 0; i < rb->sz; i++) {
        int start = i - window_size / 2;
        int end = i + window_size / 2;
        RB_DTYPE median_data[sizeof(RB_DTYPE) * (end - start + 1)];
        if (start < 0) start = 0;
        if (end >= rb->sz) end = rb->sz - 1;
        for (int j = start; j <= end; j++) {
            median_data[j - start] = temp_data[j];
        }
        qsort(median_data, end - start + 1, sizeof(RB_DTYPE), rbutil_cmpfunc);
        rb->d[i] = median_data[(end - start + 1) / 2];
    }
}

/* Filter2 writes to a caller-provided secondary ringbuffer
 * window_size must be >= 2, and is divided by 2,
 *  and really is best understood as needing to be an odd number
 *  (start = location - windowsize/2)
 */
void ringbuffer_median_filter2(rb_st *rb, rb_st *rb_med, RB_IDXTYPE window_size) {
    //RB_DTYPE *temp_data = alloca(sizeof(RB_DTYPE) * rb->sz);
    for (int i = 0; i < rb->sz; i++) {
    	/* printf("[%d]\n", i); fflush(stdout); */
        int start = i - window_size / 2;
        int end = i + window_size / 2;
        RB_DTYPE median_data[sizeof(RB_DTYPE) * (end - start + 1)];
        if (start < 0) start = 0;
        if (end >= rb->sz) end = rb->sz - 1;
        for (int j = start; j <= end; j++) {
            median_data[j - start] = rb->d[j];
        }
        qsort(median_data, end - start + 1, sizeof(RB_DTYPE), rbutil_cmpfunc);
        rb_med->d[i] = median_data[(end - start + 1) / 2];
    }
}

void ringbuffer_print(rb_st *rb) {
	printf("[%d:%d] ", rb->mn, rb->mx);
	printf("%d", rb->d[0]);
	for (int i = 1; i < rb->sz; i++) printf(" %d", rb->d[i]);
	printf("\n");
}

int rbutil_cmpfunc(const void * a, const void * b) {
    return (*(RB_DTYPE*)a - *(RB_DTYPE*)b);
}
