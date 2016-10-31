
#include <cstdlib>
#include <stdint.h>

#ifndef BUFFER_H_
#define BUFFER_H_

#define N 15

class Node {
	//int id; na to prosthesoume gia extra sigouria?
	int neighbor[N]; //the ids of the neighbor nodes
	int endPos; // pointing to first empty cell of array.
	int maxCapacity; // maxCapacity of arrays|Needed for reallocation.
	int edgeProperty[N]; //property for each edge
	int nextNode;
public:
	Node();
	~Node();
	void Init();

	int GetNextNode();

	int AddNeighbor(int); // return 0 for ok and -1 for needing extra setting a nextNode from buffer class.
	void SetNextNode(int);

	void PrintNeightbors();
};


struct index_node {
	//int id; // id of corresponding graph Node
	int in; // offset of incoming node
	int out; // offset of outcoming node
	bool initIn; // check parameter if node is initialized
	bool initOut;
	//index_node() :in(0),out(0),initIn(false),initOut(false) {}
};

class Buffer;

class Index {
	index_node *indexArray; // Dynamic array of graph indices
	int indSize;
public:
	Index();
	~Index();

	index_node* GetIndexNode();
	int GetSize() {return indSize;};

	void Insert(int,int,Buffer *);
	void Reallocate(int); // increase capacity of Index data type till == "int" | Realloc

	void Print();
};

class Buffer {
	Node *incoming; // Dynamic array of graph incoming adjacency list
	int incSize; // Size of incoming array.Needed for realloc
	int incEnd; // first empty cell of array.(The next of the last cell)
	Node *outcoming; // Dynamic array of graph outcoming adjacency list
	int outSize;
	int outEnd;
public:
	Buffer();
	~Buffer();

	Node* GetListNode(char);
	int GetIncEnd();
	int GetOutEnd();

	void InitBuffer(char , int); // initializing Buffer starting from "int"-th cell till maxSize
	void InsertBuffer(int,int,Index*);
	void IncreaseEndPos(char c);
	int DestroyBuffer();
	/*
	 * Increase capacity of Buffer data type.
	 * if char == 'i' then realloc incoming array
	 * else if char == 'o' then realloc outcoming array.
	 */
	void Reallocate(char);
	void PrintBuffer(Index *);
};

#endif /* BUFFER_H_ */
