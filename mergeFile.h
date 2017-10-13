/**Andrew Jones
*Merge sort Program
*The program is designed to take two arrays, sort them,
*and then merge them together
*This code is for reading in the file
*/
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using namespace std

int main () {
	
	srand(time(NULL));
	char c;  //the letter
	int v, n;
	
	ofstream myfile;
	cout << "Enter a file name: " << endl;
	getline( cin, filename );
	myfile.open( filename.c_str() );
	v = rand() % 150 + 10; //give the file a variety in length
	char * a = new char a[v];
	n = rand() % ;

	myfile << n << endl;
	myfile.close();
	return 0;
}