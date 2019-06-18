CC = mpicc
LDFLAGS = -O2 -fopenmp -lnuma -lm

stream_numa_char: test2.c stream_numa_char.c stream_numa_char.h
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) stream_numa_char
