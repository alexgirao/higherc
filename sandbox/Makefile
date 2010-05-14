PROGS = sort-insertion sort-bubble

all: $(PROGS)

$(PROGS): %: %.c
	gcc -Wall -o $@ $<

clean:
	rm -f $(PROGS)
