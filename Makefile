CC = gcc
LDFLAGS = -O2 -fopenmp -lnuma -lm

identify_numa: test.c identify_numa.c identify_numa.h
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) identify_numa
