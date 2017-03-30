#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;
#define MAX_TREE_SIZE 256
void get_input(const char fileName[], int array[], int &size, int &search);
void get_input_from_master(int array[], int &size, int &search);
void populate_binary_tree(int array[], int size, int tree[]);
void insert_into_tree(int tree[], int value);
int search_in_binary_tree(int tree[], int value);
void print_tree(int tree[], int index);
int main(int argc, char **argv)
{
	int rank;
	char hostname[256];
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	gethostname(hostname,255);

	//printf("Hello world!  I am process number: %d on host %s\n", rank, hostname);
	if(rank==0) {
		// send a message here!
		// printf("What message would you like to send to slave?\n");
		// scanf("%s", message);
		//MPI_Send(message, MSG_SIZE, MPI_BYTE, 1, 0, MPI_COMM_WORLD);
		/*
		The master will take the input from the file and the search query.
		Then, it will divide the elements and send to slave along with search query.
		The slaves will then respond if they found the element or not using binary search.
		*/
		int array[50], size, search;
		get_input("input.txt", array, size, search);
		// for(int i=0; i<size; i++)
		// 	cout<<array[i]<<" ";
		// cout<<endl<<"search: "<<array[size]<<endl;

		//send the first half of this array to slave1
		int meta[2]; meta[0] = size/2; meta[1] = search;
		// the meta array contains the meta data regarding the sent array
		MPI_Send(meta, 2, MPI_INT, 1, 0, MPI_COMM_WORLD);
		MPI_Send(array, meta[0], MPI_INT, 1, 0, MPI_COMM_WORLD);

		//and the other half to slave2
		meta[0] = size - meta[0];
		MPI_Send(meta, 2, MPI_INT, 2, 0, MPI_COMM_WORLD);
		MPI_Send(array+size/2, meta[0], MPI_INT, 2, 0, MPI_COMM_WORLD);

		//now get the responses from the slaves
		int foundSlave1;
		MPI_Recv(&foundSlave1, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		if(foundSlave1) {
			cout<<"Slave 1 has found "<<search<<"\n";
		}

		int foundSlave2;
		MPI_Recv(&foundSlave2, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		if(foundSlave2) {
			cout<<"Slave 2 has found "<<search<<"\n";
		}

		if(!(foundSlave1+foundSlave2)) {
			cout<<"The value "<<search<<" could not be found\n";
		}
	}
	else
	{
		// display the message here
		// MPI_Recv(incoming, MSG_SIZE, MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		// printf("The master sent: %s\n", incoming);
		// printf("The message's value is %s\n", message);
		int array[25], meta[2], size, search;
		get_input_from_master(array, size, search);
		// for(int i=0; i<size; i++) {
		// 	cout<<"Slave"<<rank<<": "<<array[i]<<endl;
		// }
		// cout<<"Slave"<<rank<<": search is "<<search<<endl;
		//now make the binary tree from the array and search for the element
		int tree[MAX_TREE_SIZE];
		fill_n(tree, MAX_TREE_SIZE, -1);
		// cout<<"tree"<<tree[2]<<endl;
		// cout<<"now calling populate binary tree\n";
		populate_binary_tree(array, size, tree);
		
		cout<<rank<<":";
		print_tree(tree, 1);
		cout<<endl;
		
		int found = search_in_binary_tree(tree, search);
		if(found<=0) found = 0; else found = 1;
		MPI_Send(&found, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

	}

	MPI_Finalize();

	return 0;
}

void get_input(const char fileName[], int array[], int &size, int &search) 
{
	cout<<"Now reading from file: "<<fileName<<"...\n";
	ifstream iff;
	iff.open(fileName);
	if(!iff.is_open()) {
		cout<<"Cannot open the file. Aborting\n";
		exit(0);
	}
	size=0;
	while(!iff.eof()) {
		iff>>array[size++];
	}
	iff.close();
	size = size-1;
	search = array[size];	
}

void get_input_from_master(int array[], int &size, int &search)
{
	int meta[2];
	MPI_Recv(meta, 2, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	MPI_Recv(array, meta[0], MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	size = meta[0];
	search = meta[1];
}
void populate_binary_tree(int array[], int size, int tree[])
{
	// the tree starts at index 1
	tree[0] = size;
	for(int i=0; i<size; i++) {
		// cout<<"now inserting "<<array[i]<<" in tree\n";
		insert_into_tree(tree, array[i]);
		// cout<<"successfully inserted "<<array[i]<<" into tree\n";
	}
}
void insert_into_tree(int tree[], int value)
{
	int index = 1;
	// cout<<"Now searching for "<<value<<" in the tree\n";
	index = search_in_binary_tree(tree, value);
	// cout<<"search returned "<<index<<" upon inserting "<<value<<endl;
	if(index>=0)
	{
		cout<<"Cannot insert "<<value<<", it already exists. Aborting\n";
		exit(0);
	}
	index = index*-1;
	tree[index] = value;
}
int search_in_binary_tree(int tree[], int value) 
{
	//this function returns the index
	// of the found element, else negative of 
	// the index where it should be placed
	int index = 1;

	//if the tree is empty, it won't go in
	// this while loop
	while(tree[index]!=-1) {
		// cout<<"searching, current element: "<<tree[index]<<endl;
		if(value < tree[index])
			index = index*2;
		else if (value > tree[index])
			index = index*2 + 1;
		else {
			// the element was found, return the index
			return index;
		} 
	}
	return -1*index;
}
void print_tree(int tree[], int index)
{
	if(tree[index]==-1)
		return;
	print_tree(tree, index*2);
	
	// print left child if it exists
	if(tree[index*2]!=-1) {
		cout<<tree[index*2]<<"<--";
	}
	cout<<"|"<<tree[index]<<"|";
	if(tree[index*2+1]!=-1) {
		cout<<"-->"<<tree[index*2+1];
	}
	cout<<"  ";

	print_tree(tree, index*2+1);
}

/*
--------OUTPUT--------:
[pict@theMaster A1]$ ./copy.sh bst.cpp
a.out                                                                    100%  110KB 109.8KB/s   00:00    
a.out                                                                    100%  110KB 109.8KB/s   00:00    
[pict@theMaster A1]$ mpirun -np 3 --hostfile hostfile /home/pict/MMR_CL4/A1/a.out
Now reading from file: input.txt...
1:|12|  12<--|41|  41<--|49|  49<--|65|-->98  |71|-->72  |72|-->95  |77|  77<--|95|  71<--|98|  
The value 110 could not be found
2:|9|  9<--|14|-->45  |31|  31<--|45|-->81  |52|-->58  |58|-->62  |62|  52<--|81|-->84  |84|-->91  |91|  
[pict@theMaster A1]$ mpirun -np 3 --hostfile hostfile /home/pict/MMR_CL4/A1/a.out
Now reading from file: input.txt...
1:|12|  12<--|41|  41<--|49|  49<--|65|-->98  |71|-->72  |72|-->95  |77|  77<--|95|  71<--|98|  
Slave 2 has found 58
2:|9|  9<--|14|-->45  |31|  31<--|45|-->81  |52|-->58  |58|-->62  |62|  52<--|81|-->84  |84|-->91  |91|  
[pict@theMaster A1]$ mpirun -np 3 --hostfile hostfile /home/pict/MMR_CL4/A1/a.out
Now reading from file: input.txt...
2:|9|  9<--|14|-->45  |31|  31<--|45|-->81  |52|-->58  |58|-->62  |62|  52<--|81|-->84  |84|-->91  |91|  
Slave 1 has found 41
1:|12|  12<--|41|  41<--|49|  49<--|65|-->98  |71|-->72  |72|-->95  |77|  77<--|95|  71<--|98| 

--------input.txt--------:
65 98 49 71 72 41 95 12 77 14 45 81 52 9 84 58 91 62 31
41

--------hostfile--------:
localhost
theSlave1
theSlave2

--------copy.sh--------:
#!/bin/bash
mpic++ $1
HOSTS="theslave1 theslave2"
for HOSTNAME in ${HOSTS} ; do
    scp a.out pict@${HOSTNAME}:/home/pict/MMR_CL4/A1
done
*/