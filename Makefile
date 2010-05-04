
CFLAGS = 
LDFLAGS = 
LIBS = 

all: test

test: test.o
	gcc $(LDFLAGS) -o $@ $< $(LIBS)

test.o: test.c
	gcc -Wall -c $(CFLAGS) -o $@ $<

clean:
	rm -f test test.o
