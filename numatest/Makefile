CC = gcc
LDFLAGS = -O2 -fopenmp -lnuma -lm

numatest: test2.c numatest_omp.c numatest_omp.h
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) numatest
