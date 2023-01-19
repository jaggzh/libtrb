LIBNAME=libtrb.so.0.1
INCNAME=trb-0.1.h
LIBDEST="/usr/lib"
INCDEST="/usr/include"
LIBSYM=libtrb.so
DEBUG=
DEBUG=-O0 -ggdb3

all: test

tests: ringbuffer_test_run ringbuffer_plot_run

test: tests

tests/ringbuffer_test: tests/ringbuffer_test.c ringbuffer.c trb.h
	gcc -I. $(DEBUG) -Wall -o tests/ringbuffer_test tests/ringbuffer_test.c ringbuffer.c

ringbuffer_test_run: tests/ringbuffer_test
	tests/ringbuffer_test


tests/ringbuffer_plot: tests/ringbuffer_plot.c ringbuffer.c trb.h
	gcc -I. $(DEBUG) -Wall -o "$@" tests/ringbuffer_plot.c ringbuffer.c

ringbuffer_plot_run: tests/ringbuffer_plot
	tests/ringbuffer_plot


ringbuffer.o: ringbuffer.c
	gcc -c $(DEBUG) -Wall -Werror -fpic "$@"

libtrb.so: ringbuffer.o
	gcc -shared -o "$@" $^
	echo "Created libtrb.so"

lib: libtrb.so

install:
	cp libtrb.so "$(LIBDEST)/$(LIBNAME)"
	ln -s "$(LIBDEST)/$(LIBNAME)" libtrb.so
	cp trb.h "$(INCDEST)/$(INCNAME)

vi:
	vim Makefile \
		tests/ringbuffer_plot.c \
		tests/ringbuffer_test.c \
		ringbuffer.c \
		trb.h
