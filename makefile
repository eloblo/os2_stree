run: all

all: stree.c
	gcc -g -o stree stree.c
	
.PHONY: clean

clean:
	rm -f stree