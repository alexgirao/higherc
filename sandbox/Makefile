PROGS = sort-insertion sort-bubble sort-merge

all: $(PROGS)

$(PROGS): %: %.c
	gcc -Wall -o $@ $< -L.. -lhigherc

clean:
	rm -f $(PROGS)
