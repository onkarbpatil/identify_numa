#!/usr/bin/bash

make clean
make

#echo "OMP"
#thr=8
#export OMP_NUM_THREADS=1
#./stream_numa_char OMP
#cp sicm_numa_config sicm_numa_config_o1
#while [ $thr -lt 97 ]; do
#	export OMP_NUM_THREADS=$thr
#	./stream_numa_char OMP >> memory_chararcterization
#	cp sicm_numa_config "sicm_numa_config_o$thr"
#	let thr=thr+8
#done

echo "MPI"

thr=8
pn=1
export OMP_NUM_THREADS=1
srun -n 1 ./stream_numa_char MPI
#cp sicm_numa_config sicm_numa_config_o1
while [ $thr -lt 97 ]; do
#	export OMP_NUM_THREADS=$thr
	mpirun -np $thr -ppn $pn ./stream_numa_char MPI >> memory_chararcterization
#	cp sicm_numa_config "sicm_numa_config_o$thr"
	let thr=thr+8
	let pn=pn/12
	if (( $pn%12 != 0 ))
	then
		let pn=pn+1
	fi
done
