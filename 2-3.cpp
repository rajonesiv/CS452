/* Author: Andrew Jones
 * Project: 2-3 Tree
 * CS452 Professor Gupta
 */
 
 using namespace std;

class Node {
	Node * parent;
	int value[6];
	Node * child[6];
	
	public:
	Node(int val); //constructor
	int numChildren();
	void absorb(Node * newChild);
	void discard(Node * removeChild);
}

class Tree {
	Node * root;
	void print(Node * start);
	
	public:
	Tree(); //constructor
	Node * search(int valToFind);
	bool insert(int valToAdd);
	bool del(int valToKill);
	void print();
}