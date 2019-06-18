#include "stream_numa_char.h"

int main(int argc, char ** argv){
	char *labels[] = {"NVME","DRAM"};
	if(strcmp(argv[1], "OMP") == 0)
	stream_numa_char_omp(2,labels);
	else if(strcmp(argv[1], "MPI") == 0)
	stream_numa_char_mpi(2,labels);
}
