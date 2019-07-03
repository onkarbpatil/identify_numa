#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <sched.h>
#include <sys/time.h>
#include "numa.h"

#define BILLION  1000000000L;

int main(){
	int max_node;
	int total_numa_nodes = 0;
	int * numa_node_ids;
	max_node = numa_max_node() + 1;
	int cpu_count = numa_num_possible_cpus();
	numa_node_ids = (int*)malloc(sizeof(int)*max_node);
	struct bitmask * numa_nodes = numa_get_membind();
	int n = 0;
	while(n < numa_nodes->size){
		if(numa_bitmask_isbitset(numa_nodes, n)){
			numa_node_ids[total_numa_nodes] = n;
			total_numa_nodes++;
		}
		n++;
	}
	n = 0;
	while(n < total_numa_nodes){
		printf("Numa id: %d\n", n);
        int * arr1;
        int * arr2;
//        int * arr3;
        long long int size1 = ((long long int)(4*1024)*(1024*1024));
        long long int size2 = (long long int)(512*1024*1024);
	arr1 = (int *)numa_alloc_onnode(size1, numa_node_ids[n]);
	arr2 = (int *)numa_alloc_onnode(size2, numa_node_ids[n]);
//        arr3 = (int *)malloc(512*1024*1024);
        int i = 1;
        int j = 0;
	int k = 0;
	volatile int dp;
        long double accum;
        struct timespec begin, stop;
        for(j=0;j<((((long long int)(4*1024)*(1024*1024))/sizeof(int)));j++){
                arr1[j] = j*3;
	//	if(j <(512*1024*1024))
               		//arr2[j] = j*7;
               // arr3[j] = j*5;
        }
printf("Read\n");
        while(i < (((40*1024*1024)/sizeof(int))+1)){
                j = 0;
                        clock_gettime( CLOCK_MONOTONIC, &begin);
                while(j < ((((long long int)(4*1024)*(1024*1024))/sizeof(int)))){
                        dp = arr1[j];
                        j+=i;
                }
                        clock_gettime( CLOCK_MONOTONIC, &stop);
                accum = ( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION;
                //printf("Access stride: %d\n No. of accessess: %d\n access latency: %.10Lf\n access bandwidth %LF\n", (i*sizeof(int)), ((j/i)-1), (long double)(accum/((j/i)-1)), (((j/i)-1)*sizeof(int)/(accum*1024*1024)));
                printf("%d %.10Lf %LF\n", (i*sizeof(int)), (long double)(accum/((j/i)-1)), (((j/i)-1)*sizeof(int)/(accum*1024*1024)));
                i+=8192;
		for(k=0;k<(512*1024*1024)/sizeof(int);k++)
			arr2[k]+=arr2[k]-k;
        }
printf("Write\n");
	i=1;
        while(i < (((36*1024*1024)/sizeof(int))+1)){
                j = 0;
                        clock_gettime( CLOCK_MONOTONIC, &begin);
                while(j < ((((long long int)(4*1024)*(1024*1024))/sizeof(int)))){
                        arr1[j] = dp;
                        j+=i;
                }
                        clock_gettime( CLOCK_MONOTONIC, &stop);
                accum = ( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION;
                //printf("Access stride: %d\n No. of accessess: %d\n access latency: %.10Lf\n access bandwidth %LF\n", (i*sizeof(int)), ((j/i)-1), (long double)(accum/((j/i)-1)), (((j/i)-1)*sizeof(int)/(accum*1024*1024)));
                printf("%d %.10Lf %LF\n", (i*sizeof(int)), (long double)(accum/((j/i)-1)), (((j/i)-1)*sizeof(int)/(accum*1024*1024)));
                i+=8192;
		//i*=2;
		for(k=0;k<(512*1024*1024)/sizeof(int);k++)
			arr2[k]+=arr2[k]-k;
		dp = arr2[k/4];
        }
        numa_free(arr1, size1);
        numa_free(arr2, size2);
        //free(arr3);
	n++;
	}
        return 1;
}
