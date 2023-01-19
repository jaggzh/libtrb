#include <stdio.h>
#include "trb.h"

#define RB_BUF_SIZE 10

void print_rb(rb_st *rb) {
	for (int i=0; i<rb->sz; i++) {
		printf("[%d] %d\n", i, rb->d[i]);
	}
}

int main(int argc, char *argv[]) {
	rb_st rbs;
	rb_st *rb = &rbs; // convenience
	ringbuffer_init(rb, RB_BUF_SIZE);
	ringbuffer_setall(rb, 0);
	for (int i=1; i<20; i++) {
		ringbuffer_add(rb, i);
		print_rb(rb);
	}
}
