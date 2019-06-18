#include "stream_numa_char.h"

int main(int argc, char ** argv){
	char *labels[] = {"NVME","DRAM"};
	//stream_numa_char_omp(2,labels);
	stream_numa_char_mpi(2,labels);
}
