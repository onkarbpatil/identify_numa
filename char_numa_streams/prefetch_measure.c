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
        int * arr1;
        int * arr2;
//        int * arr3;
        arr1 = (int *)malloc((long long int)(4*1024)*(1024*1024));
        arr2 = (int *)malloc(512*1024*1024);
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
        while(i < (((36*1024*1024)/sizeof(int))+1)){
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
        free(arr1);
        free(arr2);
        //free(arr3);
        return 1;
}
