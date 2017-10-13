/**Andrew Jones
* CS 452,  Merge Sort Program
*This program is designed to use a recursive function for the merge sort algorithm.
*It takes two arrays of size n, merges and sorts them, and combines them into a
*third array.
*/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
using namespace std;

void merge(int * a, int first, int last, int mid) {
	int *b = new int[last - first + 1]; //the array (size should be n)
	int left = first; //setting the left and right sides of the array
	int right = mid + 1;
	for(int i = first; i < last; i++) {
		//comparing the array to make sure it's bigger than one
		if(right > last || left <= mid && a[left] <= a[right]) {
			b[i] = a[left]; //fill the b array with elements from the first half
			left++; //increment the element position
		} else {
			b[i] = a[right]; //fill the b array with elements from the second half
			right++; //increment the element position
		}
	}
	for(int i = first; i < last; i++) {
		a[i] = b[i - first]; 
	}
}

void mergesort(int * a, int first, int last) {
	int temp;
	int mid = (first + last) / 2;
	//fill out the base cases
	//array of size 0 or 1
	if(last <= first) { return; }
	//array of size 2 
	if(first + 1 == last) {
		if(a[first] > a[last])
			//swap the two numbers so that they are in order
			temp = a[first]; a[first] = a[last]; a[last] = temp;
			return;
	} else {  //the recursive cases will be here
		mergesort(a, first, last);
		mergesort(a, mid + 1, last);
		//merge the two halves
		merge(a, first, last, mid);
	}
}

int main () {
	int n;
	int i = 1; // index values of A and B
	int * a = new int[n];  //array a of size n
	int * b = new int[n];  //array b of size n
	int first, last;  //the range of the array.
	int mid = (first + last) / 2;  //the middle of the array
	
	srand(time(NULL));
	
	cout << "Enter the size of the array: " << endl;
	cin >> n;
	for (int i = 0; i < n; i++) {
		a[i] = (rand() % 1000000) + 1; //numbers range from 0 to 1000000
	}

	mergesort(a, first, last);
	
	cout << "This is the final array: ";
	for(int i = 0; i < n; i++){
		cout << a[i] << " "; 	//print out the array values
	}
	cout << endl;
	delete [] a; //delete array a
	delete []b;  //delete array b
	return 0;
}

/* SAMPLE RUN:
thomas% g++ mergesort.cpp
thomas% ./a.out
Enter the size of the array:
10
This is the final array: 641173 434794 286073 533045 328163 388434 83992 802510 951386 644970
thomas%

thomas% g++ mergesort.cpp
thomas% ./a.out
Enter the size of the array:
3
This is the final array: 446140 816464 888440
thomas%
*/