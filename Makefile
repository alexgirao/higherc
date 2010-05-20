
CFLAGS = -Wall -g
LDFLAGS = -L.
LIBS = -lhigherc

LIB_OBJS = byte.o alloc.o str.o list.o s.o pbuf.o bytewise.o readfd.o	\
fatal.o rand.o n.o

LIB_HEADERS = higherc/alloc.h higherc/byte.h higherc/higherc.h		\
higherc/list.h higherc/pbuf.h higherc/s.h higherc/str.h higherc/tok.h	\
higherc/bytewise.h higherc/readfd.h higherc/fatal.h higherc/rand.h	\
higherc/n.h

PROGS = test-0 test-1 test-2 test-3 test-list-0 test-stralloc-0		\
test-pbuf-0 test-tok-0 sample-cat test-bytewise test-rand test-n-0	\
test-n-1

PROGS_OBJS = test-0.o test-1.o test-2.o test-3.o test-list-0.o	\
test-stralloc-0.o test-pbuf-0.o test-tok-0.o test-bytewise.o	\
sample-cat.o test-rand.o test-n-0.o test-n-1.o

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
s.o: higherc/higherc.h higherc/byte.h higherc/str.h higherc/s.h
list.o: higherc/higherc.h higherc/byte.h higherc/str.h higherc/alloc.h higherc/list.h
pbuf.o: higherc/higherc.h higherc/byte.h higherc/str.h higherc/alloc.h higherc/list.h higherc/pbuf.h
bytewise.o: higherc/higherc.h higherc/bytewise.h
readfd.o: higherc/higherc.h higherc/byte.h higherc/readfd.h
n.o: higherc/higherc.h higherc/byte.h higherc/n.h

$(PROGS): $(LIB_OBJS)
$(PROGS_OBJS): $(LIB_HEADERS)
