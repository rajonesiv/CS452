#include <iostream>
using namespace std;

#include <cstdlib>
#include <cmath>
#include <cstdio>

#include <string>
#include "mpi.h" // message passing interface

void merge_sort(int* A, int* B, int n, int my_rank, int p);		//A = Original Array, B = Sorted Array, n = length of array

//void s_merge(int* A, int* B, int* C, int n);	//Merge Arrays A and B into Array C
//void p_merge(int* A, int* B, int* C, int n, int my_rank, int p);	//Merge Arrays A and B into Array C
//void clear(int* A, int n);
//void sort(int* A, int n);

//Array Functions
void clear( int* A, int n)		//Sets everything to 0
{
	for(int i = 0; i < n; i++)
		A[i] = 0;
}
void print(int* A, int n)		//Prints out array
{
	cout<<"[";
	for(int i = 0; i < n; i++)
		printf("%4d ",A[i]);
	cout<<"]"<<endl;
}

int main(int argc, char * argv[]){
	int my_rank;			// my CPU number for this process
	int p;					// number of CPUs that we have
	int source;				// rank of the sender
	int dest;				// rank of destination
	int tag = 0;			// message number
	char message[100];		// message itself
	MPI_Status status;		// return status for receive
	
	// Start MPI
	MPI_Init(&argc, &argv);
	
	// Find out my rank!
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	
	// Find out the number of processes!
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	
	
	//Step 0 - Make Variables and shit
	bool print_results = true;	//set to false to not print results
	srand(time(NULL));
	int power = 10;
	int size = pow(2,power);
	int int_max = size * 4;
	int* A = new int[size];		//Original Array - Fill with Randoms
	int* B = new int[size];		//Sorted Array - Fill with 0s
	for(int i = 0; i < size; i++)
	{
		A[i] = rand() % int_max;
		B[i] = 0;
	}
	
	//Step 1 - Call Sort
	merge_sort(A, B, size, my_rank, p);		//All Processes call this
	if(print_results && my_rank == 0){
		cout<<"\n\nA:"<<endl;
		print(A, size);
		cout<<"B:"<<endl;
		print(B, size);
	}

	// Shut down MPI
	MPI_Finalize();
	
	return 0;
}



//Basic insertion sort
void sort(int* A, int n){
	int i, j, key, len = n;
	for(j = 1; j < len; j++){
		key = A[j];
		for(i = j -1; (i>=0) && (A[i]>key); i--)
			A[i+1] = A[i];
		A[i+1] = key;
	}
}

//Used to find the position, in A, that "value" would be placed.
int RANK(int* A, int n, int value)
{
	if (n == 1)
		if(value<A[0])
			return 0;
		else
			return 1;
	else{
		if(value < A[n/2])
			return RANK(A, n/2, value);
		else
			return n/2 + RANK(&A[n/2], n/2, value);
	}
}

//Sequential Merge(one process)
//Merges A[a_start...a_end] with B[b_start...b_end] into C[c_start...c_end]
void s_merge(int* A, int a_start, int a_end, int* B, int b_start, int b_end, int* C, int c_start, int c_end)
{
	for(int i = c_start; i < c_end; i++)
	{
		//If there are elements left in A and either there are no elements left in B, 
		//or the next A element is less than the next B element, place the next A element
		//In the next C spot
		if(a_start < a_end && (b_start == b_end || A[a_start] < B[b_start])){
			C[i] = A[a_start++];
		}
		else{		//Else place the next B element in the next C spot
			C[i] = B[b_start++];
		}
	}
}

//Parallel merge function(all processes)
void p_merge(int* A, int* B, int* C, int n, int my_rank, int p)		//Merge Array A and B, both size n/2, into array C, size n
{
	int logn = log2(n/2);
	int x = ceil((n/2)/logn);		//Number of sampled elements
	
	//Arrays to hold positions and ranks used to determine end points of sub arrays to merge using sequential merge
	//lower case is used for local arrays to be filled out by individual processes and then all reduced into capital arrays
	//i.e Allreduce(ar, AR, n, ...)
	int* ar = new int[2*x + 2];
	int* br = new int[2*x + 2];
	int* Ar = new int[2*x + 2];
	int* Br = new int[2*x + 2];
	clear(ar,2*x+2);
	clear(br,2*x+2);
	clear(Ar,2*x+2);
	clear(Br,2*x+2);
	
	for(int i = my_rank; i < x; i+=p){		//Fill in positions of sampled elements(in parallel)
		ar[i] = 1 + i*logn;
		br[i] = 1 + i*logn;
	}		
	for(int i = my_rank; i < x; i+=p){		//Fill in ranks of sampled elements(in parallel)
		br[i+x] = RANK(B, n/2, A[0+i*logn]);
		ar[i+x] = RANK(A, n/2, B[0+i*logn]);
	}
	MPI_Allreduce(ar, Ar, 2*x+2, MPI_INT, MPI_SUM, MPI_COMM_WORLD);		//combine local arrays and broadcast
	MPI_Allreduce(br, Br, 2*x+2, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
	
	Ar[2*x] = 0;				//Fill in First Position(i = 0)
	Ar[2*x + 1] = n/2;			//Fill in Last Position(i = n)
	Br[2*x] = 0;				//Fill in First Position(i = 0)
	Br[2*x + 1] = n/2;			//Fill in Last Position(i = n)
	sort(Ar, 2*x+2);			//Sort these to make sure they are in order
	sort(Br, 2*x+2);			//Sorted using insertion sort
	
	int* localC = new int[n];		//Local C array for the sequnetial merges performed by individual process by striping.
	clear(localC, n);
	
	//Striping the sequential merges
	for(int i = my_rank; i < 2*x+1; i+=p){
		s_merge(A, Ar[i], Ar[i+1], B, Br[i], Br[i+1], localC, Ar[i] + Br[i], Ar[i+1]+Br[i+1] );
	}
	
	//Combine each process merges into the resulting C array
	MPI_Allreduce(localC, C, n, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
	
}

//Recursive merge sort function(all processes)
void merge_sort(int* A, int* B, int n, int my_rank, int p)
{
	//Base Case: if array less than 4, just insertion sort
	if(n == 4){
		for(int i = 0; i < n; i++){
			B[i] = A[i];
		}
		sort(B, n);
	}
	else
	{
		int* C = new int[n];		//Temp array to hold two individually, recursively sorted halves that have yet to be merged into B
		clear(C, n);
		merge_sort(&A[0], &C[0], n/2, my_rank, p);			//Recurse on first half
		merge_sort(&A[n/2], &C[n/2], n - n/2, my_rank, p);	//Recurse on second half
		p_merge(&C[0], &C[n/2], &B[0], n, my_rank, p);		//Merge in parallel
	}
}