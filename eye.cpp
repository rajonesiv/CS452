//Andrew Jones
//In Your Eye Program
//This program will read in a text file of randomized letters and 
//produce an output giving the number of each letter in the file

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h" // message passing interface
using namespace std;

// Do this ONCE when you start up thomas
// module load openmpi-x86_64

// New compile and run commands for MPI!
// mpicxx -o blah file.cpp
// mpirun -np 32 blah

int main (int argc, char * argv[]) {

	int my_rank;			// my CPU number for this process
	int p;					// number of CPUs that we have
	int source;				// rank of the sender
	int dest;				// rank of destination
	int tag = 0;			// message number
	char message[100];		// message itself
	MPI_Status status;		// return status for receive
	
	char c;  //the letter
	int v;
	
	// Start MPI
	MPI_Init(&argc, &argv);
	
	// Find out my rank!
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	
	// Find out the number of processes!
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	
	// THE REAL PROGRAM IS HERE
	ifstream myfile;
	myfile.open("randLet.txt");  //created a file and I randomly filled letters in
	
	//v = rand() % 150 + 10; //give the file a variety in length
	//myfile << v << endl;

	/*
	for (int x=0; x<v; x++){
		n = rand() % 26;
		c = (char)(n+97);
		myfile<< c << endl; 
		myfile<< x << endl;
	}
	*/
	
	myfile >> n; // the number of letters in the file
	char * array = new char[n]; //create a dynamic array for the letters
	if(my_rank == 0) {
		for(int x=0; x<n; x++) {
			myfile >> array[x];
		}
	}
	
	// DIVIDE THE PROBLEM UP
	
	int localn = n/p;
	double * locala = new double[localn];
	
	MPI_Scatter(&array[0], localn, MPI_CHAR, locala, localn, MPI_CHAR, 0, MPI_COMM_WORLD);
	
	
	// DO THE LOCAL WORK
	
	char freq[] = {'a','b','c','d','e','f','g','h','j','k','l','m','n','o','p',
		'q','r','s','t','u','v','w','x','y','z'}; //create an array of letters
	
	for (int i = 0; i < localn; i++){
		for(int j=0; j<25; j++ ) {
			if (array[i]=freq[j])
				freq[j]++ ;				//if the letter in the array is equal to the letter of the frequency array,
		}										//then add 1 to the frequency of that letter
	}
	
	double answer[26];
	for(int k=0;k<25; k++ ){
		MPI_Reduce(&freq[k], &answer[k], 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
	}
	
	for(int j =0;j<26;j++){
		cout<< answer[j] << endl;  //print out the answer
	}
	
	myfile.close();
	
	// Shut down MPI
	MPI_Finalize();

	return 0;
}