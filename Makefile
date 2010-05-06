
CFLAGS = -Wall
LDFLAGS = -L.
LIBS = -lhigherc
PROGS = test test-list-0 test-stralloc-0 test-pbuf-0
OBJS = byte.o alloc.o str.o list.o stralloc.o pbuf.o test-list-0.o	\
test.o test-stralloc-0.o test-pbuf-0.o

all: libhigherc.a $(PROGS)

libhigherc.a: list.o byte.o alloc.o str.o stralloc.o pbuf.o
	ar cr $@ $^
	ranlib $@

test: test.o
	gcc $(LDFLAGS) -o $@ $< $(LIBS)

test-list-0 test-stralloc-0 test-pbuf-0: %: %.o
	gcc $(LDFLAGS) -o $@ $< $(LIBS)

$(OBJS): %.o: %.c
	gcc -c $(CFLAGS) -o $@ $<

clean:
	rm -f  *.o $(PROGS) libhigherc.a

byte.o: higherc/higherc.h higherc/byte.h
str.o: higherc/higherc.h higherc/byte.h higherc/str.h
alloc.o: higherc/higherc.h higherc/byte.h higherc/alloc.h
stralloc.o: higherc/higherc.h higherc/byte.h higherc/str.h higherc/stralloc.h
list.o: higherc/higherc.h higherc/alloc.h higherc/list.h
pbuf.o: higherc/higherc.h higherc/alloc.h higherc/list.h higherc/pbuf.h
$(PROGS): libhigherc.a
