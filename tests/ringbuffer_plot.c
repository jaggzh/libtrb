/* Generate random numbers for tiny ring buffer, with periodic spikes/outliers.
 * Plot the raw and median filter output. */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/ioctl.h> // ioctl()
#include <signal.h>    // raise() SIG*
#include <limits.h>
#include <string.h>

#include "trb.h" // ringbuffer lib's main .h

void plot_data_blah(rb_st *rb, rb_st *rb_med);
void plot_data(rb_st *rb, rb_st *rb_med);

rb_st rb_real;
rb_st *rb=&rb_real;
rb_st rb_med_real; // Storage of median filter output
rb_st *rb_med=&rb_med_real;

int opt_plot_vals=0;
int opt_plot=1;

int main(int argc, char *argv[]) {
	int rbsize=30;
	if (argc>1 && !strcmp(argv[1], "-n")) opt_plot_vals=1, opt_plot=0;
	ringbuffer_init(rb, rbsize);
	ringbuffer_init(rb_med, rbsize);
	ringbuffer_setall(rb, 0);
	ringbuffer_setall(rb_med, 0);
	int window_size = 20;
	srand(time(0));
	rb->mn = UINT16_MAX;
	rb->mx = 0;
	while (1) {
		#define VRANGE_MID 1000
		#define VRANGE_RAD 100
		int range;
		char plot;
		range = VRANGE_RAD;
		if (rand() % 100 < 3) range = 400;
		int random_value = VRANGE_MID + (rand() % (range*2)) - range;
		if (rb->hd == rb->sz - 1) plot=1;
		else plot=0;
		ringbuffer_add(rb, random_value);
		if (plot) {
			ringbuffer_minmax(rb);
			ringbuffer_median_filter2(rb, rb_med, window_size);
			if (opt_plot_vals) {
				printf("RB :");
				ringbuffer_print(rb);
				printf("RBM:");
				ringbuffer_print(rb_med);
				printf("\n");
			}
			if (opt_plot) {
				plot_data(rb, rb_med);
			}
		}
	}
	return 0;
}

#define GRAPH_WIDTH 80
#define AYEL "\x1b[33;1m"
#define ABGRE "\x1b[32;1m"
#define ARST "\x1b[0m"
#define PNL printf("\n")
#define PCR printf("\r")
#define PFL fflush(stdout)
#define DEFAULT_ROWS 25
#define DEFAULT_COLS 75


void get_rows_cols(uint16_t *r, uint16_t *c) {
	struct winsize w;
	if (isatty(fileno(stdout))) {
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
		/* if (w.ws_col > 95) raise(SIGINT); */
		/* printf("rc: %d %d\n", w.ws_row, w.ws_col); */
		/* static int i=0; */
		/* if (i++ > 4000) exit(0); */
		/* 24784 6260 */
		/* 	min=1002, max=2098 */
		*r = w.ws_row;
		*c = w.ws_col;
	} else {
		*r = DEFAULT_ROWS;
		*c = DEFAULT_COLS;
	}
}

void plot_data(rb_st *rb, rb_st *rb_med) {
	uint16_t rows, cols;
	get_rows_cols(&rows, &cols);
	/* if (cols > 95) raise(SIGINT); */
	if (rows<1) rows=39;
	if (cols<1) cols=85;
	cols--; // don't cause a wrap
	/* printf("rows=%d\n", rows); */
	/* printf("cols=%d\n", cols); */
	/* exit(0); */
    RB_DTYPE max_value = rb->mx;
    RB_DTYPE min_value = rb->mn;

    for (RB_IDXTYPE y = 0; y < rb->sz; y++) {
    	RB_DTYPE v, vm;
    	v = rb->d[y];
    	vm = rb_med->d[y];
    	float colperc = (float)(v-min_value) / (max_value - min_value);
    	float colpercm = (float)(vm-min_value) / (max_value - min_value);
        /* printf("Cols: %d, Colperc: %f %f\n", cols, colperc, colpercm); */
    	int col = (int)(cols * colperc);
    	int colm = (int)(cols * colpercm);
        printf(ABGRE "\033[%dC+\r", colm);
        printf(AYEL "\033[%dC*\r", col);
        /* printf(" -->%d %d\n", col, colm); */
        usleep(20000);
        PNL;
    }
}
