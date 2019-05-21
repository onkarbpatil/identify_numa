#include "identify_numa.h"

int main(int argc, char ** argv){
	struct labelled_numa_nodes * n_id = NULL;
	char *labels[] = {"DRAM","HBM"};
	n_id = identify_numa(2,labels);
	int i =0;
	for(i = 0; i < 2; i++){
		printf("Type: %s Numa ids: ", n_id[i].mem_type);
		int j = 0;
		for(j = 0; j < n_id[i].count; j++)
			printf("%d ",n_id[i].numa_id[j]);
		printf("\n");
	}
}
