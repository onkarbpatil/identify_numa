#include "numatest_omp.h"

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

void calculate_distances(){
	int i;
        struct numa_node_bw * bw_it = numa_list_head;
	while(bw_it != NULL){
		i = 0;
		long double delta = 9999999.9999;
		while(i < mem_types){
			long double dist = abs(sqrt(abs((means[i] - bw_it->owtr_avg))*abs((means[i] - bw_it->owtr_avg))));
			if(dist < delta){
				delta = dist;
				if(strcmp(bw_it->mem_type, mem_tech[i])!=0){
					if(((i-1)>=0)&&(strcmp(bw_it->mem_type, mem_tech[i-1])==0)){
						cluster_sizes[i-1]--;
						bw_it->mem_type = mem_tech[i];
						cluster_sizes[i]++;
					}
					else if(((i+1)<mem_types)&&(strcmp(bw_it->mem_type, mem_tech[i+1])==0)){
                                                cluster_sizes[i+1]--;
						bw_it->mem_type = mem_tech[i];
						cluster_sizes[i]++;
                    			}
				}
			}
			i++;
		}
		bw_it = bw_it->next;
	}
}

void calculate_mean(){
	int i = 0;
	struct numa_node_bw * bw_it = numa_list_head;
	while(i < mem_types){
		int j = 0;
		means[i] = 0.0;
		while(j < cluster_sizes[i]){
			means[i] += bw_it->owtr_avg;
			j++;
			bw_it = bw_it->next;
		}
		means[i] /= cluster_sizes[i];
		i++;
	}
	calculate_distances();
}

void classify(){
	int cluster_size;
	int last_cluster_size;
	cluster_size = total_numa_nodes/mem_types;
	last_cluster_size = cluster_size + (total_numa_nodes%mem_types);
	cluster_sizes = (int *)malloc(sizeof(int)*mem_types);
	means = (long double *)(malloc(mem_types*sizeof(long double)));
	struct numa_node_bw * bw_it = numa_list_head;
	int i = 0;
	while(i < mem_types){
		if(i == (mem_types - 1)){
			cluster_sizes[i] = last_cluster_size;
		}
		else{
			cluster_sizes[i] = cluster_size;
		}
		i++;
	}
	i = 0;
	int j = 1;
	while(bw_it != NULL){
		bw_it->mem_type = mem_tech[i];
		if(j < cluster_sizes[i]){
			j++;
		}
		else{
			j = 1;
			i++;
		}
		bw_it = bw_it->next;
	}
	bw_it = numa_list_head;
	i = 0;
	while(i < 10){
		calculate_mean();
		i++;
	}

}

