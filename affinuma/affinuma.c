#include "affinuma.h"

struct numa_node_bw * numa_node_list = NULL;
struct numa_node_bw * numa_list_head = NULL;
int mem_types;
int max_node;
int numt;
int total_numa_nodes = 0;
int * numa_node_ids;
struct bitmask * numa_nodes;
char ** mem_tech;
long double * means;
int * cluster_sizes;
char classes[3][8] = {"fast", "slow", "slowest"};
char * cpu_range;

void label_mem(){
	struct numa_node_bw * bw_it = numa_list_head;
	struct numa_node_bw * next_bw_it = bw_it->next;
	int i = 0;
	bw_it->mem_type = classes[i];
	
	while(next_bw_it != NULL){
		long double diff = bw_it->owtr_avg - next_bw_it->owtr_avg;
		long double perct = 0.15*bw_it->owtr_avg;
		if((diff > perct)&&((i+1)<3)){
			i++;
		}
		next_bw_it->mem_type = classes[i];
		bw_it = next_bw_it;
		next_bw_it= bw_it->next;
	}
}

void sort_list(struct numa_node_bw * new_node){
	struct numa_node_bw * bw_it = numa_list_head;
	struct numa_node_bw * prev_bw_it = NULL;
	while(bw_it != NULL){
		if((bw_it->owtr_avg < new_node->owtr_avg)){
			if(prev_bw_it == NULL){
				new_node->next = bw_it;
				numa_list_head = new_node;
			}else{
				prev_bw_it->next = new_node;
				new_node->next = bw_it;
			}
			return;
		}
		prev_bw_it = bw_it;
		bw_it = bw_it->next;
	}
	prev_bw_it->next = new_node;
	return;

}

void write_config_file(){
	FILE * conf;
	char fname[50];
	strcpy(fname, "numa_class");
	conf = fopen(fname, "w");
	struct numa_node_bw * bw_it = numa_list_head;
	printf("CPU ID\tNUMA ID\tType\tInit(Mb/s)\tTriad(Mb/s)\n");
	while(bw_it != NULL){	
		fprintf(conf, "%d %s %Lf\n", bw_it->numa_id, bw_it->mem_type, bw_it->owtr_avg);
		printf("%s\t%d\t%s\t%LF\t%Lf\n", cpu_range, bw_it->numa_id, bw_it->mem_type, bw_it->wr_only_avg, bw_it->owtr_avg);
		bw_it = bw_it->next;
	}
	fclose(conf);
}

void numatest(int argc, char ** argv){
	cpu_range=argv[1];
	max_node = numa_max_node() + 1;
	int cpu_count = numa_num_possible_cpus();
	numa_node_ids = (int*)malloc(sizeof(int)*max_node);
	struct bitmask * numa_nodes = numa_get_membind();
	int i = 0;
        while(i < numa_nodes->size){
                if(numa_bitmask_isbitset(numa_nodes, i)){
                        numa_node_ids[total_numa_nodes] = i;
                        total_numa_nodes++;
                }
                i++;
        }
	int mbs = 1024;
	size_t size = mbs*1024*1024;
	int r_size = 16*32768;
	int c_size = 16*32768;
	double *a, *b, *c, *d, *e, *f, *g, *h;
	double **aa, **bb, **cc;
	clock_t start, end;
	struct timespec begin, stop;
	srand(clock());
	sleep(10);
 
 	i = 0;
	while(i < total_numa_nodes){
		int iters = 0;
		int stride;
		long double wr_only_avg=0.0;
		long double owtr_avg=0.0;
		long double accum;
		for( iters = 0; iters < 10; iters++){
			int j = 0;
			int k = 0;
			a = (double*)numa_alloc_onnode(size, numa_node_ids[i]);
			b = (double*)numa_alloc_onnode(size, numa_node_ids[i]);
			c = (double*)numa_alloc_onnode(size, numa_node_ids[i]);
			d = (double*)numa_alloc_onnode(size, numa_node_ids[i]);
			e = (double*)numa_alloc_onnode(size, numa_node_ids[i]);
			f = (double*)numa_alloc_onnode(size, numa_node_ids[i]);
			g = (double*)numa_alloc_onnode(size, numa_node_ids[i]);
			h = (double*)numa_alloc_onnode(size, numa_node_ids[i]);
			aa = (double**)numa_alloc_onnode(r_size, numa_node_ids[i]);
			for(j = 0; j < r_size/sizeof(double*); j++){
				aa[j] = (double*)numa_alloc_onnode(c_size, numa_node_ids[i]);
			}
			bb = (double**)numa_alloc_onnode(r_size, numa_node_ids[i]);
			for(j = 0; j < r_size/sizeof(double*); j++){
				bb[j] = (double*)numa_alloc_onnode(c_size, numa_node_ids[i]);
			}
			cc = (double**)numa_alloc_onnode(r_size, numa_node_ids[i]);
			for(j = 0; j < r_size/sizeof(double*); j++){
				cc[j] = (double*)numa_alloc_onnode(c_size, numa_node_ids[i]);
			}
			long double empty=0.0;
			long double empty2=0.0;

redo1:
			clock_gettime( CLOCK_MONOTONIC, &begin);
#pragma omp parallel for
			for(j = 0;j < (size/sizeof(double));j++){
				a[j] = 1.0;
				b[j] = 2.0;
				c[j] = 3.0;
				d[j] = 4.0;
				e[j] = 5.0;
				f[j] = 6.0;
				g[j] = 7.0;
				h[j] = 8.0;
			}
			clock_gettime( CLOCK_MONOTONIC, &stop);
			accum = ( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION;
			if(accum <= empty){
				goto redo1;
			}
			wr_only_avg += ((8*size*1.0E-06)/(long double)(accum - empty));
redo3:
			clock_gettime( CLOCK_MONOTONIC, &begin);
#pragma omp parallel for
			for(j =0; j < (size/sizeof(double)); j++){
                            a[j] = c[j] + b[j];
                        }
			clock_gettime( CLOCK_MONOTONIC, &stop);
			accum = ( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION;
			if(accum <= empty){
				goto redo3;
			}
			owtr_avg += ((3*size*1.0E-06)/(long double)(accum - empty));
			numa_free(a, size);
			numa_free(b, size);
			numa_free(c, size);
			numa_free(d, size);
			numa_free(e, size);
			numa_free(f, size);
			numa_free(g, size);
			numa_free(h, size);
			for(j = 0; j < r_size/sizeof(double*); j++){
				numa_free(aa[j], c_size);
			}
			for(j = 0; j < r_size/sizeof(double*); j++){
				numa_free(bb[j], c_size);
			}
			for(j = 0; j < r_size/sizeof(double*); j++){
				numa_free(cc[j], c_size);
			}
			numa_free(aa, r_size);
			numa_free(bb, r_size);
			numa_free(cc, r_size);
		}
		struct numa_node_bw * node_bw = (struct numa_node_bw *)malloc(sizeof(struct numa_node_bw));
		node_bw->numa_id = numa_node_ids[i];
		node_bw->wr_only_avg = wr_only_avg/10;
		node_bw->owtr_avg = owtr_avg/10;
		node_bw->next = NULL;
		if(numa_node_list == NULL){
			numa_node_list = node_bw;
			numa_list_head = numa_node_list;
		}
		else{
			sort_list(node_bw);
		}
		i++;
	}
	label_mem();
	write_config_file();
}
