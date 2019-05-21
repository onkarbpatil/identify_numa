#include "identify_numa.h"

struct numa_node_bw * numa_node_list = NULL;
struct numa_node_bw * numa_list_head = NULL;
struct labelled_numa_nodes * type_id_list; 
int mem_types;
int max_node;
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
		long double delta = 999999999.9999999;
		while(i < mem_types){
			long double dist = abs(sqrt(abs((means[i] - bw_it->avg_bw))*abs((means[i] - bw_it->avg_bw))));
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
			means[i] += bw_it->avg_bw;
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
		if((bw_it->avg_bw > new_node->avg_bw)){
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

void insert_ids(){
	struct numa_node_bw * bw_it = numa_list_head;
        while(bw_it != NULL){
		int j = 0;
		while(strcmp(bw_it->mem_type, type_id_list[j].mem_type)!=0){
			j++;
		}
		type_id_list[j].numa_id[type_id_list[j].count] = bw_it->numa_id;
		type_id_list[j].count++;
		bw_it = bw_it->next;
	}
}

void write_config_file(){
	FILE * conf;
	conf = fopen("sicm_numa_config", "w");
	struct numa_node_bw * bw_it = numa_list_head;
	while(bw_it != NULL){	
		fprintf(conf, "%d %s %Lf %Lf %Lf %Lf %Lf %Lf %Lf %Lf %.10Lf %.10Lf %.10Lf %.10Lf %.10Lf %.10Lf %.10Lf %.10Lf\n",bw_it->numa_id, bw_it->mem_type, bw_it->avg_bw, bw_it->read_bw, bw_it->write_bw, bw_it->rw_bw, bw_it->ran_bw, bw_it->lin_bw,bw_it->sam_bw, bw_it->dif_bw, bw_it->avg_lat, bw_it->read_lat, bw_it->write_lat, bw_it->rw_lat, bw_it->ran_lat, bw_it->lin_lat, bw_it->sam_lat, bw_it->dif_lat);
		printf("%d %s %Lf %Lf %Lf %Lf %Lf %Lf %Lf %Lf %.10Lf %.10Lf %.10Lf %.10Lf %.10Lf %.10Lf %.10Lf %.10Lf\n",bw_it->numa_id, bw_it->mem_type, bw_it->avg_bw, bw_it->read_bw, bw_it->write_bw, bw_it->rw_bw, bw_it->ran_bw, bw_it->lin_bw, bw_it->sam_bw, bw_it->dif_bw, bw_it->avg_lat, bw_it->read_lat, bw_it->write_lat, bw_it->rw_lat, bw_it->ran_lat, bw_it->lin_lat, bw_it->sam_lat, bw_it->dif_lat);
		bw_it = bw_it->next;
	}
	fclose(conf);
}

struct labelled_numa_nodes * identify_numa(int no_of_labels, char ** labels){
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
	size_t size = 512*1024*1024;
	int *a, *b;
	clock_t start, end;
        struct timespec begin, stop;
	srand(clock());
	if(no_of_labels == 0){
		printf("Enter memory technologies available in ascending order of speed. eg: GPU NVRAM DRAM HBM\n");
		return;
	}
	else{
		mem_types = no_of_labels;
		mem_tech = (char**)malloc(no_of_labels*sizeof(char*));
		type_id_list = (struct labelled_numa_nodes *)malloc(no_of_labels*sizeof(struct labelled_numa_nodes));
		int a;
		for(a = 0; a < no_of_labels; a++){
			mem_tech[a] = labels[a];
			type_id_list[a].mem_type = labels[a];
			type_id_list[a].count = 0;
		}

	}
  	i = 0;
	while(i < total_numa_nodes){
		int k = 0;
		long double wbw_avg=0.0;
		long double rbw_avg=0.0;
        	long double rwbw_avg=0.0;
		long double ranbw_avg=0.0;
		long double linbw_avg=0.0;
		long double difbw_avg=0.0;
		long double sambw_avg=0.0;
		long double wlat_avg=0.0;
		long double rlat_avg=0.0;
		long double rwlat_avg=0.0;
		long double ranlat_avg=0.0;
		long double linlat_avg=0.0;
		long double diflat_avg=0.0;
		long double samlat_avg=0.0;
		for(k = 0; k < 1; k++){
			a = (int*)numa_alloc_onnode(size, numa_node_ids[i]);
			b = (int*)numa_alloc_onnode(size, numa_node_ids[i]);
			volatile int j = 0;


			clock_gettime( CLOCK_REALTIME, &begin);
#pragma omp parallel for
			for(j = 0; j < (size/sizeof(int)); j++)
				a[j] = 1;
			clock_gettime( CLOCK_REALTIME, &stop);
			wbw_avg += 512/(( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION);
			wlat_avg += ( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION;
			int t;
			clock_gettime( CLOCK_REALTIME, &begin);
#pragma omp parallel for
			for(j = 0; j < (size/sizeof(int)); j++)
                	        t = a[j];
			clock_gettime( CLOCK_REALTIME, &stop);
			t= t + j;
			rbw_avg += 512/(( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION);
			rlat_avg += ( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION;
			clock_gettime( CLOCK_REALTIME, &begin);
#pragma omp parallel for
			for(j = 0; j < (size/sizeof(int)); j++)
                            a[j] += a[j];
			clock_gettime( CLOCK_REALTIME, &stop);
            		rwbw_avg += 512/(( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION);
            		rwlat_avg += ( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION;
			clock_gettime( CLOCK_REALTIME, &begin);
#pragma omp parallel for
			for(j = 0; j < (size/sizeof(int)); j++)
                            a[((int)rand())%((int)(size/sizeof(int)))] += a[((int)rand())%((int)(size/sizeof(int)))];
			clock_gettime( CLOCK_REALTIME, &stop);
            		ranbw_avg += 512/(( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION);
            		ranlat_avg += ( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION;
			clock_gettime( CLOCK_REALTIME, &begin);
#pragma omp parallel for
			for(j = 0; j < (size/sizeof(int)); j++)
                            a[(j+10)%((int)(size/sizeof(int)))] += a[(j+2*j)%((int)(size/sizeof(int)))];
			clock_gettime( CLOCK_REALTIME, &stop);
            		linbw_avg += 512/(( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION);
            		linlat_avg += ( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION;
			clock_gettime( CLOCK_REALTIME, &begin);
#pragma omp parallel for
			for(j = 0; j < (size/sizeof(int)); j++)
                            a[j] += b[j];
			clock_gettime( CLOCK_REALTIME, &stop);
                        sambw_avg += 512/(( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION);
                        samlat_avg += ( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION;
			clock_gettime( CLOCK_REALTIME, &begin);
#pragma omp parallel for
			for(j = 0; j < (size/sizeof(int)); j++){
                            a[j] += 20 +a[j];
			    b[j] += 40 +b[j];
                        }
			clock_gettime( CLOCK_REALTIME, &stop);
                        difbw_avg += 512/(( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION);
                        diflat_avg += ( stop.tv_sec - begin.tv_sec ) + (long double)( stop.tv_nsec - begin.tv_nsec ) / (long double)BILLION;
			numa_free(a, size);
			numa_free(b, size);
		}
		struct numa_node_bw * node_bw = (struct numa_node_bw *)malloc(sizeof(struct numa_node_bw));
		node_bw->numa_id = numa_node_ids[i];
		node_bw->write_bw = wbw_avg/1;
		node_bw->read_bw = rbw_avg/1;
		node_bw->rw_bw = rwbw_avg/1;
		node_bw->ran_bw = ranbw_avg/1;
		node_bw->lin_bw = linbw_avg/1;
		node_bw->dif_bw = difbw_avg/1;
		node_bw->sam_bw = sambw_avg/1;
		node_bw->write_lat = ((long double)wlat_avg/(long double)(size/sizeof(int)))/1;
		node_bw->read_lat = ((long double)rlat_avg/(long double)(size/sizeof(int)))/1;
        	node_bw->rw_lat = ((long double)rwlat_avg/(long double)(size/sizeof(int)))/1;
        	node_bw->ran_lat = ((long double)ranlat_avg/(long double)(size/sizeof(int)))/1;
        	node_bw->lin_lat = ((long double)linlat_avg/(long double)(size/sizeof(int)))/1;
		node_bw->dif_lat = ((long double)diflat_avg/(long double)(size/sizeof(int)))/1;
		node_bw->sam_lat = ((long double)samlat_avg/(long double)(size/sizeof(int)))/1;
		node_bw->avg_bw = (node_bw->write_bw + node_bw->read_bw)/2;
		node_bw->avg_lat = (node_bw->write_lat + node_bw->read_lat)/2;
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
	insert_ids();
	//write_config_file();
	return type_id_list;
}
