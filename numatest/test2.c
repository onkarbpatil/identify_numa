#include "numatest_omp.h"

int main(int argc, char ** argv){
	char *labels[] = {"DRAM","HBM"};
	numatest(2,labels);
}
