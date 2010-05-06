
CFLAGS = -Wall
LDFLAGS = -L.
LIBS = -lhigherc
PROGS = test test-list-0 test-stralloc-0
OBJS = byte.o alloc.o str.o list.o stralloc.o test-list-0.o test-stralloc-0.o

all: higherc.a $(PROGS)

higherc.a: list.o byte.o alloc.o str.o stralloc.o
	ar cr $@ $^
	ranlib $@

test: test.o
	gcc $(LDFLAGS) -o $@ $< $(LIBS)

test-list-0 test-stralloc-0: %: %.o
	gcc $(LDFLAGS) -o $@ $< $(LIBS)

test.o: test.c
	gcc -c $(CFLAGS) -o $@ $<

$(OBJS): %.o: %.c
	gcc -c $(CFLAGS) -o $@ $<

clean:
	rm -f  *.o $(PROGS)

byte.o: higherc/higherc.h higherc/byte.h
str.o: higherc/higherc.h higherc/byte.h higherc/str.h
alloc.o: higherc/higherc.h higherc/byte.h higherc/alloc.h
stralloc.o: higherc/higherc.h higherc/byte.h higherc/str.h higherc/stralloc.h
list.o: higherc/higherc.h higherc/alloc.h higherc/list.h
$(PROGS): higherc.a