void sort_list(struct numa_node_bw * new_node){
	struct numa_node_bw * bw_it = numa_list_head;
	struct numa_node_bw * prev_bw_it = NULL;
	while(bw_it != NULL){
		if((bw_it->owtr_avg > new_node->owtr_avg)){
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
	char thr[10];
	snprintf(thr, 10, "%d", numt);
	strcpy(fname, "sicm_numa_config");
	strcat(fname, thr);
	conf = fopen(fname, "w");
	struct numa_node_bw * bw_it = numa_list_head;
	while(bw_it != NULL){	
		fprintf(conf, "%d %s %Lf %LF %LF %LF %LF %LF %LF %LF %LF %LF %LF %LF %LF %LF %LF %LF %LF %LF %LF %LF %LF %LF %LF %LF %LF %LF %LF\n", bw_it->numa_id, bw_it->mem_type, bw_it->wr_only_avg, bw_it->owor_avg, bw_it->owtr_avg, bw_it->owthr_avg, bw_it->owfr_avg, bw_it->twor_avg, bw_it->twtr_avg, bw_it->twthr_avg, bw_it->twfr_avg, bw_it->thwor_avg, bw_it->thwtr_avg, bw_it->thwthr_avg, bw_it->thwfr_avg, bw_it->fwor_avg, bw_it->fwtr_avg, bw_it->fwthr_avg, bw_it->fwfr_avg, bw_it->str_avg, bw_it->rand_avg, bw_it->diff_avg, bw_it->row_avg, bw_it->col_avg, bw_it->rc_avg, bw_it->t_sten_avg, bw_it->f_sten_avg, bw_it->n_sten_avg, bw_it->l2cache_avg);
		printf("%d %s %Lf %LF %LF %LF %LF %LF %LF %LF %LF %LF %LF %LF %LF %LF %LF %LF %LF %LF %LF %LF %LF %LF %LF %LF %LF %LF %LF\n", bw_it->numa_id, bw_it->mem_type, bw_it->wr_only_avg, bw_it->owor_avg, bw_it->owtr_avg, bw_it->owthr_avg, bw_it->owfr_avg, bw_it->twor_avg, bw_it->twtr_avg, bw_it->twthr_avg, bw_it->twfr_avg, bw_it->thwor_avg, bw_it->thwtr_avg, bw_it->thwthr_avg, bw_it->thwfr_avg, bw_it->fwor_avg, bw_it->fwtr_avg, bw_it->fwthr_avg, bw_it->fwfr_avg, bw_it->str_avg, bw_it->rand_avg, bw_it->diff_avg, bw_it->row_avg, bw_it->col_avg, bw_it->rc_avg, bw_it->t_sten_avg, bw_it->f_sten_avg, bw_it->n_sten_avg, bw_it->l2cache_avg);
		bw_it = bw_it->next;
	}
	fclose(conf);
}

void numatest(int argc, char ** argv){
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
	int r_size = 32768;
	int c_size = 32768;
	double *a, *b, *c, *d, *e, *f, *g, *h;
	double **aa, **bb, **cc;
	clock_t start, end;
	struct timespec begin, stop;
	srand(clock());
	sleep(10);
	if(argc == 0){
		printf("Enter memory technologies available in ascending order of speed. eg: GPU NVRAM DRAM HBM\n");
		return;
	}
	else{
		mem_types = argc;
		mem_tech = (char**)malloc(argc*sizeof(char*));
		int a;
		for(a = 0; a < argc; a++){
			mem_tech[a] = argv[a];
		}

	}
#ifdef _OPENMP
#pragma omp parallel private(numt)
    {
    numt = omp_get_num_threads();
    }
#endif
  	i = 0;
	while(i < total_numa_nodes){
		int iters = 0;
		int stride;
		long double wr_only_avg=0.0;
		long double owor_avg=0.0;
		long double owtr_avg=0.0;
        	long double owthr_avg=0.0;
		long double owfr_avg=0.0;
		long double twor_avg=0.0;
		long double twtr_avg=0.0;
		long double twthr_avg=0.0;
		long double twfr_avg=0.0;
		long double thwor_avg=0.0;
		long double thwtr_avg=0.0;
		long double thwthr_avg=0.0;
		long double thwfr_avg=0.0;
		long double fwor_avg=0.0;
		long double fwtr_avg=0.0;
		long double fwthr_avg=0.0;
		long double fwfr_avg=0.0;
		long double str_avg=0.0;
		long double rand_avg = 0.0;
		long double diff_avg = 0.0;
		long double row_avg = 0.0;
		long double col_avg = 0.0;
		long double rc_avg = 0.0;
		long double l2cache_avg = 0.0;
		long double t_sten_avg = 0.0;
		long double f_sten_avg = 0.0;
		long double n_sten_avg = 0.0;
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
			clock_gettime( CLOCK_REALTIME, &begin);
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
			clock_gettime( CLOCK_REALTIME, &stop);
			accum = ( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION;
			if(accum <= empty){
				goto redo1;
			}
			wr_only_avg += ((8*size*1.0E-06)/(long double)(accum - empty));
redo2:
			clock_gettime( CLOCK_REALTIME, &begin);
#pragma omp parallel for
			for(j =0; j < (size/sizeof(double)); j++){
                            a[j] = 20 + b[j];
                        }
			clock_gettime( CLOCK_REALTIME, &stop);
			accum = ( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION;
			if(accum <= empty){
				goto redo2;
			}
			owor_avg += ((2*size*1.0E-06)/(long double)(accum - empty));
redo3:
			clock_gettime( CLOCK_REALTIME, &begin);
#pragma omp parallel for
			for(j =0; j < (size/sizeof(double)); j++){
                            a[j] = c[j] + b[j];
                        }
			clock_gettime( CLOCK_REALTIME, &stop);
			accum = ( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION;
			if(accum <= empty){
				goto redo3;
			}
			owtr_avg += ((3*size*1.0E-06)/(long double)(accum - empty));
redo4:
			clock_gettime( CLOCK_REALTIME, &begin);
#pragma omp parallel for
			for(j =0; j < (size/sizeof(double)); j++){
                            a[j] = c[j] + d[j] + b[j];
                        }
			clock_gettime( CLOCK_REALTIME, &stop);
			accum = ( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION;
			if(accum <= empty){
				goto redo4;
			}
			owthr_avg += ((4*size*1.0E-06)/(long double)(accum - empty));
redo5:
			clock_gettime( CLOCK_REALTIME, &begin);
#pragma omp parallel for
			for(j =0; j < (size/sizeof(double)); j++){
                            a[j] = c[j] + d[j] + e[j] + b[j];
                        }
			clock_gettime( CLOCK_REALTIME, &stop);
			accum = ( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION;
			if(accum <= empty){
				goto redo5;
			}
			owfr_avg += ((5*size*1.0E-06)/(long double)(accum - empty));
redo6:
			clock_gettime( CLOCK_REALTIME, &begin);
#pragma omp parallel for
                        for(j =0; j < (size/sizeof(double)); j++){
                            a[j] = c[j] = b[j];
                        }
			clock_gettime( CLOCK_REALTIME, &stop);
			accum = ( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION;
			if(accum <= empty){
				goto redo6;
			}
			twor_avg += ((3*size*1.0E-06)/(long double)(accum - empty));
redo7:
			clock_gettime( CLOCK_REALTIME, &begin);
#pragma omp parallel for
			for(j =0; j < (size/sizeof(double)); j++){
                            a[j] = c[j] = d[j] + b[j];
                        }
			clock_gettime( CLOCK_REALTIME, &stop);
			accum = ( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION;
			if(accum <= empty){
				goto redo7;
			}
			twtr_avg += ((4*size*1.0E-06)/(long double)(accum - empty));
redo8:
			clock_gettime( CLOCK_REALTIME, &begin);
#pragma omp parallel for
			for(j =0; j < (size/sizeof(double)); j++){
                            a[j] = c[j] = d[j] + b[j]+ e[j];
                        }
			clock_gettime( CLOCK_REALTIME, &stop);
			accum = ( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION;
			if(accum <= empty){
				goto redo8;
			}
			twthr_avg += ((5*size*1.0E-06)/(long double)(accum - empty));
redo9:
			clock_gettime( CLOCK_REALTIME, &begin);
#pragma omp parallel for
			for(j =0; j < (size/sizeof(double)); j++){
                            a[j] = c[j] = d[j] + b[j]+ e[j] + f[j];
                        }
			clock_gettime( CLOCK_REALTIME, &stop);
			accum = ( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION;
			if(accum <= empty){
				goto redo9;
			}
			twfr_avg += ((6*size*1.0E-06)/(long double)(accum - empty));
redo10:
			clock_gettime( CLOCK_REALTIME, &begin);
#pragma omp parallel for
                        for(j =0; j < (size/sizeof(double)); j++){
                            a[j] = c[j] = d[j] = e[j];
                        }
			clock_gettime( CLOCK_REALTIME, &stop);
			accum = ( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION;
			if(accum <= empty){
				goto redo10;
			}
			thwor_avg += ((4*size*1.0E-06)/(long double)(accum - empty));
redo11:
			clock_gettime( CLOCK_REALTIME, &begin);
#pragma omp parallel for
			for(j =0; j < (size/sizeof(double)); j++){
                            a[j] = c[j] = d[j] = b[j]+ e[j];
                        }
			clock_gettime( CLOCK_REALTIME, &stop);
			accum = ( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION;
			if(accum <= empty){
				goto redo11;
			}
			thwtr_avg += ((5*size*1.0E-06)/(long double)(accum - empty));
redo12:
			clock_gettime( CLOCK_REALTIME, &begin);
#pragma omp parallel for
			for(j =0; j < (size/sizeof(double)); j++){
                            a[j] = c[j] = d[j] = b[j] + e[j] +f[j];
                        }
			clock_gettime( CLOCK_REALTIME, &stop);
			accum = ( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION;
			if(accum <= empty){
				goto redo12;
			}
			thwthr_avg += ((6*size*1.0E-06)/(long double)(accum - empty));
redo13:
			clock_gettime( CLOCK_REALTIME, &begin);
#pragma omp parallel for
			for(j =0; j < (size/sizeof(double)); j++){
                            a[j] = c[j] = d[j] = b[j] + e[j] + f[j] +g[j];
                        }
			clock_gettime( CLOCK_REALTIME, &stop);
			accum = ( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION;
			if(accum <= empty){
				goto redo13;
			}
			thwfr_avg += ((7*size*1.0E-06)/(long double)(accum - empty));
redo14:
			clock_gettime( CLOCK_REALTIME, &begin);
#pragma omp parallel for
                        for(j =0; j < (size/sizeof(double)); j++){
                            a[j] = c[j] = d[j] = b[j] = e[j];
                        }
			clock_gettime( CLOCK_REALTIME, &stop);
			accum = ( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION;
			if(accum <= empty){
				goto redo14;
			}
			fwor_avg += ((5*size*1.0E-06)/(long double)(accum - empty));
redo15:
			clock_gettime( CLOCK_REALTIME, &begin);
#pragma omp parallel for
                        for(j =0; j < (size/sizeof(double)); j++){
                            a[j] = c[j] = d[j] = b[j] = e[j] + f[j];
                        }
			clock_gettime( CLOCK_REALTIME, &stop);
			accum = ( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION;
			if(accum <= empty){
				goto redo15;
			}
			fwtr_avg += ((6*size*1.0E-06)/(long double)(accum - empty));
redo16:
			clock_gettime( CLOCK_REALTIME, &begin);
#pragma omp parallel for
                        for(j =0; j < (size/sizeof(double)); j++){
                            a[j] = c[j] = d[j] = b[j] = e[j] + f[j] +g[j];
                        }
			clock_gettime( CLOCK_REALTIME, &stop);
			accum = ( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION;
			if(accum <= empty){
				goto redo16;
			}
			fwthr_avg += ((7*size*1.0E-06)/(long double)(accum - empty));
redo17:
			clock_gettime( CLOCK_REALTIME, &begin);
#pragma omp parallel for
                        for(j =0; j < (size/sizeof(double)); j++){
                            a[j] = c[j] = d[j] = b[j] = e[j] + f[j] +g[j] + h[j];
                        }
			clock_gettime( CLOCK_REALTIME, &stop);
			accum = ( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION;
			if(accum <= empty){
				goto redo17;
			}
			fwfr_avg += ((8*size*1.0E-06)/(long double)(accum - empty));
redo18:
			stride = 0;
			clock_gettime( CLOCK_REALTIME, &begin);
#pragma omp parallel for
                        for(j =0; j < (size/sizeof(double)); j++){
                            a[stride%(size/sizeof(double))] = c[stride%(size/sizeof(double))] = d[stride%(size/sizeof(double))] = b[stride%(size/sizeof(double))] = e[stride%(size/sizeof(double))] + f[stride%(size/sizeof(double))] +g[stride%(size/sizeof(double))] + h[stride%(size/sizeof(double))];
			    stride +=3;
                        }
                        clock_gettime( CLOCK_REALTIME, &stop);
                        accum = ( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION;
			if(accum <= empty){
				goto redo18;
			}
			str_avg += ((8*size*1.0E-06)/(long double)(accum - empty));
redo19:
			stride = 0;
                        clock_gettime( CLOCK_REALTIME, &begin);
#pragma omp parallel for
                        for(j =0; j < (size/sizeof(double)); j++){
                            a[stride%(size/sizeof(double))] = c[stride%(size/sizeof(double))] = d[stride%(size/sizeof(double))] = b[stride%(size/sizeof(double))] = e[stride%(size/sizeof(double))] + f[stride%(size/sizeof(double))] +g[stride%(size/sizeof(double))] + h[stride%(size/sizeof(double))];
			    if((j%8 == 0)&&(j != 0))
                            	stride = (int)(((j+13)*37)/((j+10)*23));
			    else
				stride++;
                        }
                        clock_gettime( CLOCK_REALTIME, &stop);
                        accum = ( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION;
			if(accum <= empty){
				goto redo19;
			}
			rand_avg += ((8*size*1.0E-06)/(long double)(accum - empty));
redo20:
                        clock_gettime( CLOCK_REALTIME, &begin);
#pragma omp parallel for
                        for(j =0; j < (size/sizeof(double)); j++){
                            a[j] = e[j] + f[j] +g[j] + h[j];
			    c[j] = e[j] + f[j] +g[j] + h[j];
			    d[j] = e[j] + f[j] +g[j] + h[j];
			    b[j] = e[j] + f[j] +g[j] + h[j];
                        }
                        clock_gettime( CLOCK_REALTIME, &stop);
                        accum = ( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION;
			if(accum <= empty){
				goto redo20;
			}
			diff_avg += ((8*size*1.0E-06)/(long double)(accum - empty));
redo21:
			clock_gettime( CLOCK_REALTIME, &begin);
#pragma omp parallel for
                        for(j =0; j < (r_size/sizeof(double*)); j++){
				for(k = 0; k < (c_size/sizeof(double)); k++)
                            		aa[j][k] = bb[j][k]*cc[j][k];
                        }
			clock_gettime( CLOCK_REALTIME, &stop);
			accum = ( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION;
			if(accum <= empty2){
				goto redo21;
			}
			row_avg += ((long double)(3*(long)r_size*c_size*1.0E-06)/(long double)(accum - empty2));
redo22:
			clock_gettime( CLOCK_REALTIME, &begin);
#pragma omp parallel for
                        for(j =0; j < (r_size/sizeof(double*)); j++){
				for(k = 0; k < (c_size/sizeof(double)); k++)
                            		aa[k][j] = bb[k][j]*cc[k][j];
                        }
			clock_gettime( CLOCK_REALTIME, &stop);
			accum = ( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION;
			if(accum <= empty2){
				goto redo22;
			}
			col_avg += ((long double)(3*(long)r_size*c_size*1.0E-06)/(long double)(accum - empty2));
redo23:
			clock_gettime( CLOCK_REALTIME, &begin);
#pragma omp parallel for
                        for(j =0; j < (r_size/sizeof(double*)); j++){
				for(k = 0; k < (c_size/sizeof(double)); k++)
                            		aa[j][k] = bb[j][k]*cc[k][j];
                        }
			clock_gettime( CLOCK_REALTIME, &stop);
			accum = ( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION;
			if(accum <= empty2){
				goto redo23;
			}
			rc_avg += ((long double)(3*(long)r_size*c_size*1.0E-06)/(long double)(accum - empty2));
redo24:
			clock_gettime( CLOCK_REALTIME, &begin);
#pragma omp parallel for
                        for(j =0; j < (r_size/sizeof(double*)); j++){
				for(k = 0; k < (c_size/sizeof(double)); k++)
					if((k!=0)&&(k!=((c_size/sizeof(double))-1))&&(j!=0)&&(j!=((r_size/sizeof(double*))-1)))
                            			aa[j][k] = aa[j][k-1]+aa[j][k+1] + aa[j-1][k] + aa[j+1][k];
                        }
			clock_gettime( CLOCK_REALTIME, &stop);
			accum = ( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION;
			if(accum <= empty2){
				goto redo24;
			}
			f_sten_avg += ((long double)(((long)r_size-1)*((long)c_size-1)*1.0E-06)/(long double)(accum - empty2));
redo25:
			clock_gettime( CLOCK_REALTIME, &begin);
#pragma omp parallel for
                        for(j =0; j < (r_size/sizeof(double*)); j++){
                                for(k = 0; k < (c_size/sizeof(double)); k++)
                                        if((k!=0)&&(k!=((c_size/sizeof(double))-1)))
                                                aa[j][k] = aa[j][k-1]+aa[j][k+1];
                        }
                        clock_gettime( CLOCK_REALTIME, &stop);
                        accum = ( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION;
			if(accum <= empty2){
				goto redo25;
			}
			t_sten_avg += ((long double)(((long)r_size)*(c_size)*1.0E-06)/(long double)(accum - empty2));
redo26:
			clock_gettime( CLOCK_REALTIME, &begin);
#pragma omp parallel for
                        for(j =0; j < (r_size/sizeof(double*)); j++){
                                for(k = 0; k < (c_size/sizeof(double)); k++)
                                        if((k!=0)&&(k!=((c_size/sizeof(double))-1))&&(j!=0)&&(j!=((r_size/sizeof(double*))-1)))
                                                aa[j][k] = aa[j][k-1]+aa[j][k+1] + aa[j-1][k] + aa[j+1][k] + aa[j-1][k-1] + aa[j-1][k+1] + aa[j+1][k-1] + aa[j+1][k+1];
                        }
                        clock_gettime( CLOCK_REALTIME, &stop);
                        accum = ( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION;
			if(accum <= empty2){
				goto redo26;
			}
			n_sten_avg += ((long double)(((long)r_size-1)*((long)c_size-1)*1.0E-06)/(long double)(accum - empty2));
redo27:
			stride = 0;
                        clock_gettime( CLOCK_REALTIME, &begin);
#pragma omp parallel for
                        for(j =0; j < (size/sizeof(double)); j++){
                            a[stride%(size/sizeof(double))] = h[stride%(size/sizeof(double))];
			    if((j%8 == 0)&&(j != 0))
				stride = j*65536;
			    else
				stride++;
                        }
                        clock_gettime( CLOCK_REALTIME, &stop);
                        accum = ( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION;
                        if(accum <= empty){
                                goto redo27;
                        }
                        l2cache_avg += ((2*size*1.0E-06)/(long double)(accum - empty));
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
		node_bw->owor_avg = owor_avg/10;
		node_bw->owtr_avg = owtr_avg/10;
		node_bw->owthr_avg = owthr_avg/10;
		node_bw->owfr_avg = owfr_avg/10;
		node_bw->twor_avg = twor_avg/10;
		node_bw->twtr_avg = twtr_avg/10;
		node_bw->twthr_avg = twthr_avg/10;
		node_bw->twfr_avg = twfr_avg/10;
		node_bw->thwor_avg = thwor_avg/10;
		node_bw->thwtr_avg = thwtr_avg/10;
		node_bw->thwthr_avg = thwthr_avg/10;
		node_bw->thwfr_avg = thwfr_avg/10;
		node_bw->fwor_avg = fwor_avg/10;
		node_bw->fwtr_avg = fwtr_avg/10;
		node_bw->fwthr_avg = fwthr_avg/10;
		node_bw->fwfr_avg = fwfr_avg/10;
		node_bw->str_avg = str_avg/10;
		node_bw->rand_avg = rand_avg/10;
		node_bw->diff_avg = diff_avg/10;
		node_bw->row_avg = row_avg/10;
		node_bw->col_avg = col_avg/10;
		node_bw->rc_avg = rc_avg/10;
		node_bw->l2cache_avg = l2cache_avg/10;
		node_bw->t_sten_avg = t_sten_avg/10;
		node_bw->f_sten_avg = f_sten_avg/10;
		node_bw->n_sten_avg = n_sten_avg/10;
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
	classify();
	write_config_file();
}
