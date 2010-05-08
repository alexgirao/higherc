
CFLAGS = -Wall
LDFLAGS = -L.
LIBS = -lhigherc

LIB_OBJS = byte.o alloc.o str.o list.o stralloc.o pbuf.o

LIB_HEADERS = higherc/alloc.h higherc/buffer.h higherc/byte.h		\
higherc/higherc.h higherc/list.h higherc/pbuf.h higherc/stralloc.h	\
higherc/str.h higherc/tok.h

PROGS = test test-list-0 test-stralloc-0 test-pbuf-0 test-tok-0	\
sample-cat

PROGS_OBJS = test.o test-list-0.o test-stralloc-0.o test-pbuf-0.o	\
test-tok-0.o sample-cat.o

all: libhigherc.a $(PROGS)

libhigherc.a: $(LIB_OBJS)
	ar cr $@ $^
	ranlib $@

$(LIB_OBJS) $(PROGS_OBJS): %.o: %.c
	gcc -c $(CFLAGS) -o $@ $<

$(PROGS): %: %.o
	gcc $(LDFLAGS) -o $@ $< $(LIBS)

clean:
	rm -f  *.o $(PROGS) libhigherc.a

byte.o: higherc/higherc.h higherc/byte.h
str.o: higherc/higherc.h higherc/byte.h higherc/str.h
alloc.o: higherc/higherc.h higherc/byte.h higherc/alloc.h
stralloc.o: higherc/higherc.h higherc/byte.h higherc/str.h higherc/stralloc.h
list.o: higherc/higherc.h higherc/byte.h higherc/str.h higherc/alloc.h higherc/list.h
pbuf.o: higherc/higherc.h higherc/byte.h higherc/str.h higherc/alloc.h higherc/list.h higherc/pbuf.h

$(PROGS_OBJS): $(LIB_HEADERS)
