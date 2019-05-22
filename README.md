# identify_numa
Identify the type of memory underneath NUMA ID

This library helps you associate numa ids to a memory technology.
The assumption here is that you know the memory technologies used in the system you are operating on(DRAM,HBM,NVM).

Look at the test program to understand how to use the library.
The API to the library is identify_numa(int no_of_labels, char ** labels) where, 
no_of_labels is the number of memory technologies in your system and 
labels is the array of string containing the labels of memory technologies in ascending order of bandwidth speed.
eg: char **labels = {"NVM", "DRAM", "HBM"}; identify_numa(2, labels);
The API returns a list of Numa IDs associated with each label, of the type struct labelled_numa_nodes *
Use the member count to traverse the numa_id[] member and mem_type to identify the memory technology.

#numatest

This library is similar to identify_numa where it does identify the type of numa node but it characterizes the memory further.
The API only prints performance numbers.
