/* Generate random numbers for tiny ring buffer, with periodic spikes/outliers.
 * Plot the raw and median filter output. */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#ifndef ARDUINO
#include <sys/ioctl.h> // ioctl()
#endif
#include <signal.h>    // raise() SIG*
#include <limits.h>
#include <string.h>

#include "../trb.h" // ringbuffer lib's main .h

void plot_data_blah(rb_st *rb, rb_st *rb_med);
void plot_data(rb_st *rb, rb_st *rb_med);
void plot_val(RB_DTYPE v, RB_DTYPE vm);
void get_rows_cols(uint16_t *r, uint16_t *c);

rb_st rb_real;
rb_st *rb=&rb_real;
rb_st rb_med_real; // Storage of median filter output
rb_st *rb_med=&rb_med_real;

#define PT_NUM     0x0001
#define PT_MEDBUFF 0x0002
#define PT_MEDINDI 0x0004
int opts=PT_MEDINDI | PT_NUM;

#define RB_SIZE    3 
#define RB_WINSIZE 3
#define VRANGE_MID 1000 // random number "input" middle and radius
#define VRANGE_RAD 100

#define GRAPH_WIDTH 80
#define AYEL "\x1b[33;1m"
#define ABGRE "\x1b[32;1m"
#define ABBLA "\x1b[30;1m"
#define ARST "\x1b[0m"
#define PNL printf("\n")
#define PCR printf("\r")
#define PFL fflush(stdout)
#define DEFAULT_ROWS 25
#define DEFAULT_COLS 75

uint16_t rows, cols;

void usage() {
	printf("Ringbuffer_plot_test [options]\n"
	 " -n for numerical output\n"
	 " -b for whole buffer chunks processed with a median filter\n"
	 " default is median filter run on each new value added\n");
}

int main(int argc, char *argv[]) {
	if (argc>1) {
		if (!strcmp(argv[1], "-n")) opts |= PT_NUM;
		else if (!strcmp(argv[1], "-b")) opts = (opts & ~PT_MEDINDI) | PT_MEDBUFF;
		else if (!strcmp(argv[1], "-h")) { usage(); exit(0); }
	}
	get_rows_cols(&rows, &cols);
	ringbuffer_init(rb, RB_WINSIZE);
	ringbuffer_init(rb_med, RB_WINSIZE);
	ringbuffer_setall(rb, 0);
	ringbuffer_setall(rb_med, 0);
	int window_size = 20;
	srand(time(0));
	rb->mn = UINT16_MAX;
	rb->mx = 0;

	while (1) {
		static int ctr=0;
		int range;
		range = VRANGE_RAD;
		if (rand() % 100 < 3) range = 400;
		int random_value = VRANGE_MID + (rand() % (range*2)) - range;
		ringbuffer_add(rb, random_value);
		ctr++;
		if (opts & PT_MEDINDI) {
			RB_DTYPE m = rb_median1(rb);
			plot_val(random_value, m); // median value only
			if (opts & PT_NUM)
				printf("\033[A%4d(%4d) \n", random_value, m);
		} else {
			if (ctr >= rb->sz) {
				ctr=0; // reset counter
				rb_minmax(rb);
				if (opts & PT_MEDBUFF || opts & PT_NUM)
					rb_median(rb, rb_med, window_size);
				printf(ABBLA "------------------------------------------------" ARST "\n");
				if (opts & PT_NUM) {
					printf("RB :");
					rb_print(rb);
					printf("RBM:");
					rb_print(rb_med);
					printf("\n");
				} else {
					plot_data(rb, rb_med);
				}
			}
		}
	}
	return 0;
}

void get_rows_cols(uint16_t *r, uint16_t *c) {
	#ifndef ARDUINO
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
	#else
		*r = DEFAULT_ROWS;
		*c = DEFAULT_COLS;
	#endif
}

void plot_val(RB_DTYPE v, RB_DTYPE vm) {
    static RB_DTYPE max = RB_DT_MIN;
    static RB_DTYPE min = RB_DT_MAX;
    if (max < v) max=v;
    if (min > v) min=v;
	get_rows_cols(&rows, &cols); // update
	/* if (cols > 95) raise(SIGINT); */
	if (rows<1) rows=39;
	if (cols<1) cols=85;
	cols--; // don't cause a wrap
	/* if (v == vm) raise(SIGINT); */
	float colperc = (float)(v-min) / (max - min);
	float colpercm = (float)(vm-min) / (max - min);
	/* printf("Cols: %d, Colperc: %f %f\n", cols, colperc, colpercm); */
	int col = (int)(cols * colperc);
	int colm = (int)(cols * colpercm);
	printf(ABGRE "\033[%dCM\r", colm-1);
	printf(AYEL "\033[%dCv\r", col-1);
	/* printf(" -->%d %d\n", col, colm); */
	usleep(20000);
	PNL;

}

void plot_data(rb_st *rb, rb_st *rb_med) {
	get_rows_cols(&rows, &cols); // update
	/* if (cols > 95) raise(SIGINT); */
	if (rows<1) rows=39;
	if (cols<1) cols=85;
	cols--; // don't cause a wrap
	/* printf("rows=%d\n", rows); printf("cols=%d\n", cols); exit(0); */
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
        printf(ABGRE "\033[%dCM\r", colm);
        printf(AYEL "\033[%dCv\r", col);
        /* printf(" -->%d %d\n", col, colm); */
        usleep(20000);
        PNL;
    }
}
