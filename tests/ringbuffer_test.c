#include <stdio.h>
#include "trb.h"

#define RB_BUF_SIZE 5

#define NL puts("");
#define PF(...) printf(__VA_ARGS__)

void print_rb(rb_st *rb) {
	for (int i=0; i<rb->sz; i++) {
		printf(" [%d] %d\n", i, rb->d[i]);
	}
}

int main(int argc, char *argv[]) {
	rb_st rbs;
	rb_st *rb = &rbs; // convenience
	ringbuffer_init(rb, RB_BUF_SIZE);
	ringbuffer_setall(rb, 0);
	printf("Fill test:\n");
	for (int i=1; i<RB_BUF_SIZE+5; i++) {
		PF(" i=%d (1 .. %d)\n", i, RB_BUF_SIZE+5);
		ringbuffer_add(rb, i);
		print_rb(rb);
	}
	rb_free(rb);

	int lastval=7;
	NL;
	PF("rb_get_last() (First retrieved should be '%d')\n", lastval);
	rb_init(rb, 5);
	for (int i=0; i<lastval+1; i++) { rb_add(rb, i); }
	print_rb(rb);
	for (int i=0; i<5; i++) {
		RB_DTYPE v;
		v = rb_get_last(rb, i);
		printf("%d ", v);
	}
	rb_free(rb);
	NL;

	NL;
	PF("Testing all-buffer median ()\n");
	lastval=10;
	rb_init(rb, 6);
	for (int i=0; i<lastval+1; i++) { rb_add(rb, i); }
	print_rb(rb);
	printf(" Median: %d\n", rb_median1(rb));
}
