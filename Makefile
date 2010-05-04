
CFLAGS = 
LDFLAGS = 
LIBS = higherc.a

all: higherc.a test test2

higherc.a: list.o
	ar cr $@ $^
	ranlib $@

test: test.o
	gcc $(LDFLAGS) -o $@ $< $(LIBS)

test2: test2.o
	gcc $(LDFLAGS) -o $@ $< $(LIBS)

test.o: test.c
	gcc -Wall -c $(CFLAGS) -o $@ $<

test2.o: test2.c
	gcc -Wall -c $(CFLAGS) -o $@ $<

list.o: list.c
	gcc -Wall -c $(CFLAGS) -o $@ $<

clean:
	rm -f test test.o test2 test2.o

list.o: higherc/higherc.h higherc/list.h
