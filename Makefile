all: aslr

CFLAGS = -Wall -O0 -L. -m32 -ggdb3 -Ilib/xed2-ia32/include -I.
CC = gcc

aslr: aslr.o bounds.o tree.o sample.o needed.o stat.o
	$(CC) $(CFLAGS) -static -o $@ $^ lib/xed2-ia32/lib/libxed.a -lelf

clean:
	rm -f *.o *.core core aslr
