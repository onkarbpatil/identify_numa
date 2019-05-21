#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <sched.h>
#include <sys/time.h>
#include <omp.h>
#include "numa.h"

#define BILLION  1000000000L

struct numa_node_bw{
	int numa_id;
	char * mem_type;
	long double read_bw;
	long double write_bw;
	long double rw_bw;
	long double ran_bw;
	long double lin_bw;
	long double sam_bw;
	long double dif_bw;
	long double read_lat;
	long double write_lat;
	long double rw_lat;
	long double ran_lat;
	long double lin_lat;
	long double sam_lat;
	long double dif_lat;
	long double avg_lat;
	long double avg_bw;
	struct numa_node_bw * next;
};

struct labelled_numa_nodes{
	char * mem_type;
	int numa_id[40];
	int count;
};

struct numa_node_bw * numa_node_list;
struct numa_node_bw * numa_list_head;
struct labelled_numa_nodes * type_id_list; 
int mem_types;
int max_node;
int total_numa_nodes;
int * numa_node_ids;
struct bitmask * numa_nodes;
char ** mem_tech;
long double * means;
int * cluster_sizes;

void calculate_distances();
void calculate_mean();
void classify();
void sort_list(struct numa_node_bw * new_node);
void insert_ids();
void write_config_file();
struct labelled_numa_nodes * identify_numa(int no_of_labels, char ** labels);
