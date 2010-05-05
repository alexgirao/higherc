
CFLAGS = 
LDFLAGS = 
LIBS = higherc.a

all: higherc.a test test-list-0

higherc.a: list.o str.o
	ar cr $@ $^
	ranlib $@

test: test.o
	gcc $(LDFLAGS) -o $@ $< $(LIBS)

test-list-0: test-list-0.o
	gcc $(LDFLAGS) -o $@ $< $(LIBS)

test.o: test.c
	gcc -Wall -c $(CFLAGS) -o $@ $<

test-list-0.o: test-list-0.c
	gcc -Wall -c $(CFLAGS) -o $@ $<

list.o str.o: %.o: %.c
	gcc -Wall -c $(CFLAGS) -o $@ $<

clean:
	rm -f test test.o test-list-0 test-list-0.o

list.o: higherc/higherc.h higherc/list.h
str.o: higherc/higherc.h higherc/str.h
test test-list-0: higherc.a
